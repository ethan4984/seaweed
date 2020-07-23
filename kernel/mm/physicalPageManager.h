#ifndef PHYSICALPAGEMANAGER_H
#define PHYSICALPAGEMANAGER_H

#include <stdint.h>

void initPMM();

uint64_t physicalPageAlloc(uint64_t count);

void physicalPageFree(uint64_t base, uint64_t count);

#endif
