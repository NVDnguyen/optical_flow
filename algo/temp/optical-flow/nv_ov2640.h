/*
 * nv_ov2640.h
 *
 *  Created on: May 21, 2025
 *      Author: nvd
 */

#ifndef NV_OV2640_H_
#define NV_OV2640_H_
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "frame1_rgb565.h"
#include "frame2_rgb565.h"

uint16_t *frame_buffer = NULL;

uint8_t ov2640_init(uint16_t height,uint16_t width){
  frame_buffer = (uint16_t *)malloc(height * width * sizeof(uint16_t));
  if (frame_buffer == NULL) {
      return 1;
  }
  return 0;
}
uint8_t ov2640_capture_frame(uint16_t **frame, uint8_t num) {
    if(frame_buffer != NULL){
        const uint16_t *source = (num == 1) ? frame1_rgb565 : frame2_rgb565;
        memcpy(frame_buffer, source, sizeof(frame1_rgb565));
        *frame = frame_buffer;
        return 0;
    }
    return 1;
}
void ov2640_deinit() {
    if (frame_buffer != NULL) {
        free(frame_buffer);
        frame_buffer = NULL;
    }
}



#endif /* NV_OV2640_H_ */
