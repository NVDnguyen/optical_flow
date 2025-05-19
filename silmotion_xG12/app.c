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

extern const unsigned char frame1_jpg[];
extern const unsigned int frame1_jpg_len;
extern const unsigned char frame2_jpg[];
extern const unsigned int frame2_jpg_len;

#define WIDTH 160
#define HEIGHT 90
#define PYR_LEVELS 2
#define WINDOW_SIZE 5
#define NUM_ITER 5
#define MAX_BUFFER_SIZE (WIDTH * HEIGHT)
#define PYR_SIZE (MAX_BUFFER_SIZE + (MAX_BUFFER_SIZE >> 2))

typedef enum {
  OK,
  ERROR,
  INVALID_SIZE,
  LOAD_FAIL
} status_t;

static unsigned char pyr_buffer[PYR_SIZE];
static int16_t grad_buffer[MAX_BUFFER_SIZE];
static unsigned char gray_buffer[MAX_BUFFER_SIZE];

static void send_string(const char *msg) {
  for (int i = 0; msg[i] != '\0'; i++) {
    usart_send(msg[i]);
  }
}

static void rx_callback(uint8_t data) {
  send_string("OK\n");
}

static void cleanup_image_resources(unsigned char *img) {
  if (img) stbi_image_free(img);
}

void app_init(void) {
  usart_init();
  usart_set_rx_callback(rx_callback);
  send_string("Hello from xG24\nStarting...\n");

  int16_t p0[2], p1[2];
  unsigned char *img = NULL;
  unsigned char *gray = gray_buffer;
  unsigned char *pyr[PYR_LEVELS];
  int16_t *gradx[PYR_LEVELS];
  int16_t *grady[PYR_LEVELS];
  int16_t dy = 0;
  int valid_frame1 = 0;

  for (int frame = 1; frame <= 2; frame++) {
    const unsigned char *frame_data = (frame == 1) ? frame1_jpg : frame2_jpg;
    unsigned int frame_len = (frame == 1) ? frame1_jpg_len : frame2_jpg_len;
    char debug[48];
    snprintf(debug, sizeof(debug), "Frame%d: %u bytes\n", frame, frame_len);
    send_string(debug);

    // Load image
    int width, height, channels;
    img = stbi_load_from_memory(frame_data, frame_len, &width, &height, &channels, 0);
    if (!img || width != WIDTH || height != HEIGHT) {
        snprintf(debug, sizeof(debug), "Frame%d load failed\n", frame);
        send_string(debug);
        cleanup_image_resources(img);
        continue;
    }
    snprintf(debug, sizeof(debug), "Frame%d channels=%d\n", frame, channels);
    send_string(debug);

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
        build_image_pyramid(pyr[l-1], pyr[l], w * 2, h * 2);
    }

    // Process frame 1: Compute gradients and find feature
    if (frame == 1) {
        for (int l = 0; l < PYR_LEVELS; l++) {
            int w = WIDTH >> l;
            int h = HEIGHT >> l;
            gradx[l] = grad_buffer;
            grady[l] = grad_buffer + (w * h / 2);
            compute_gradient(pyr[l], gradx[l], grady[l], w, h);
            int idx = (h/2) * w + (w/2);
            snprintf(debug, sizeof(debug), "Level%d: Gradx=%d, Grady=%d\n", l, gradx[l][idx], grady[l][idx]);
            send_string(debug);
        }

        valid_frame1 = find_strong_feature(gray, WIDTH, HEIGHT, p0);
        snprintf(debug, sizeof(debug), "Feature at (%d,%d), max_grad=%d\n", p0[0] >> 14, p0[1] >> 14, valid_frame1 ? 30 : 0);
        send_string(debug);
        if (!valid_frame1) {
            send_string("No valid feature, using center\n");
            p0[0] = (WIDTH / 2) << 14;
            p0[1] = (HEIGHT / 2) << 14;
        }
    } else if (valid_frame1 || (p0[0] != 0 && p0[1] != 0)) {
        // Compute optical flow
        unsigned char *pyr2[PYR_LEVELS] = { pyr_buffer, pyr_buffer + MAX_BUFFER_SIZE };
        memcpy(pyr2[0], gray, WIDTH * HEIGHT);
        for (int l = 1; l < PYR_LEVELS; l++) {
            int w = WIDTH >> l;
            int h = HEIGHT >> l;
            build_image_pyramid(pyr2[l-1], pyr2[l], w * 2, h * 2);
        }

        int valid = lucas_kanade_pyramid(pyr, pyr2, gradx, grady, p0, p1, WIDTH, HEIGHT, PYR_LEVELS);
        if (!valid) {
            send_string("Optical flow failed\n");
            dy = 0;
        } else {
            dy = p1[1] - p0[1];
            snprintf(debug, sizeof(debug), "p0=(%d,%d), p1=(%d,%d), dy=%d\n",
                     p0[0] >> 14, p0[1] >> 14, p1[0] >> 14, p1[1] >> 14, dy);
            send_string(debug);
        }
    }
  }

  // Report results
  const int16_t THRESHOLD = 205; // 0.0125 in Q15
  if (dy < -THRESHOLD) {
      send_string("Up\n");
  } else if (dy > THRESHOLD) {
      send_string("Down\n");
  } else {
      send_string("Unknown\n");
      char debug[32];
      snprintf(debug, sizeof(debug), "Final dy=%d\n", dy);
      send_string(debug);
  }
}

void app_process_action(void) {
}
