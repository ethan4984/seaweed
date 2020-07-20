#ifndef VIRTUALPAGEMANAGER_H
#define VIRTUALPAGEMANAGER_H

#define KERNEL_HIGH_VMA 0xffffffff80000000
#define HIGH_VMA 0xffff800000000000

#define PAGESIZE 0x1000

#define ROUNDUP(a, b) (((a) + ((b) - 1)) / (b))

#include <stdint.h>

typedef void *symbol[];

uint64_t createNewAddressSpace(uint64_t size);

void initAddressSpace(uint64_t index);

void initVMM();

#endif
