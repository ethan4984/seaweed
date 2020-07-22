#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/kHeap.h>
#include <lib/memUtils.h>
#include <lib/output.h>

#include <stddef.h>

uint64_t kHeapBegin;

uint8_t *kbitmap;
uint64_t kbitmapSize;

uint64_t allocationBitmapSize;
allocation_t *allocationBitmap;

static uint64_t firstFreeSlot();
static uint64_t firstFreeAllocationSlot();

void kHeapInit() {
    kHeapBegin = physicalPageAlloc(8); // allocate a 32kb heap

    kbitmap = (void*)physicalPageAlloc(1) + HIGH_VMA; // reserve 4kb for the bitmap (complete overkill)
    kbitmapSize = (0x8000 / BLOCKSIZE); 

    allocationBitmap = (void*)physicalPageAlloc(4) + HIGH_VMA;
    allocationBitmapSize = (0x8000 / sizeof(allocation_t)); 

    /* zero out the bitmaps */

    memset(kbitmap, 0, 0x1000); 
    memset(allocationBitmap, 0, 0x4000); 

    kprintDS("[KMM]", "kHeap initalized");
}

void *kmalloc(uint64_t size) {
    uint64_t cnt = 0, blockCount = ROUNDUP(size, 32);
    void *base = (void*)(firstFreeSlot() * BLOCKSIZE);
    for(uint64_t i = firstFreeSlot(); i < kbitmapSize; i++) {
        if(isset(kbitmap, i)) {
            base += (cnt + 1) * BLOCKSIZE;
            cnt = 0;
            continue;
        }

        if(++cnt == blockCount) {
            uint64_t slot = firstFreeAllocationSlot();

            allocationBitmap[slot].size = blockCount;
            allocationBitmap[slot].block = (uint64_t)base / 32; 

            for(uint64_t j = 0; j < blockCount; j++) {
                set(kbitmap, (uint64_t)base / BLOCKSIZE + j);
            }

            return (void*)((uint64_t)base + kHeapBegin + HIGH_VMA);
        }
    }
    kprintDS("[KMM]", "Error out of pages");
    return NULL;
}

void kfree(void *addr) {
    uint64_t bitmapBase = ((uint64_t)addr - HIGH_VMA - kHeapBegin) / 32; 
    
    uint64_t i;
    for(i = 0; i < allocationBitmapSize; i++) {
        if(allocationBitmap[i].block == bitmapBase)
            break;
    }

    for(uint64_t j = bitmapBase; j < bitmapBase + allocationBitmap[i].size; j++) {
        clear(kbitmap, j);
    }

    allocationBitmap[i].size = 0;
    allocationBitmap[i].block = 0;
}

static uint64_t firstFreeSlot() {
    for(uint64_t i = 0; i < kbitmapSize; i++) {
        if(!isset(kbitmap, i))
            return i;
    }
}

static uint64_t firstFreeAllocationSlot() {
    for(uint64_t i = 0; i < allocationBitmapSize; i++) {
        if(allocationBitmap[i].size == 0) {
            return i;
        }
    }
}
