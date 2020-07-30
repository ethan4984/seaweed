#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/kHeap.h>
#include <kernel/int/gdt.h>
#include <lib/memUtils.h>
#include <lib/output.h>

#include <stddef.h>

uint64_t *kpml4 = (uint64_t*)kpml4Addr; 
uint64_t *kpml3 = (uint64_t*)kpml3Addr; 
uint64_t *kpml2_1G = (uint64_t*)kpml2Addr_1G; 
uint64_t *kpml2_2G = (uint64_t*)kpml2Addr_2G; 
uint64_t *kpml2_3G = (uint64_t*)kpml2Addr_3G; 
uint64_t *kpml2_4G = (uint64_t*)kpml2Addr_4G; 
uint64_t *kpml3HH = (uint64_t*)kpml3AddrHH;
uint64_t *kpml2HH = (uint64_t*)kpml2AddrHH;

gdtPtr_t gdtPtr;

pageDirectoryEntry_t *pageDirectoryTables;
uint64_t pageDirectoryCount = 0;

static uint64_t findFirstFreeSlot();

uint64_t createNewAddressSpace(uint64_t size, uint64_t flags) {
    uint64_t index = findFirstFreeSlot();

    /*if(kpml4[0] != 0) {
        asm volatile ("sgdt %0" :: "m"(gdtPtr));
        gdtPtr.addr += HIGH_VMA;
        asm volatile ("lgdt %0" :: "m"(gdtPtr));
        kpml4[0] = 0;
    }*/

    pageDirectoryTables[index].pml4 = physicalPageAlloc(1);
    pageDirectoryTables[index].pml3 = physicalPageAlloc(1);
    pageDirectoryTables[index].pml2 = physicalPageAlloc(1);
    pageDirectoryTables[index].flags = flags;

    uint64_t *pml4Virtual = (uint64_t*)(pageDirectoryTables[index].pml4 + HIGH_VMA);
    uint64_t *pml3Virtual = (uint64_t*)(pageDirectoryTables[index].pml3 + HIGH_VMA);
    uint64_t *pml2Virtual = (uint64_t*)(pageDirectoryTables[index].pml2 + HIGH_VMA);

    /* inport the kernel mappings */ 

    pml4Virtual[256] = ((uint64_t)kpml3) | 0x3; // set as global and present and r/w
    pml4Virtual[511] = ((uint64_t)kpml3HH) | 0x3;
    
    pml4Virtual[0] = ((uint64_t)&pml3Virtual[0] - HIGH_VMA) | 0x3;
    pml3Virtual[0] = ((uint64_t)&pml2Virtual[0] - HIGH_VMA) | 0x3;

    if(!(pageDirectoryTables[index].flags & (1 << 7))) { /* check if 2mb pages */
        pageDirectoryTables[index].pml1 = physicalPageAlloc(1); 
        uint64_t *pml1Virtual = (uint64_t*)(pageDirectoryTables[index].pml1 + HIGH_VMA);

        pml2Virtual[0] = ((uint64_t)&pml1Virtual[0] - HIGH_VMA) | 0x3;

        uint64_t physical = physicalPageAlloc(size);
        for(uint64_t i = 0; i < size; i++) {
            pml1Virtual[i] = physical | pageDirectoryTables[index].flags;
            physical += 0x1000;
        }
    } else {
        uint64_t physical = physicalPageAlloc(size * 0x200);
        for(uint64_t i = 0; i < size; i++) {
            pml2Virtual[i] = physical | pageDirectoryTables[index].flags;
            physical += 0x200000;
        }
    }

    return index;
}

void freeAddressSpace(uint64_t index) {
    physicalPageFree(pageDirectoryTables[index].pml4, 1);
    physicalPageFree(pageDirectoryTables[index].pml3, 1);
    physicalPageFree(pageDirectoryTables[index].pml2, 1);
    physicalPageFree(pageDirectoryTables[index].pml1, 1);
    pageDirectoryEntry_t zero = { 0 };
    pageDirectoryTables[index] = zero;
}

void initAddressSpace(uint64_t index) {
    if(index > pageDirectoryCount || pageDirectoryTables[index].pml4 == 0) {
        kprintDS("[KMM]", "Error: VMM trying to enter an invaild address space");
        return;
    }

    asm volatile("movq %0, %%cr3" :: "r" (pageDirectoryTables[index].pml4) : "memory");
}

void initVMM() {
    pageDirectoryTables = kmalloc(sizeof(pageDirectoryEntry_t) * 100);
    pageDirectoryCount = 1000;

    memset(pageDirectoryTables, 0, sizeof(pageDirectoryEntry_t) * 100);
    memset((void*)kpml4Addr, 0, 0x8000);

    kpml4[256] = ((uint64_t)&kpml3[0]) | KPAGE_FLAGS; // set as global and present and r/w
    kpml4[0] = ((uint64_t)&kpml3[0]) | KPAGE_FLAGS; // set as global and present and r/w
    kpml3[0] = ((uint64_t)&kpml2_1G[0]) | KPAGE_FLAGS;
    kpml3[1] = ((uint64_t)&kpml2_2G[0]) | KPAGE_FLAGS; 
    kpml3[2] = ((uint64_t)&kpml2_3G[0]) | KPAGE_FLAGS;
    kpml3[3] = ((uint64_t)&kpml2_4G[0]) | KPAGE_FLAGS; 

    kpml4[511] = ((uint64_t)&kpml3HH[0]) | KPAGE_FLAGS;
    kpml3HH[510] = ((uint64_t)&kpml2HH[0]) | KPAGE_FLAGS;

    uint64_t physical = 0;
    for(int i = 0; i < 512; i++) {
        kpml2_1G[i] = physical | (1 << 7) | KPAGE_FLAGS;
        kpml2_2G[i] = (physical + 0x40000000) | (1 << 7) | KPAGE_FLAGS;
        kpml2_3G[i] = (physical + (uint64_t)0x40000000 * 2) | (1 << 7) | KPAGE_FLAGS;
        kpml2_4G[i] = (physical + (uint64_t)0x40000000 * 3) | (1 << 7) | KPAGE_FLAGS;
        kpml2HH[i] = physical | (1 << 7) | KPAGE_FLAGS;
        physical += 0x200000;
    }

    asm volatile("movq %0, %%cr3" :: "r" (kpml4) : "memory");

    kprintDS("[KMM]", "Virtual Memory Manager Initalized");
}

uint64_t grabPML4() {
    uint64_t pml4;
    asm volatile ("movq %%cr3, %0" : "=r"(pml4));
    return pml4;
}

static uint64_t findFirstFreeSlot() { 
    for(uint64_t i = 0; i < pageDirectoryCount; i++) {
        if(pageDirectoryTables[i].pml4 == 0) {
            return i; 
        }
    }
    return -1;
}
