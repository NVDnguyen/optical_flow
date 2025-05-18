/*
 * optical_flow.h
 *
 *  Created on: May 18, 2025
 *      Author: nvd
 */

#ifndef NV_OPTICAL_FLOW_H_
#define NV_OPTICAL_FLOW_H_

#define WIDTH 320
#define HEIGHT 180
#define PYR_LEVELS 3
#define WINDOW_SIZE 5
#define NUM_ITER 5

void rgb_to_grayscale(unsigned char *rgb, unsigned char *gray, int width, int height);

void build_image_pyramid(unsigned char *gray, unsigned char **pyramid, int width, int height, int levels);

void compute_gradient(unsigned char *pyr, int *gradx, int *grady, int width, int height);

int find_strong_feature(unsigned char *gray, int width, int height, float *point);

int lucas_kanade_at_level(unsigned char *pyr1, unsigned char *pyr2, int *gradx, int *grady,
                          float *p0, float *p1, int width, int height);

int lucas_kanade_pyramid(unsigned char **pyr1, unsigned char **pyr2, int **gradx, int **grady,
                         float *p0, float *p1, int width, int height, int levels);


#endif /* NV_OPTICAL_FLOW_H_ */
