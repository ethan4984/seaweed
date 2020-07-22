#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/kHeap.h>
#include <lib/memUtils.h>
#include <lib/output.h>

#include <stddef.h>

static uint64_t findFirstFreeSlot();

uint64_t *pml4 = (uint64_t*)pml4Addr; 
uint64_t *pml3 = (uint64_t*)pml3Addr; 
uint64_t *pml2 = (uint64_t*)pml2Addr; 

uint64_t *pml3HH = (uint64_t*)pml3AddrHH;
uint64_t *pml2HH = (uint64_t*)pml2AddrHH;

pageDirectoryEntry_t *pageDirectoryTables;
uint64_t pageDirectoryCount = 0;

uint16_t *hash;

uint64_t createNewAddressSpace(uint64_t size, uint64_t flags) {
    uint64_t hB = findFirstFreeSlot();
    hash[hB] = 1; /* present */

    pageDirectoryTables[hB].pml4 = physicalPageAlloc(1);
    pageDirectoryTables[hB].pml3 = physicalPageAlloc(1);
    pageDirectoryTables[hB].pml2 = physicalPageAlloc(1);
    pageDirectoryTables[hB].flags = flags;

    uint64_t *pml4Virtual = (uint64_t*)(pageDirectoryTables[hB].pml4 + HIGH_VMA);
    uint64_t *pml3Virtual = (uint64_t*)(pageDirectoryTables[hB].pml3 + HIGH_VMA);
    uint64_t *pml2Virtual = (uint64_t*)(pageDirectoryTables[hB].pml2 + HIGH_VMA);

    pml4Virtual[0] = ((uint64_t)&pml3Virtual[0] - HIGH_VMA) | 0x3;
    pml3Virtual[0] = ((uint64_t)&pml2Virtual[0] - HIGH_VMA) | 0x3;

    if(!(pageDirectoryTables[hB].flags & (1 << 7))) { /* check if 2mb pages */
        pageDirectoryTables[hB].pml1 = physicalPageAlloc(1); 
        uint64_t *pml1Virtual = (uint64_t*)(pageDirectoryTables[hB].pml1 + HIGH_VMA);

        pml2Virtual[0] = ((uint64_t)&pml1Virtual[0] - HIGH_VMA) | 0x3;

        uint64_t physical = physicalPageAlloc(size);
        for(uint64_t i = 0; i < size; i++) {
            pml1Virtual[i] = physical | pageDirectoryTables[hB].flags;
            physical += 0x1000;
        }
    } else {
        uint64_t physical = physicalPageAlloc(size * 0x200);
        for(uint64_t i = 0; i < size; i++) {
            pml2Virtual[i] = physical | pageDirectoryTables[hB].flags;
            physical += 0x200000;
        }
    }

    return hB;
}

void initAddressSpace(uint64_t index) {
    if(index > pageDirectoryCount || hash[index] == 0) {
        kprintDS("[KMM]", "Error: Trying to enter an invaild address space");
        return;
    }

    asm volatile("movq %0, %%cr3" :: "r" (pageDirectoryTables[index].pml4) : "memory");
}

void initVMM() {
    pageDirectoryTables = kmalloc(sizeof(pageDirectoryEntry_t) * 100);
    pageDirectoryCount = 100;

    hash = kmalloc(sizeof(uint16_t) * 100);

    memset(pageDirectoryTables, 0, sizeof(pageDirectoryEntry_t) * 100);
    memset(hash, 0xff, sizeof(uint16_t) * 100);
    memset((void*)pml4Addr, 0, 0x5000); 

    pml4[256] = ((uint64_t)&pml3[0]) | (1 << 8) | 0x3; // set as global and present and r/w
    pml3[0] = ((uint64_t)&pml2[0]) | (1 << 8) | 0x3;
    
    pml4[511] = ((uint64_t)&pml3HH[0]) | (1 << 8) | 0x3;
    pml3HH[510] = ((uint64_t)&pml2HH[0]) | (1 << 8) | 0x3;

    uint64_t physical = 0;
    for(int i = 0; i < 512; i++) {
        pml2[i] = physical | (1 << 8) | (1 << 7) | 0x3; // set as global present and r/w and size
        pml2HH[i] = physical | (1 << 8) | (1 << 7) | 0x3;
        physical += 0x200000;
    }

    asm volatile("movq %0, %%cr3" :: "r" (pml4) : "memory");

    kprintDS("[KMM]", "Virtual Memory Manager Initalized");
}

static uint64_t findFirstFreeSlot() { 
    for(uint64_t i = 0; i < pageDirectoryCount; i++) {
        if(hash[i] == 0xffff) {
            return i; 
        }
    }
}
