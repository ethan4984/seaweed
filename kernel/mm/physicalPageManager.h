#ifndef PHYSICALPAGEMANAGER_H
#define PHYSICALPAGEMANAGER_H

#include <stdint.h>

void initPMM();

void *physicalPageAlloc(uint64_t count);

void physicalPageFree(void *base, uint64_t count);

#endif
