#ifndef IDT_H
#define IDT_H

#include <stdint.h>

typedef struct {
    uint16_t offsetLow;
    uint16_t selector;
    uint8_t zero8;
    uint8_t attributes;
    uint16_t offsetMid;
    uint32_t offsetHigh;
    uint32_t zero32;
} idtEntry_t;

typedef struct {
    uint16_t limit;
    uint64_t offset;
} idtr_t;

#endif 
