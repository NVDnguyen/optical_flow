/********************************************************************************//**
 * @file
 * @brief Optimized optical flow for direction detection
 * @license Silicon Laboratories Inc. MSLA (see original)
 ***********************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <inttypes.h>
#include "nv_optical_flow.h"
#include "nv_ov2640.h"

#define STBI_NO_STDIO

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

typedef struct {
    unsigned char *pyr[PYR_LEVELS];
    int16_t *gradx[PYR_LEVELS];
    int16_t *grady[PYR_LEVELS];
    int32_t feature_point[2];
    int valid_feature;
} FrameData;

/********************************************************************************//**
 * Static Functions
 ***********************************************************************************/
static unsigned char pyr_buffer[PYR_SIZE];
static unsigned char gray_buffer[MAX_BUFFER_SIZE];


static int process_single_frame(int frame, FrameData *frame_data) {
    uint16_t *img = NULL;
    printf("%d: Processing frame\n", frame);
    if (ov2640_capture_frame(&img, frame) == 1) {
        printf("Error: Cannot load frame %d to RAM.\n", frame);
        return ERROR;
    }
    rgb565_to_grayscale(img, gray_buffer, WIDTH, HEIGHT);
    printf("%d: Converted frame to grayscale\n", frame);

    // Build pyramid for current frame
    frame_data->pyr[0] = pyr_buffer;
    frame_data->pyr[1] = pyr_buffer + MAX_BUFFER_SIZE;
    memcpy(frame_data->pyr[0], gray_buffer, WIDTH * HEIGHT);

    for (int l = 1; l < PYR_LEVELS; l++) {
        int w = WIDTH >> l;
        int h = HEIGHT >> l;
        build_image_pyramid(frame_data->pyr[l - 1], frame_data->pyr[l], w * 2, h * 2);
        printf("%d: Built pyramid level %d (size: %dx%d)\n", frame, l, w, h);
    }

    static int16_t grad_buffer_local[PYR_SIZE * 2];
    for (int l = 0; l < PYR_LEVELS; l++) {
        int w = WIDTH >> l;
        int h = HEIGHT >> l;
        frame_data->gradx[l] = grad_buffer_local;
        frame_data->grady[l] = grad_buffer_local + (w * h / 2);
        compute_gradient(frame_data->pyr[l], frame_data->gradx[l], frame_data->grady[l], w, h);
        printf("%d: Computed gradients, level %d\n", frame, l);
    }

    // Find specific feature point
    if (frame == 1) {
        frame_data->valid_feature = find_strong_feature(gray_buffer, WIDTH, HEIGHT, frame_data->feature_point);
        if (!frame_data->valid_feature) {
            frame_data->feature_point[0] = (WIDTH / 2) << Q15_SHIFT; // x
            frame_data->feature_point[1] = (HEIGHT / 2) << Q15_SHIFT; // y
            printf("No strong feature found for frame %d, using center (%d,%d)\n",
                   frame, frame_data->feature_point[0] >> Q15_SHIFT, frame_data->feature_point[1] >> Q15_SHIFT);
        } else {
            printf("%d: Found feature for frame at (%d,%d)\n",
                   frame, frame_data->feature_point[0] >> Q15_SHIFT, frame_data->feature_point[1] >> Q15_SHIFT);
        }
    }

    return OK;
}

static int calculate_motion(FrameData *prev_frame, FrameData *curr_frame, int32_t p0[2], int32_t p1[2], int32_t *dy) {
    printf("Calculating motion from prev to curr frame\n");
    printf("Initial feature point: (%d,%d)\n", p0[0] >> Q15_SHIFT, p0[1] >> Q15_SHIFT);
    int valid = lucas_kanade_pyramid(prev_frame->pyr, curr_frame->pyr,
                                     prev_frame->gradx, prev_frame->grady,
                                     p0, p1, WIDTH, HEIGHT, PYR_LEVELS);
    if (!valid) {
        printf("Optical flow failed\n");
        *dy = 0;
        return ERROR;
    } else {
        *dy = p1[1] - p0[1];
        printf("Optical flow valid: (%d,%d)\n", p1[0] >> Q15_SHIFT, p1[1] >> Q15_SHIFT);
        printf("Computed dy = %d (approx %d pixels)\n", *dy, *dy >> Q15_SHIFT);
        return OK;
    }
}

/*********************************************************************************
 * Main function
 ***********************************************************************************/
int main(void) {
    ov2640_init(HEIGHT, WIDTH);
    printf("Hello from Windows\nStarting...\n");

    FrameData frame1_data, frame2_data;
    int32_t dy = 0;

    process_single_frame(1, &frame1_data);
    process_single_frame(2, &frame2_data);
    calculate_motion(&frame1_data, &frame2_data, frame1_data.feature_point, frame2_data.feature_point, &dy);

    const int16_t THRESHOLD = 205; // 0.0125 in Q15
    if (dy > THRESHOLD) {
        printf("=> Up\n");
    } else if (dy < -THRESHOLD) {
        printf("=> Down\n");
    } else {
        printf("=> Unknown\n");
    }
    printf("Final dy=%d\n", dy);

    return 0;
}