#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/physicalPageManager.h>
#include <kernel/bproto.h>
#include <lib/memUtils.h>
#include <lib/output.h>

static void allocateRegion(uint64_t start, uint64_t length);
static uint64_t firstFreePage();

uint8_t *bitmap;
uint64_t totalDetectedMemory = 0;

void initPMM(bproto_t *bproto) {
    bitmap = (uint8_t*)0xF900 + 64000;
    memset(bitmap, 0, 0x20000);

    mmapEntry_t *mmapEntries = (mmapEntry_t*)bproto->mmapAddress;
    for(uint64_t i = 0; i < bproto->mmapEntries; i++) {
        totalDetectedMemory += mmapEntries[i].length;
        if(mmapEntries[i].type != 1) {
            allocateRegion(mmapEntries[i].base, mmapEntries[i].length);
        }
    }

    kprintDS("Total Detected Memory: %x", totalDetectedMemory);
}

void *physicalPageAlloc(uint64_t count) {
    uint64_t cnt = 0;
    void *base = (void*)(firstFreePage() * PAGESIZE);
    for(uint64_t i = firstFreePage(); i < totalDetectedMemory / PAGESIZE; i++) {
        if(isset(bitmap, i)) {
            base += (cnt + 1) * PAGESIZE;
            cnt = 0;
            continue;
        }
        
        if(++cnt == count) {
            for(uint64_t j = 0; j < count; j++) {
                set(bitmap, (uint64_t)base / PAGESIZE + j);
            }
            return base; 
        }
    }
}

void physicalPageFree(void *base, uint64_t count) {
    for(uint64_t i = ROUNDUP((uint64_t)base, PAGESIZE); i < ROUNDUP((uint64_t)base, PAGESIZE) + count; i++) {
        clear(bitmap, i);
    }
}

static void allocateRegion(uint64_t start, uint64_t length) {
    for(uint64_t i = start / PAGESIZE; i < (start / PAGESIZE) + (length / PAGESIZE); i++) {
        set(bitmap, i);
    }
}

static uint64_t firstFreePage() {
    for(uint64_t i = 0; i < totalDetectedMemory / PAGESIZE; i++) {
        if(!isset(bitmap, i)) {
            return i;
        }
    }
}
