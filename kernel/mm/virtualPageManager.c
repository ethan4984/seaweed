#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/physicalPageManager.h>
#include <lib/memUtils.h>

#define pml4Addr (0x100000 + 64000)
#define pml3Addr (0x100000 + 64000 + 0x1000)
#define pml2Addr (0x100000 + 64000 + 0x2000)
#define pml1Addr (0x100000 + 64000 + 0x3000)

typedef struct {
    
} pageDirectoryEntry_t;

uint64_t createNewAddressSpace(uint64_t size) {
        
}

void initVMM() {
    memset((void*)(0x100000 + 64000), 0, 0x4000); 

    uint64_t *pml4 = (uint64_t*)pml4Addr; 
    uint64_t *pml3 = (uint64_t*)pml3Addr; 
    uint64_t *pml2 = (uint64_t*)pml2Addr; 
    uint64_t *pml1 = (uint64_t*)pml1Addr; 

    asm volatile("movq %0, %%cr3;" ::"r"(pml4) : "memory");
}
