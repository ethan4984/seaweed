#ifndef MEMUTILS_H
#define MEMUTILS_H

#include <stdint.h>
#include <stdbool.h>

void memset(void *src, int64_t data, uint64_t count);

void memset8(uint8_t *src, uint8_t data, uint64_t count);

void memset16(uint16_t *src, uint16_t data, uint64_t count);

void memset32(uint32_t *src, uint32_t data, uint64_t count);

void memset64(uint64_t *src, uint64_t data, uint64_t count);

void set(uint8_t *bitmap, uint64_t location);

void clear(uint8_t *bitmap, uint64_t location);

bool isset(uint8_t *bitmap, uint64_t location);

#endif
