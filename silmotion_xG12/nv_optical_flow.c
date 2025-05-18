/*
 * optical_flow.c
 *
 *  Created on: May 18, 2025
 *      Author: nvd
 */

#ifndef GNU_ARM_V12_2_1___DEBUG_OPTICAL_FLOW_C_
#define GNU_ARM_V12_2_1___DEBUG_OPTICAL_FLOW_C_

#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <nv_optical_flow.h>

void rgb_to_grayscale(unsigned char *rgb, unsigned char *gray, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        unsigned char r = rgb[i * 3];
        unsigned char g = rgb[i * 3 + 1];
        unsigned char b = rgb[i * 3 + 2];
        gray[i] = (unsigned char)((r * 30 + g * 59 + b * 11) / 100);
    }
}

void build_image_pyramid(unsigned char *gray, unsigned char **pyramid, int width, int height, int levels) {
    pyramid[0] = gray;
    int kernel[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}};
    int pcw = width;

    for (int l = 1; l < levels; l++) {
        int cw = width >> l;
        int ch = height >> l;
        pyramid[l] = (unsigned char *)malloc(cw * ch);
        if (!pyramid[l]) return;

        for (int i = 0; i < ch; i++) {
            for (int j = 0; j < cw; j++) {
                int sum = 0;
                for (int u = -1; u <= 1; u++) {
                    for (int v = -1; v <= 1; v++) {
                        int nu = u, nv = v;
                        if (i == 0) nu++;
                        if (j == 0) nv++;
                        sum += kernel[u + 1][v + 1] * (int)pyramid[l-1][(i*2 + nu)*pcw + (j*2 + nv)];
                    }
                }
                pyramid[l][i*cw + j] = (unsigned char)(sum / 16);
            }
        }
        pcw = cw;
    }
}

void compute_gradient(unsigned char *pyr, int *gradx, int *grady, int width, int height) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (i == 0 || i == height - 1 || j == 0 || j == width - 1) {
                gradx[i*width + j] = 0;
                grady[i*width + j] = 0;
            } else {
                gradx[i*width + j] = -pyr[(i-1)*width + (j-1)] + pyr[(i-1)*width + (j+1)]
                                   + -2*pyr[i*width + (j-1)] + 2*pyr[i*width + (j+1)]
                                   + -pyr[(i+1)*width + (j-1)] + pyr[(i+1)*width + (j+1)];
                grady[i*width + j] = -pyr[(i-1)*width + (j-1)] - 2*pyr[(i-1)*width + j] - pyr[(i-1)*width + (j+1)]
                                   + pyr[(i+1)*width + (j-1)] + 2*pyr[(i+1)*width + j] + pyr[(i+1)*width + (j+1)];
            }
        }
    }
}

int find_strong_feature(unsigned char *gray, int width, int height, float *point) {
    int cx = WIDTH / 2;
    int cy = HEIGHT / 2;
    int search_radius = 10;
    int max_grad = 0;
    int best_x = cx, best_y = cy;

    for (int y = cy - search_radius; y <= cy + search_radius && y < height - 2; y++) {
        if (y < 2) continue;
        for (int x = cx - search_radius; x <= cx + search_radius && x < width - 2; x++) {
            if (x < 2) continue;
            int Ix = gray[y*width + (x+1)] - gray[y*width + (x-1)];
            int Iy = gray[(y+1)*width + x] - gray[(y-1)*width + x];
            int grad = abs(Ix) + abs(Iy);
            if (grad > max_grad) {
                max_grad = grad;
                best_x = x;
                best_y = y;
            }
        }
    }

    if (max_grad < 20) return 0;
    point[0] = (float)best_x;
    point[1] = (float)best_y;
    return 1;
}

int lucas_kanade_at_level(unsigned char *pyr1, unsigned char *pyr2, int *gradx, int *grady,
                          float *p0, float *p1, int width, int height) {
    float x = p0[0], y = p0[1];
    if (x < WINDOW_SIZE/2 || x >= width - WINDOW_SIZE/2 || y < WINDOW_SIZE/2 || y >= height - WINDOW_SIZE/2) {
        p1[0] = -1;
        p1[1] = -1;
        return 0;
    }

    float u = 0, v = 0;
    for (int iter = 0; iter < NUM_ITER; iter++) {
        float sum_x = 0, sum_y = 0, sum_xx = 0, sum_xy = 0, sum_yy = 0;
        float nx = x + u, ny = y + v;

        for (int dy = -WINDOW_SIZE/2; dy <= WINDOW_SIZE/2; dy++) {
            for (int dx = -WINDOW_SIZE/2; dx <= WINDOW_SIZE/2; dx++) {
                int px = (int)(x + dx), py = (int)(y + dy);
                int qx = (int)(nx + dx), qy = (int)(ny + dy);
                if (qx < 0 || qx >= width || qy < 0 || qy >= height) continue;

                float Ix = gradx[py*width + px];
                float Iy = grady[py*width + px];
                float It = (float)(pyr2[py*width + px] - pyr1[qy*width + qx]);

                sum_x += Ix * It;
                sum_y += Iy * It;
                sum_xx += Ix * Ix;
                sum_xy += Ix * Iy;
                sum_yy += Iy * Iy;
            }
        }

        float det = sum_xx * sum_yy - sum_xy * sum_xy;
        if (fabs(det) < 1e-6) {
            p1[0] = -1;
            p1[1] = -1;
            return 0;
        }

        float du = (sum_yy * (-sum_x) - sum_xy * (-sum_y)) / det;
        float dv = (sum_xx * (-sum_y) - sum_xy * (-sum_x)) / det;
        u += du;
        v += dv;

        if (fabs(du) < 1e-3 && fabs(dv) < 1e-3) break;
    }

    p1[0] = x + u;
    p1[1] = y + v;
    return 1;
}

int lucas_kanade_pyramid(unsigned char **pyr1, unsigned char **pyr2, int **gradx, int **grady,
                         float *p0, float *p1, int width, int height, int levels) {
    float point[2] = { p0[0], p0[1] };
    float curr_p[2], next_p[2];

    for (int l = levels - 1; l >= 0; l--) {
        int w = width >> l;
        int h = height >> l;

        curr_p[0] = point[0] / (1 << l);
        curr_p[1] = point[1] / (1 << l);

        if (!lucas_kanade_at_level(pyr1[l], pyr2[l], gradx[l], grady[l], curr_p, next_p, w, h)) {
            p1[0] = -1;
            p1[1] = -1;
            return 0;
        }

        point[0] = next_p[0] * (1 << l);
        point[1] = next_p[1] * (1 << l);
    }

    p1[0] = point[0];
    p1[1] = point[1];
    return 1;
}

#endif /* GNU_ARM_V12_2_1___DEBUG_OPTICAL_FLOW_C_ */
