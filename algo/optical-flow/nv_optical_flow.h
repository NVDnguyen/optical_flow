/*
 * optical_flow.h
 *
 *  Created on: May 18, 2025
 *      Author: nvd
 */
#ifndef NV_OPTICAL_FLOW_H_
#define NV_OPTICAL_FLOW_H_
#include <stdint.h>
#define WIDTH 160
#define HEIGHT 90
#define PYR_LEVELS 2
#define WINDOW_SIZE 5
#define NUM_ITER 5

void rgb_to_grayscale(unsigned char *rgb, unsigned char *gray, int width, int height, int channels);
void build_image_pyramid(unsigned char *src, unsigned char *dst, int src_width, int src_height);
void compute_gradient(unsigned char *pyr, int16_t *gradx, int16_t *grady, int width, int height);
int find_strong_feature(unsigned char *gray, int width, int height, int32_t *point);
int lucas_kanade_at_level(unsigned char *pyr1, unsigned char *pyr2, int16_t *gradx, int16_t *grady,
                          int32_t *p0, int32_t *p1, int width, int height);
int lucas_kanade_pyramid(unsigned char **pyr1, unsigned char **pyr2, int16_t **gradx, int16_t **grady,
                         int32_t *p0, int32_t *p1, int width, int height, int levels);

#endif /* NV_OPTICAL_FLOW_H_ */
