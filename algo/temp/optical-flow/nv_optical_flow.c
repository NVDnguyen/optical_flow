#include "nv_optical_flow.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Define a named struct for candidates
typedef struct {
    float score; // Shi-Tomasi score (minimum eigenvalue)
    int x;
    int y;
} FeatureCandidate;
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
void rgb_to_grayscale(unsigned char *rgb, unsigned char *gray, int width, int height, int channels) {
    if (channels == 1) {
        memcpy(gray, rgb, width * height);
    } else if (channels == 3) {  
        for (int i = 0; i < width * height; i++) {
            unsigned char r = rgb[i * 3];
            unsigned char g = rgb[i * 3 + 1];
            unsigned char b = rgb[i * 3 + 2];
            gray[i] = (r * 30 + g * 59 + b * 11) / 100;
        }
    }

    for (int i = 0; i < width * height; i++) {
        gray[i] = (gray[i] < 128) ? gray[i] * 3 / 4 : (gray[i] * 5 / 4 > 255 ? 255 : gray[i] * 5 / 4);
    }
}

void gaussian_blur(unsigned char *src, unsigned char *dst, int width, int height) {
    // 3x3 Gaussian kernel: [1, 2, 1; 2, 4, 2; 1, 2, 1] / 16
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int idx = y * width + x;
            int sum = src[(y-1)*width + (x-1)] + 2*src[(y-1)*width + x] + src[(y-1)*width + (x+1)] +
                      2*src[y*width + (x-1)] + 4*src[y*width + x] + 2*src[y*width + (x+1)] +
                      src[(y+1)*width + (x-1)] + 2*src[(y+1)*width + x] + src[(y+1)*width + (x+1)];
            dst[idx] = sum >> 4; // Divide by 16
        }
    }
    // Copy border pixels (no blur applied)
    for (int x = 0; x < width; x++) {
        dst[x] = src[x]; // Top row
        dst[(height-1)*width + x] = src[(height-1)*width + x]; // Bottom row
    }
    for (int y = 0; y < height; y++) {
        dst[y*width] = src[y*width]; // Left column
        dst[y*width + (width-1)] = src[y*width + (width-1)]; // Right column
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

int find_strong_features(unsigned char *gray, int width, int height, int32_t *points, int max_features) {
    int num_features = 0;

    // Array to store Shi-Tomasi scores and positions
    FeatureCandidate candidates[WIDTH * HEIGHT];
    int num_candidates = 0;

    // Temporary buffers for gradients
    int16_t *gradx = (int16_t *)malloc(width * height * sizeof(int16_t));
    int16_t *grady = (int16_t *)malloc(width * height * sizeof(int16_t));
    if (!gradx || !grady) {
        free(gradx);
        free(grady);
        return 0;
    }

    // Compute gradients
    compute_gradient(gray, gradx, grady, width, height);

    // Compute Shi-Tomasi scores (minimum eigenvalue of the second-moment matrix)
    for (int y = WINDOW_SIZE / 2; y < height - WINDOW_SIZE / 2; y++) {
        for (int x = WINDOW_SIZE / 2; x < width - WINDOW_SIZE / 2; x++) {
            // Compute second-moment matrix over a 3x3 window
            float sum_xx = 0, sum_xy = 0, sum_yy = 0;
            for (int dy = -1; dy <= 1; dy++) {
                for (int dx = -1; dx <= 1; dx++) {
                    int px = x + dx, py = y + dy;
                    if (px < 0 || px >= width || py < 0 || py >= height) continue;
                    int16_t Ix = gradx[py * width + px];
                    int16_t Iy = grady[py * width + px];
                    sum_xx += (float)Ix * Ix;
                    sum_xy += (float)Ix * Iy;
                    sum_yy += (float)Iy * Iy;
                }
            }

            // Compute eigenvalues of the 2x2 matrix [sum_xx, sum_xy; sum_xy, sum_yy]
            float trace = sum_xx + sum_yy;
            float det = sum_xx * sum_yy - sum_xy * sum_xy;
            float disc = trace * trace - 4.0f * det;
            if (disc < 0) disc = 0; // Avoid complex roots
            float score = (trace - sqrtf(disc)) / 2.0f; // Shi-Tomasi: minimum eigenvalue

            if (score > 1000.0f && num_candidates < WIDTH * HEIGHT) { // Threshold for strong corners
                candidates[num_candidates].score = score;
                candidates[num_candidates].x = x;
                candidates[num_candidates].y = y;
                num_candidates++;
            }
        }
    }

    free(gradx);
    free(grady);

    // Sort candidates by score (bubble sort)
    for (int i = 0; i < num_candidates - 1; i++) {
        for (int j = 0; j < num_candidates - i - 1; j++) {
            if (candidates[j].score < candidates[j + 1].score) {
                FeatureCandidate temp;
                temp.score = candidates[j].score;
                temp.x = candidates[j].x;
                temp.y = candidates[j].y;
                candidates[j].score = candidates[j + 1].score;
                candidates[j].x = candidates[j + 1].x;
                candidates[j].y = candidates[j + 1].y;
                candidates[j + 1].score = temp.score;
                candidates[j + 1].x = temp.x;
                candidates[j + 1].y = temp.y;
            }
        }
    }

    // Select top max_features, ensuring they are not too close
    for (int i = 0; i < num_candidates && num_features < max_features; i++) {
        int x = candidates[i].x;
        int y = candidates[i].y;
        int too_close = 0;
        for (int j = 0; j < num_features; j++) {
            int dx = (points[j * 2] >> 14) - x;
            int dy = (points[j * 2 + 1] >> 14) - y;
            if (dx * dx + dy * dy < 225) { // Minimum distance of 15 pixels
                too_close = 1;
                break;
            }
        }
        if (!too_close) {
            points[num_features * 2] = x << 14; // Q15
            points[num_features * 2 + 1] = y << 14;
            num_features++;
        }
    }

    return num_features; // Return only the number of valid features
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

        // Early check for out-of-bounds in the destination point
        if (nx < WINDOW_SIZE / 2 || nx >= width - WINDOW_SIZE / 2 || ny < WINDOW_SIZE / 2 || ny >= height - WINDOW_SIZE / 2) {
            p1[0] = -1;
            p1[1] = -1;
            return 0;
        }

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

    // Final check for out-of-bounds
    int32_t final_x = p1[0] >> 14;
    int32_t final_y = p1[1] >> 14;
    if (final_x < 0 || final_x >= width || final_y < 0 || final_y >= height) {
        p1[0] = -1;
        p1[1] = -1;
        return 0;
    }

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