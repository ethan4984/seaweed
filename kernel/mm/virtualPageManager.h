#ifndef VIRTUALPAGEMANAGER_H
#define VIRTUALPAGEMANAGER_H

#define KERNEL_HIGH_VMA 0xffffffff80000000
#define HIGH_VMA 0xffff800000000000

#define kpml4Addr (0x100000 + 65536)
#define kpml3Addr (0x100000 + 65536 + 0x1000)
#define kpml2Addr_1G (0x100000 + 65536 + 0x2000)
#define kpml2Addr_2G (0x100000 + 65536 + 0x3000)
#define kpml2Addr_3G (0x100000 + 65536 + 0x4000)
#define kpml2Addr_4G (0x100000 + 65536 + 0x5000)

#define kpml2AddrHH (0x100000 + 65536 + 0x6000)
#define kpml3AddrHH (0x100000 + 65536 + 0x7000)

#define PAGESIZE 0x1000

#define ROUNDUP(a, b) (((a) + ((b) - 1)) / (b))

#include <stdint.h>

typedef struct {
    uint64_t pml4;
    uint64_t pml3;
    uint64_t pml2;
    uint64_t pml1;
    uint64_t flags;
} pageDirectoryEntry_t;

typedef struct pageDirectoryEntry pageDirectoryTableEntry_t;

typedef void *symbol[];

uint64_t createNewAddressSpace(uint64_t size, uint64_t flags);

void initAddressSpace(uint64_t index);

void freeAddressSpace(uint64_t index);

void initVMM();

#endif
