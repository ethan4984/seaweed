#ifndef KHEAP_H
#define KHEAP_H

#include <stdint.h>

#define BLOCKSIZE 32

typedef struct {
    uint16_t block;
    uint16_t size;
} allocation_t;

void *kmalloc(uint64_t size);

uint64_t kfree(void *addr);

void *krealloc(void *addr, uint64_t size);

void kHeapInit();

#endif
