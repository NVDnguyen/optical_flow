/********************************************************************************//**
 * @file
 * @brief Optimized optical flow for direction detection
 * @license Silicon Laboratories Inc. MSLA (see original)
 ***********************************************************************************/
#include <stdlib.h>
#include <nv_usart.h>
#include <stdio.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_eusart.h"
#include "em_gpio.h"
#include "sl_sleeptimer.h"
#include "nv_optical_flow.h"
#define STBI_NO_STDIO
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <inttypes.h>

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
static unsigned char gray_buffer[MAX_BUFFER_SIZE];

static void rx_callback(uint8_t data) {
  usart_printf("OK\n");
}

static void cleanup_image_resources(unsigned char *img) {
  if (img) stbi_image_free(img);
}
static int process_frame(int frame, int32_t p0[2], int32_t p1[2], int *valid_frame1, int32_t *dy) {
    const unsigned char *frame_data = (frame == 1) ? frame1_jpg : frame2_jpg;
    unsigned int frame_len = (frame == 1) ? frame1_jpg_len : frame2_jpg_len;
    usart_printf("Frame%d: %u bytes\n", frame, frame_len);

    int width, height, channels;
    unsigned char *img = stbi_load_from_memory(frame_data, frame_len, &width, &height, &channels, 0);
    if (!img || width != WIDTH || height != HEIGHT) {
        usart_printf("Error: Image size does not match\n");
        cleanup_image_resources(img);
        return ERROR;
    }

    if (channels != 3) {
        usart_printf("Error: Image is not RGB\n");
        cleanup_image_resources(img);
        return ERROR;
    }

    rgb_to_grayscale(img, gray_buffer, WIDTH, HEIGHT, channels);
    cleanup_image_resources(img);

    // Build pyramid for current frame
    unsigned char *pyr[PYR_LEVELS];
    pyr[0] = pyr_buffer;
    pyr[1] = pyr_buffer + MAX_BUFFER_SIZE;
    memcpy(pyr[0], gray_buffer, WIDTH * HEIGHT);

    for (int l = 1; l < PYR_LEVELS; l++) {
        int w = WIDTH >> l;
        int h = HEIGHT >> l;
        build_image_pyramid(pyr[l - 1], pyr[l], w * 2, h * 2);
    }

    static int16_t grad_buffer_local[MAX_BUFFER_SIZE];
    int16_t *gradx[PYR_LEVELS];
    int16_t *grady[PYR_LEVELS];

    if (frame == 1) {
        for (int l = 0; l < PYR_LEVELS; l++) {
            int w = WIDTH >> l;
            int h = HEIGHT >> l;
            gradx[l] = grad_buffer_local;
            grady[l] = grad_buffer_local + (w * h / 2);
            compute_gradient(pyr[l], gradx[l], grady[l], w, h);
            int idx = (h / 2) * w + (w / 2);
            usart_printf("Gradient at (%d,%d): (%d,%d)\n", w / 2, h / 2, gradx[l][idx], grady[l][idx]);
        }

        *valid_frame1 = find_strong_feature(gray_buffer, WIDTH, HEIGHT, p0);
        usart_printf("Feature point: (%" PRId32 ",%" PRId32 ")\n", p0[0] >> Q15_SHIFT, p0[1] >> Q15_SHIFT);
        if (*valid_frame1) {
            usart_printf("Valid feature point found\n");
        } else {
            usart_printf("No valid feature point found\n");
            usart_printf("No strong feature near center\n");
            p0[0] = (WIDTH / 2) << Q15_SHIFT;
            p0[1] = (HEIGHT / 2) << Q15_SHIFT;
        }
    } else if (*valid_frame1 || (p0[0] != 0 && p0[1] != 0)) {
        unsigned char *pyr2[PYR_LEVELS] = { pyr_buffer, pyr_buffer + MAX_BUFFER_SIZE };
        memcpy(pyr2[0], gray_buffer, WIDTH * HEIGHT);
        for (int l = 1; l < PYR_LEVELS; l++) {
            int w = WIDTH >> l;
            int h = HEIGHT >> l;
            build_image_pyramid(pyr2[l - 1], pyr2[l], w * 2, h * 2);
        }

        int valid = lucas_kanade_pyramid(pyr, pyr2, gradx, grady, p0, p1, WIDTH, HEIGHT, PYR_LEVELS);
        if (!valid) {
            usart_printf("Optical flow failed\n");
            *dy = 0;
        } else {
            *dy = p1[1] - p0[1];
            usart_printf("Optical flow valid: (%" PRId32 ",%" PRId32 ")\n", p1[0] >> Q15_SHIFT, p1[1] >> Q15_SHIFT);
        }
    }
    return OK;
}

/*********************************************************************************
 * Initialize application.
 ***********************************************************************************/
void app_init(void) {
    usart_init();
    usart_set_rx_callback(rx_callback);
    usart_printf("Hello from xG24\nStarting...\n");

    int32_t p0[2] = {0, 0}, p1[2] = {0, 0};
    int valid_frame1 = 0;
    int32_t dy = 0;

    for (int frame = 1; frame <= 2; frame++) {
        process_frame(frame, p0, p1, &valid_frame1, &dy);
    }
    // Report results
    const int16_t THRESHOLD = 205; // 0.0125 in Q15
    if (dy < -THRESHOLD) {
        usart_printf("Up\n");
    } else if (dy > THRESHOLD) {
        usart_printf("Down\n");
    } else {
        usart_printf("Unknown\n");
        usart_printf("Final dy=%ld\n", dy);
    }
}
/********************************************************************************//**
 * App ticking function.
 ***********************************************************************************/
void app_process_action(void) {
}
