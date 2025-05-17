#include <stdio.h>
#include <stdlib.h>
#include "optical_flow.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Usage: %s <image1_path> <image2_path>\n", argv[0]);
        return -1;
    }

    int width, height, channels;
    unsigned char *img1 = stbi_load(argv[1], &width, &height, &channels, 3);
    unsigned char *img2 = stbi_load(argv[2], &width, &height, &channels, 3);

    if (!img1 || !img2) {
        printf("Error: Cannot load images\n");
        if (img1) stbi_image_free(img1);
        if (img2) stbi_image_free(img2);
        return -1;
    }

    if (width != WIDTH || height != HEIGHT || channels != 3) {
        printf("Error: Image size does not match (%dx%d) or not RGB\n", WIDTH, HEIGHT);
        stbi_image_free(img1);
        stbi_image_free(img2);
        return -1;
    }

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

    valid = lucas_kanade_pyramid(pyr1, pyr2, gradx, grady, p0, p1, width, height, PYR_LEVELS);

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