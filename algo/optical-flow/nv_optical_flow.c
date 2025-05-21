#include "nv_optical_flow.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
typedef struct {
    int x, y;
    int score;
} Candidate;
void rgb565_to_grayscale(uint16_t *rgb565, unsigned char *gray, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        uint16_t pixel = rgb565[i];
        unsigned char r = (pixel >> 11) & 0x1F;  // 5 bits red
        unsigned char g = (pixel >> 5) & 0x3F;   // 6 bits green
        unsigned char b = pixel & 0x1F;          // 5 bits blue

        // Convert to 8-bit values
        r = (r * 255) / 31;
        g = (g * 255) / 63;
        b = (b * 255) / 31;

        // Calculate grayscale value using weighted sum
        gray[i] = (r * 30 + g * 59 + b * 11) / 100;
    }

    // brightness adjustment
    for (int i = 0; i < width * height; i++) {
        gray[i] = (gray[i] < 128) ? (gray[i] * 3 / 4) : (gray[i] * 5 / 4 > 255 ? 255 : gray[i] * 5 / 4);
    }
}


void build_image_pyramid(unsigned char *src, unsigned char *dst, int src_width, int src_height) {
    int dst_width = src_width >> 1;
    int dst_height = src_height >> 1;
    for (int i = 0; i < dst_height; i++) {
        for (int j = 0; j < dst_width; j++) {
            int idx = (i * 2) * src_width + (j * 2);
            dst[i * dst_width + j] = (src[idx] + src[idx + 1] +
                                      src[idx + src_width] + src[idx + src_width + 1]) >> 2;
        }
    }
}

void compute_gradient(unsigned char *pyr, int16_t *gradx, int16_t *grady, int width, int height) {
    for (int i = 1; i < height - 1; i++) {
        for (int j = 1; j < width - 1; j++) {
            int idx = i * width + j;
            gradx[idx] = (-pyr[(i-1)*width + (j-1)] + pyr[(i-1)*width + (j+1)] +
                          -2*pyr[i*width + (j-1)] + 2*pyr[i*width + (j+1)] +
                          -pyr[(i+1)*width + (j-1)] + pyr[(i+1)*width + (j+1)]) >> 1;
            grady[idx] = (-pyr[(i-1)*width + (j-1)] - 2*pyr[(i-1)*width + j] - pyr[(i-1)*width + (j+1)] +
                          pyr[(i+1)*width + (j-1)] + 2*pyr[(i+1)*width + j] + pyr[(i+1)*width + (j+1)]) >> 1;
        }
    }
}

int find_strong_feature(unsigned char *gray, int width, int height, int32_t *point) {
    int cx = width / 2, cy = height / 2;
    int search_radius = 20;
    int max_grad = 0;
    int best_x = cx, best_y = cy;

    for (int y = cy - search_radius; y <= cy + search_radius; y++) {
        if (y < WINDOW_SIZE / 2 || y >= height - WINDOW_SIZE / 2) continue;
        for (int x = cx - search_radius; x <= cx + search_radius; x++) {
            if (x < WINDOW_SIZE / 2 || x >= width - WINDOW_SIZE / 2) continue;
            int Ix = (-gray[(y-1)*width + (x-1)] + gray[(y-1)*width + (x+1)] +
                      -2*gray[y*width + (x-1)] + 2*gray[y*width + (x+1)] +
                      -gray[(y+1)*width + (x-1)] + gray[(y+1)*width + (x+1)]) >> 1;
            int Iy = (-gray[(y-1)*width + (x-1)] - 2*gray[(y-1)*width + x] - gray[(y-1)*width + (x+1)] +
                      gray[(y+1)*width + (x-1)] + 2*gray[(y+1)*width + x] + gray[(y+1)*width + (x+1)]) >> 1;
            int grad = abs(Ix) + abs(Iy);
            if (grad > max_grad) {
                max_grad = grad;
                best_x = x;
                best_y = y;
            }
        }
    }

    if (max_grad < 30) {
        best_x = width / 2;
        best_y = height / 2;
    }
    point[0] = best_x << 14; // Q15
    point[1] = best_y << 14;
    return max_grad >= 30;
}
int find_multiple_features(unsigned char *gray, int width, int height, int32_t feature_points[MAX_FEATURES][2], int *num_features) {
    int search_radius = 50;
    int cx = width / 2, cy = height / 2;
    int feature_count = 0;
    Candidate candidates[10]; // Store potential features
    int candidate_count = 0;

    // Compute corner response (Shi-Tomasi)
    for (int y = cy - search_radius; y <= cy + search_radius; y++) {
        if (y < WINDOW_SIZE / 2 || y >= height - WINDOW_SIZE / 2) continue;
        for (int x = cx - search_radius; x <= cx + search_radius; x++) {
            if (x < WINDOW_SIZE / 2 || x >= width - WINDOW_SIZE / 2) continue;

            int Ix = (-gray[(y-1)*width + (x-1)] + gray[(y-1)*width + (x+1)] +
                      -2*gray[y*width + (x-1)] + 2*gray[y*width + (x+1)] +
                      -gray[(y+1)*width + (x-1)] + gray[(y+1)*width + (x+1)]) >> 1;
            int Iy = (-gray[(y-1)*width + (x-1)] - 2*gray[(y-1)*width + x] - gray[(y-1)*width + (x+1)] +
                      gray[(y+1)*width + (x-1)] + 2*gray[(y+1)*width + x] + gray[(y+1)*width + (x+1)]) >> 1;

            int Ixx = Ix * Ix;
            int Iyy = Iy * Iy;
            int Ixy = Ix * Iy;
            int score = (Ixx + Iyy) - sqrt((Ixx - Iyy) * (Ixx - Iyy) + 4 * Ixy * Ixy);

            if (score > 100) {
                candidates[candidate_count].x = x;
                candidates[candidate_count].y = y;
                candidates[candidate_count].score = score;
                candidate_count++;
            }
        }
    }

    // Sort candidates by score (descending)
    for (int i = 0; i < candidate_count - 1; i++) {
        for (int j = i + 1; j < candidate_count; j++) {
            if (candidates[j].score > candidates[i].score) {
                Candidate temp = candidates[i];
                candidates[i] = candidates[j];
                candidates[j] = temp;
            }
        }
    }

    // Select up to MAX_FEATURES, ensuring spatial separation
    for (int i = 0; i < candidate_count && feature_count < MAX_FEATURES; i++) {
        int x = candidates[i].x;
        int y = candidates[i].y;
        int valid = 1;

        for (int j = 0; j < feature_count; j++) {
            int dx = (feature_points[j][0] >> Q15_SHIFT) - x;
            int dy = (feature_points[j][1] >> Q15_SHIFT) - y;
            if (sqrt(dx * dx + dy * dy) < MIN_DISTANCE) {
                valid = 0;
                break;
            }
        }

        if (valid) {
            feature_points[feature_count][0] = x << Q15_SHIFT;
            feature_points[feature_count][1] = y << Q15_SHIFT;
            feature_count++;
        }
    }

    *num_features = feature_count;
    return feature_count > 0;
}
int lucas_kanade_at_level(unsigned char *pyr1, unsigned char *pyr2, int16_t *gradx, int16_t *grady,
                          int32_t *p0, int32_t *p1, int width, int height) {
    int32_t x = p0[0] >> 14, y = p0[1] >> 14;
    if (x < WINDOW_SIZE / 2 || x >= width - WINDOW_SIZE / 2 || y < WINDOW_SIZE / 2 || y >= height - WINDOW_SIZE / 2) {
        p1[0] = -1;
        p1[1] = -1;
        return 0;
    }

    int32_t u = 0, v = 0; // Q15
    int32_t det = 0;
    for (int iter = 0; iter < NUM_ITER; iter++) {
        int32_t sum_x = 0, sum_y = 0, sum_xx = 0, sum_xy = 0, sum_yy = 0;
        int32_t nx = x + (u >> 14), ny = y + (v >> 14);

        for (int dy = -WINDOW_SIZE / 2; dy <= WINDOW_SIZE / 2; dy++) {
            for (int dx = -WINDOW_SIZE / 2; dx <= WINDOW_SIZE / 2; dx++) {
                int px = x + dx, py = y + dy;
                int qx = nx + dx, qy = ny + dy;
                if (px < 0 || px >= width || py < 0 || py >= height || qx < 0 || qx >= width || qy < 0 || qy >= height) continue;

                int16_t Ix = gradx[py * width + px];
                int16_t Iy = grady[py * width + px];
                int16_t It = pyr2[qy * width + qx] - pyr1[py * width + px];

                sum_x += (int32_t)Ix * It;
                sum_y += (int32_t)Iy * It;
                sum_xx += (int32_t)Ix * Ix;
                sum_xy += (int32_t)Ix * Iy;
                sum_yy += (int32_t)Iy * Iy;
            }
        }

        det = sum_xx * sum_yy - sum_xy * sum_xy;
        if (abs(det) < 1000) {
            p1[0] = -1;
            p1[1] = -1;
            return 0;
        }

        int32_t du = ((sum_yy * (-sum_x) - sum_xy * (-sum_y)) << 14) / det;
        int32_t dv = ((sum_xx * (-sum_y) - sum_xy * (-sum_x)) << 14) / det;
        u += du;
        v += dv;

        if (abs(du) < (1 << 11) && abs(dv) < (1 << 11)) break;
    }

    p1[0] = (x << 14) + u;
    p1[1] = (y << 14) + v;
    return 1;
}

int lucas_kanade_pyramid(unsigned char **pyr1, unsigned char **pyr2, int16_t **gradx, int16_t **grady,
                         int32_t *p0, int32_t *p1, int width, int height, int levels) {
    int32_t point[2] = { p0[0], p0[1] };
    int32_t curr_p[2], next_p[2];

    for (int l = levels - 1; l >= 0; l--) {
        int w = width >> l;
        int h = height >> l;
        curr_p[0] = point[0] >> l;
        curr_p[1] = point[1] >> l;

        if (!lucas_kanade_at_level(pyr1[l], pyr2[l], gradx[l], grady[l], curr_p, next_p, w, h)) {
            p1[0] = -1;
            p1[1] = -1;
            return 0;
        }

        point[0] = next_p[0] << 1;
        point[1] = next_p[1] << 1;
    }

    p1[0] = point[0];
    p1[1] = point[1];
    return 1;
}
