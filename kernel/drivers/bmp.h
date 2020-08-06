#ifndef BMP_H
#define BMP_H

#include <stdint.h>

typedef struct {
    uint16_t bfType;
    uint32_t bfSize;
    uint32_t reserved;
    uint32_t bfOffset;

    uint32_t biSize;
    uint32_t biWidth;
    uint32_t biHeight;
    uint16_t biPlanes;
    uint16_t biBitCount;
    uint32_t biCompression;
    uint32_t biSizeImage;
    uint32_t biXPelPerMeter;
    uint32_t biYPelPerMeter;
    uint32_t biClrUsed;
    uint32_t biClrImportant;
    uint32_t redMask;
    uint32_t greenMask;
    uint32_t blueMask;
} __attribute__((packed)) bmpFileHdr_t;

void drawBmp(const char *fileName);

#endif
