#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include "nv_optical_flow.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "frame1.h"
#include "frame2.h"

#define MAX_BUFFER_SIZE (WIDTH * HEIGHT)
#define PYR_SIZE (MAX_BUFFER_SIZE + (MAX_BUFFER_SIZE >> 2))
#define Q15_SHIFT 14
#define GRAD_SCALE_FACTOR (1 << Q15_SHIFT)

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

static void cleanup_image_resources(unsigned char *img) {
  if (img) stbi_image_free(img);
}
/********************************************************************************//**
 * Initialize application.
 ***********************************************************************************/
int main() {

    printf("Hello from xG24\nStarting...\n");

    int32_t p0[2] = {0, 0}, p1[2] = {0, 0};
    unsigned char *img = NULL;
    unsigned char *gray = gray_buffer;
    unsigned char *pyr[PYR_LEVELS];
    int16_t *gradx[PYR_LEVELS];
    int16_t *grady[PYR_LEVELS];
    int32_t dy = 0;
    int valid_frame1 = 0;

    for (int frame = 1; frame <= 2; frame++) {
        const unsigned char *frame_data = (frame == 1) ? frame1_jpg : frame2_jpg;
        unsigned int frame_len = (frame == 1) ? frame1_jpg_len : frame2_jpg_len;
        printf("Frame%d: %u bytes\n", frame, frame_len);

        // Load image
        int width, height, channels;
        img = stbi_load_from_memory(frame_data, frame_len, &width, &height, &channels, 0);
        if (!img || width != WIDTH || height != HEIGHT) {
            printf("Error: Image size does not match\n");
            cleanup_image_resources(img);
            continue;
        }

        // Check if image is RGB
        if (channels != 3) {
            printf("Error: Image is not RGB\n");
            cleanup_image_resources(img);
            continue;
        }

        // Convert to grayscale
        rgb_to_grayscale(img, gray, WIDTH, HEIGHT, channels);
        cleanup_image_resources(img);
        img = NULL;

        // Build image pyramid
        pyr[0] = pyr_buffer;
        pyr[1] = pyr_buffer + MAX_BUFFER_SIZE;
        memcpy(pyr[0], gray, WIDTH * HEIGHT);
        for (int l = 1; l < PYR_LEVELS; l++) {
            int w = WIDTH >> l;
            int h = HEIGHT >> l;
            build_image_pyramid(pyr[l - 1], pyr[l], w * 2, h * 2);
        }

        // Process frame 1: Compute gradients and find feature
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

            valid_frame1 = find_strong_feature(gray, WIDTH, HEIGHT, p0);
            printf("Feature point: (%" PRId32 ",%" PRId32 ")\n", p0[0] >> Q15_SHIFT, p0[1] >> Q15_SHIFT);
            if (valid_frame1) {
                printf("Valid feature point found\n");
            } else {
                printf("No valid feature point found\n");
            }
            if (!valid_frame1) {
                printf("No strong feature near center\n");
                p0[0] = (WIDTH / 2) << Q15_SHIFT;
                p0[1] = (HEIGHT / 2) << Q15_SHIFT;
            }
        } else if (valid_frame1 || (p0[0] != 0 && p0[1] != 0)) {
            // Compute optical flow
            unsigned char *pyr2[PYR_LEVELS] = { pyr_buffer, pyr_buffer + MAX_BUFFER_SIZE };
            memcpy(pyr2[0], gray, WIDTH * HEIGHT);
            for (int l = 1; l < PYR_LEVELS; l++) {
                int w = WIDTH >> l;
                int h = HEIGHT >> l;
                build_image_pyramid(pyr2[l - 1], pyr2[l], w * 2, h * 2);
            }

            int valid = lucas_kanade_pyramid(pyr, pyr2, gradx, grady, p0, p1, WIDTH, HEIGHT, PYR_LEVELS);
            if (!valid) {
                printf("Optical flow failed\n");
                dy = 0;
            } else {
                dy = p1[1] - p0[1];
                printf("Optical flow valid: (%" PRId32 ",%" PRId32 ")\n", p1[0] >> Q15_SHIFT, p1[1] >> Q15_SHIFT);
            }
        }
    }

    // Report results
    const int16_t THRESHOLD = 205; // 0.0125 in Q15
    if (dy < -THRESHOLD) {
        printf("Up\n");
    } else if (dy > THRESHOLD) {
        printf("Down\n");
    } else {
        printf("Unknown\n");
        printf("Final dy=%" PRId32 "\n", dy);
        
    }
    return 0;
}
