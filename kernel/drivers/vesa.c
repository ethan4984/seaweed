#include <kernel/drivers/font.h>
#include <kernel/bproto.h>
#include <lib/output.h>

#include <stdint.h>
    
uint16_t height, width, pitch, bpp;
uint32_t framebuffer;

void initVESA(bproto_t *bproto) {
    framebuffer = bproto->framebuffer; 
    height = bproto->height;
    width = bproto->width;
    pitch = bproto->pitch;
    bpp = bproto->bpp;
}

void setPixel(uint16_t x, uint16_t y, uint32_t colour) {
    *(volatile uint32_t*)((uint64_t)framebuffer + ((y * pitch) + (x * bpp / 8))) = colour; 
}

uint32_t grabColour(uint16_t x, uint16_t y) {
    return *(volatile uint32_t*)((uint64_t)framebuffer + ((y * pitch) + (x * bpp / 8)));
}

void renderChar(uint64_t x, uint64_t y, uint32_t fg, uint32_t bg, char c) {
    for(uint8_t i = 0; i < 8; i++) {
        for(uint8_t j = 0; j < 8; j++) {
            if((font[(uint8_t)c][i] >> j) & 1) {
                setPixel(j + x, y + i, fg);
            }
            else {
                setPixel(j + x, y + i, bg);
            }
        }
    }
}
