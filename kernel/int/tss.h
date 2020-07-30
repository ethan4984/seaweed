#ifndef TSS_H
#define TSS_H

#include <stdint.h>

typedef struct {
    uint32_t reserved;
    uint64_t rsp0;
    uint64_t rsp1;
    uint64_t rsp2;
    uint32_t reserved1;
    uint32_t reserved2;
    uint64_t ist1;
    uint64_t ist2;
    uint64_t ist3;
    uint64_t ist4;
    uint64_t ist5;
    uint64_t ist6;
    uint64_t ist7;
    uint64_t reserved3;
    uint16_t reserved4;
    uint16_t IOPB;
} __attribute__((packed)) tss_t;

void initTSS();

uint64_t grabTSS(uint64_t index);

void createGenericTSS(uint64_t rsp0);

void addNewTSS(tss_t newTSS);

#endif
