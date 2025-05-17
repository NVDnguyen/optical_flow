#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define WIDTH 320
#define HEIGHT 180
#define PYR_LEVELS 3 // Số mức kim tự tháp
#define WINDOW_SIZE 5 // Kích thước cửa sổ gradient (5x5)
#define NUM_ITER 5 // Số lần lặp tinh chỉnh mỗi mức

// Chuyển đổi RGB sang grayscale
void rgb_to_grayscale(unsigned char *rgb, unsigned char *gray, int width, int height) {
    for (int i = 0; i < width * height; i++) {
        unsigned char r = rgb[i * 3];
        unsigned char g = rgb[i * 3 + 1];
        unsigned char b = rgb[i * 3 + 2];
        gray[i] = (unsigned char)((r * 30 + g * 59 + b * 11) / 100);
    }
}

// Tạo kim tự tháp ảnh với bộ lọc Gaussian 
void build_image_pyramid(unsigned char *gray, unsigned char **pyramid, int width, int height, int levels) {
    pyramid[0] = gray; // Mức 0 là ảnh gốc
    int kernel[3][3] = {{1, 2, 1}, {2, 4, 2}, {1, 2, 1}}; // Kernel Gaussian
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

// Tính gradient cho một mức kim tự tháp 
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

// Tìm điểm có gradient mạnh nhất trong vùng lân cận trung tâm
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

    if (max_grad < 20) return 0; // Gradient quá yếu
    point[0] = (float)best_x;
    point[1] = (float)best_y;
    return 1;
}

// Lucas-Kanade cải tiến tại một mức kim tự tháp
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

        // Tính gradient và chênh lệch trong cửa sổ 5x5
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

        // Giải hệ phương trình
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

        if (fabs(du) < 1e-3 && fabs(dv) < 1e-3) break; // Hội tụ
    }

    p1[0] = x + u;
    p1[1] = y + v;
    return 1;
}

// Lucas-Kanade với kim tự tháp
int lucas_kanade_pyramid(unsigned char **pyr1, unsigned char **pyr2, int **gradx, int **grady,
                         float *p0, float *p1, int width, int height, int levels) {
    float point[2] = { p0[0], p0[1] };
    float curr_p[2], next_p[2];

    for (int l = levels - 1; l >= 0; l--) {
        int w = width >> l;
        int h = height >> l;

        // Chia tỷ lệ điểm
        curr_p[0] = point[0] / (1 << l);
        curr_p[1] = point[1] / (1 << l);

        // Tính luồng quang học
        if (!lucas_kanade_at_level(pyr1[l], pyr2[l], gradx[l], grady[l], curr_p, next_p, w, h)) {
            p1[0] = -1;
            p1[1] = -1;
            return 0;
        }

        // Cập nhật điểm
        point[0] = next_p[0] * (1 << l);
        point[1] = next_p[1] * (1 << l);
    }

    p1[0] = point[0];
    p1[1] = point[1];
    return 1;
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        printf("Usage: %s <image1_path> <image2_path>\n", argv[0]);
        return -1;
    }
    int width, height, channels;
    unsigned char *img1 = stbi_load(argv[1], &width, &height, &channels, 3);
    unsigned char *img2 = stbi_load(argv[2], &width, &height, &channels, 3);

    // Kiểm tra lỗi đọc ảnh
    if (!img1 || !img2) {
        printf("Error: Cannot load images\n");
        if (img1) stbi_image_free(img1);
        if (img2) stbi_image_free(img2);
        return -1;
    }

    // Kiểm tra kích thước ảnh
    if (width != WIDTH || height != HEIGHT || channels != 3) {
        printf("Error: Image size does not match (%dx%d) or not RGB\n", WIDTH, HEIGHT);
        stbi_image_free(img1);
        stbi_image_free(img2);
        return -1;
    }

    // Chuyển sang ảnh xám
    unsigned char *gray1 = (unsigned char *)malloc(width * height);
    unsigned char *gray2 = (unsigned char *)malloc(width * height);
    if (!gray1 || !gray2) {
        printf("Error: Memory allocation failed\n");
        if (gray1) free(gray1);
        if (gray2) free(gray2);
        stbi_image_free(img1);
        stbi_image_free(img2);
        return -1;
    }
    rgb_to_grayscale(img1, gray1, width, height);
    rgb_to_grayscale(img2, gray2, width, height);

    // Tạo kim tự tháp ảnh
    unsigned char *pyr1[PYR_LEVELS];
    unsigned char *pyr2[PYR_LEVELS];
    int *gradx[PYR_LEVELS];
    int *grady[PYR_LEVELS];
    for (int i = 1; i < PYR_LEVELS; i++) {
        pyr1[i] = NULL;
        pyr2[i] = NULL;
        gradx[i] = NULL;
        grady[i] = NULL;
    }
    build_image_pyramid(gray1, pyr1, width, height, PYR_LEVELS);
    build_image_pyramid(gray2, pyr2, width, height, PYR_LEVELS);

    // Tính gradient cho mỗi mức
    for (int l = 0; l < PYR_LEVELS; l++) {
        int w = width >> l;
        int h = height >> l;
        gradx[l] = (int *)malloc(w * h * sizeof(int));
        grady[l] = (int *)malloc(w * h * sizeof(int));
        if (!gradx[l] || !grady[l]) {
            printf("Error: Memory allocation failed\n");
            for (int i = 0; i < PYR_LEVELS; i++) {
                if (pyr1[i] && i > 0) free(pyr1[i]);
                if (pyr2[i] && i > 0) free(pyr2[i]);
                if (gradx[i]) free(gradx[i]);
                if (grady[i]) free(grady[i]);
            }
            free(gray1);
            free(gray2);
            stbi_image_free(img1);
            stbi_image_free(img2);
            return -1;
        }
        compute_gradient(pyr1[l], gradx[l], grady[l], w, h);
    }

    // Chọn điểm gần trung tâm có gradient mạnh
    float p0[2], p1[2];
    int valid = find_strong_feature(gray1, width, height, p0);
    if (!valid) {
        printf("Error: No strong feature near center\n");
        for (int i = 0; i < PYR_LEVELS; i++) {
            if (pyr1[i] && i > 0) free(pyr1[i]);
            if (pyr2[i] && i > 0) free(pyr2[i]);
            if (gradx[i]) free(gradx[i]);
            if (grady[i]) free(grady[i]);
        }
        free(gray1);
        free(gray2);
        stbi_image_free(img1);
        stbi_image_free(img2);
        return -1;
    }

    // Tính vector chuyển dịch
    valid = lucas_kanade_pyramid(pyr1, pyr2, gradx, grady, p0, p1, width, height, PYR_LEVELS);

    // Tính dy và xác định hướng
    float dy = valid ? p1[1] - p0[1] : 0.0f;
    const float THRESHOLD = 0.2f;
    if (dy < -THRESHOLD) {
        printf("Up\n");
    } else if (dy > THRESHOLD) {
        printf("Down\n");
    } else {
        printf("Unknown\n");
    }


    printf("Valid point: %d/1\n", valid);
    printf("Translation vector: (%.2f, %.2f)\n", valid ? p1[0] - p0[0] : 0.0f, dy);

    // free
    for (int i = 0; i < PYR_LEVELS; i++) {
        if (pyr1[i] && i > 0) free(pyr1[i]);
        if (pyr2[i] && i > 0) free(pyr2[i]);
        if (gradx[i]) free(gradx[i]);
        if (grady[i]) free(grady[i]);
    }
    free(gray1);
    free(gray2);
    stbi_image_free(img1);
    stbi_image_free(img2);

    return 0;
}