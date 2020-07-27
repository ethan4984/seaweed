#ifndef BPROTO_H
#define BPROTO_H

#include <stdint.h>

typedef struct {
    uint64_t mmapAddress;
    uint8_t mmapEntries;
    uint32_t rsdp;
    uint16_t pitch;
    uint16_t width;
    uint16_t height;
    uint8_t bpp;
    uint32_t framebuffer;
} __attribute__((packed)) bproto_t;

typedef struct {
    uint64_t base;
    uint64_t length;
    uint32_t type;
    uint32_t unused;
} __attribute__((packed)) mmapEntry_t;

#endif
