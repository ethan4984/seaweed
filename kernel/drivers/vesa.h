#ifndef VESA_H
#define VESA_H

#include <kernel/bproto.h>

void initVESA(bproto_t *bproto);

void renderChar(uint64_t x, uint64_t y, uint32_t fg, uint32_t bg, char c);

void setPixel(uint16_t x, uint16_t y, uint32_t colour);

uint32_t grabColour(uint16_t x, uint16_t y);

#endif
