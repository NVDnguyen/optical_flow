#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <math.h>
#include "nv_optical_flow.h"
#define STB_IMAGE_IMPLEMENTATION
#include "nv_ov2640.h"

#define MAX_BUFFER_SIZE (WIDTH * HEIGHT)
#define PYR_SIZE (MAX_BUFFER_SIZE + (MAX_BUFFER_SIZE >> 2))
#define Q15_SHIFT 14
#define GRAD_SCALE_FACTOR (1 << Q15_SHIFT)
#define Q15_TO_PIXEL(val) ((val) >> Q15_SHIFT)

// Define M_PI if not provided
#ifndef M_PI
#define M_PI 3.14159265358979323846f
#endif

typedef enum {
  OK,
  ERROR,
  INVALID_SIZE,
  LOAD_FAIL
} status_t;

/********************************************************************************//**
 * Static Functions
 ***********************************************************************************/
static unsigned char pyr_buffer[PYR_SIZE];
static int16_t grad_buffer[MAX_BUFFER_SIZE];
static unsigned char gray_buffer[MAX_BUFFER_SIZE];
static unsigned char blurred_buffer[MAX_BUFFER_SIZE];


/********************************************************************************//**
 * Initialize application.
 ***********************************************************************************/
int main() {
    printf("Hello from xG24\nStarting...\n");
    ov2640_init(HEIGHT, WIDTH);
    int32_t p0[MAX_FEATURES * 2] = {0}, p1[MAX_FEATURES * 2] = {0};
    unsigned char *gray = gray_buffer;
    unsigned char *blurred = blurred_buffer;
    unsigned char *pyr[PYR_LEVELS];
    int16_t *gradx[PYR_LEVELS];
    int16_t *grady[PYR_LEVELS];
    int32_t total_dx = 0, total_dy = 0;
    int valid_count = 0;
    int num_features = 0;

    for (int frame = 1; frame <= 2; frame++) {
        uint16_t *image = NULL;
        ov2640_capture_frame(&image, frame);
        if (image == NULL) {
            printf("Error: Cannot load frame %d to RAM.\n", frame);
            return ERROR;
        }
        rgb565_to_grayscale(image, gray, WIDTH, HEIGHT);
        image = NULL;

        // Apply Gaussian blur
        gaussian_blur(gray, blurred, WIDTH, HEIGHT);
        memcpy(gray, blurred, WIDTH * HEIGHT); // Copy blurred image back to gray

        // Build image pyramid
        pyr[0] = pyr_buffer;
        pyr[1] = pyr_buffer + MAX_BUFFER_SIZE;
        memcpy(pyr[0], gray, WIDTH * HEIGHT);
        for (int l = 1; l < PYR_LEVELS; l++) {
            int w = WIDTH >> l;
            int h = HEIGHT >> l;
            build_image_pyramid(pyr[l - 1], pyr[l], w * 2, h * 2);
        }

        // Process frame 1: Compute gradients and find features
        if (frame == 1) {
            for (int l = 0; l < PYR_LEVELS; l++) {
                int w = WIDTH >> l;
                int h = HEIGHT >> l;
                gradx[l] = grad_buffer;
                grady[l] = grad_buffer + (w * h / 2);
                compute_gradient(pyr[l], gradx[l], grady[l], w, h);
                int idx = (h / 2) * w + (w / 2);
                printf("Gradient at (%d,%d): (%d,%d)\n", w / 2, h / 2, gradx[l][idx], grady[l][idx]);
            }

            num_features = find_strong_features(gray, WIDTH, HEIGHT, p0, MAX_FEATURES);
            printf("Found %d feature points:\n", num_features);
            for (int i = 0; i < num_features; i++) {
                printf("Feature %d: (%" PRId32 ",%" PRId32 ")\n", i, p0[i * 2] >> Q15_SHIFT, p0[i * 2 + 1] >> Q15_SHIFT);
            }
            if (num_features > 0) {
                printf("Valid feature points found\n");
            } else {
                printf("No valid feature points found\n");
            }
        } else if (num_features > 0) {
            // Compute optical flow for each feature point
            unsigned char *pyr2[PYR_LEVELS] = { pyr_buffer, pyr_buffer + MAX_BUFFER_SIZE };
            memcpy(pyr2[0], gray, WIDTH * HEIGHT);
            for (int l = 1; l < PYR_LEVELS; l++) {
                int w = WIDTH >> l;
                int h = HEIGHT >> l;
                build_image_pyramid(pyr2[l - 1], pyr2[l], w * 2, h * 2);
            }

            total_dx = 0;
            total_dy = 0;
            valid_count = 0;
            for (int i = 0; i < num_features; i++) {
                int32_t curr_p0[2] = { p0[i * 2], p0[i * 2 + 1] };
                int32_t curr_p1[2] = { 0, 0 };
                int valid = lucas_kanade_pyramid(pyr, pyr2, gradx, grady, curr_p0, curr_p1, WIDTH, HEIGHT, PYR_LEVELS);
                p1[i * 2] = curr_p1[0];
                p1[i * 2 + 1] = curr_p1[1];
                if (!valid) {
                    printf("Optical flow failed for feature %d\n", i);
                    continue;
                }
                int x1 = Q15_TO_PIXEL(p1[i * 2]);
                int y1 = Q15_TO_PIXEL(p1[i * 2 + 1]);
                if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT) {
                    printf("Optical flow result out of bounds for feature %d: (%d, %d)\n", i, x1, y1);
                    continue;
                }
                int32_t dx = p1[i * 2] - p0[i * 2];
                int32_t dy = p1[i * 2 + 1] - p0[i * 2 + 1];
                total_dx += dx;
                total_dy += dy;
                valid_count++;
                printf("Optical flow valid for feature %d: (%d, %d), dx=%" PRId32 ", dy=%" PRId32 "\n", i, x1, y1, dx, dy);
            }
        }
    }

    // Report results
    const int32_t THRESHOLD = 205; // 0.0125 in Q15
    if (valid_count > 0) {
        // Compute magnitude and angle of total optical flow vector
        float magnitude = sqrtf((float)(total_dx * total_dx + total_dy * total_dy)) / (1 << Q15_SHIFT);
        float angle = atan2f((float)total_dy, (float)total_dx) * 180.0f / M_PI;
        if (angle < 0) angle += 360.0f; // Normalize to [0, 360)

        printf("Total optical flow vector: dx=%" PRId32 ", dy=%" PRId32 "\n", total_dx, total_dy);
        printf("Magnitude: %.2f pixels, Angle: %.2f degrees\n", magnitude, angle);

        // Determine direction based on angle
        if (magnitude * (1 << Q15_SHIFT) < THRESHOLD) {
            printf("Motion too small\n");
        } else {
            if (angle >= 337.5f || angle < 22.5f) {
                printf("Direction: Right\n");
            } else if (angle >= 22.5f && angle < 67.5f) {
                printf("Direction: Up-right\n");
            } else if (angle >= 67.5f && angle < 112.5f) {
                printf("Direction: Up\n");
            } else if (angle >= 112.5f && angle < 157.5f) {
                printf("Direction: Up-left\n");
            } else if (angle >= 157.5f && angle < 202.5f) {
                printf("Direction: Left\n");
            } else if (angle >= 202.5f && angle < 247.5f) {
                printf("Direction: Down-left\n");
            } else if (angle >= 247.5f && angle < 292.5f) {
                printf("Direction: Down\n");
            } else {
                printf("Direction: Down-right\n");
            }
        }
    } else {
        printf("No valid optical flow results\n");
        printf("Direction: Unknown\n");
    }
    return 0;
}