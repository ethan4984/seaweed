#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/physicalPageManager.h>
#include <lib/memUtils.h>
#include <lib/output.h>

#include <stddef.h>

uint64_t *pml4 = (uint64_t*)pml4Addr; 
uint64_t *pml3 = (uint64_t*)pml3Addr; 
uint64_t *pml2 = (uint64_t*)pml2Addr; 

uint64_t *pml3HH = (uint64_t*)pml3AddrHH;
uint64_t *pml2HH = (uint64_t*)pml2AddrHH;

struct {
    uint64_t *pml4;
    uint64_t *pml3;
    uint64_t *pml2;
    uint64_t *pml1;
    uint64_t flags;
} pageDirectoryEntry = { NULL, NULL, NULL, NULL, 0 };

typedef struct pageDirectoryEntry pageDirectoryTableEntry_t;

void initVMM() {
    memset((void*)pml4Addr, 0, 0x5000); 

    pml4[256] = ((uint64_t)&pml3[0]) | 0x3;
    pml3[0] = ((uint64_t)&pml2[0]) | 0x3;
    
    pml4[511] = ((uint64_t)&pml3HH[0]) | (1 << 8) | 0x3;
    pml3HH[510] = ((uint64_t)&pml2HH[0]) | (1 << 8) | 0x3;

    uint64_t physical = 0;
    for(int i = 0; i < 512; i++) {
        pml2[i] = physical | (1 << 7) | 0x3;
        pml2HH[i] = physical | (1 << 7) | 0x3;
        physical += 0x200000;
    }

    asm volatile("movq %0, %%cr3" :: "r" (pml4) : "memory");

    kprintDS("[KMM]", "Virtual Memory Manager Initalized");
}
