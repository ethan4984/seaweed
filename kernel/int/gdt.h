#ifndef GDT_H
#define GDT_H

#include <kernel/int/tss.h>

#include <stdint.h>

typedef struct {
    uint16_t limit;
    uint16_t baseLow;
    uint8_t baseMid;
    uint8_t access;
    uint8_t granularity; 
    uint8_t baseHigh;
} __attribute__((packed)) gdtEntry_t;

typedef struct {
    uint16_t length;
    uint16_t baseLow;
    uint8_t baseMid;
    uint8_t flags1;
    uint8_t flags2; 
    uint8_t baseHigh;
    uint32_t baseHigh32;
    uint32_t reserved;
} __attribute__((packed)) gdtTSSentry_t;

typedef struct {
    uint16_t size;
    uint64_t addr;
} __attribute__((packed)) gdtPtr_t;

typedef struct {
    gdtEntry_t gdtEntries[5];
    gdtTSSentry_t tss;
    gdtPtr_t gdtPtr;
} __attribute__((packed)) gdt_t;

extern void lgdt(uint64_t gdtAddr);

void initGDT();

void createNewGDT(uint64_t core, uint64_t tssAddr);

#endif
