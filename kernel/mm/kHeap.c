#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/kHeap.h>
#include <libk/memUtils.h>
#include <libk/output.h>

#include <stddef.h>

uint64_t kHeapBegin;

uint8_t *kbitmap;
int64_t kbitmapSize;

int64_t allocationBitmapSize;
allocation_t *allocationBitmap;

static int64_t firstFreeSlot();
static int64_t firstFreeAllocationSlot();

void kHeapInit() {
    kHeapBegin = physicalPageAlloc(0x100); // allocate a 1mb heap

    kbitmap = (void*)physicalPageAlloc(1) + HIGH_VMA; // reserve 4kb for the bitmap (complete overkill)
    kbitmapSize = (0x100000 / BLOCKSIZE); 

    allocationBitmap = (void*)physicalPageAlloc(16) + HIGH_VMA;
    allocationBitmapSize = (0x100000 / sizeof(allocation_t)); 

    /* zero out the bitmaps */

    memset(kbitmap, 0, 0x1000); 
    memset(allocationBitmap, 0, 0x10000); 

    kprintDS("[KMM]", "kHeap initalized");
}

void *kmalloc(uint64_t size) {
    static uint64_t i = 0;
    int64_t cnt = 0, blockCount = ROUNDUP(size, 32);
    void *base = (void*)(firstFreeSlot() * BLOCKSIZE);
    for(int64_t i = firstFreeSlot(); i < kbitmapSize; i++) {
        if(isset(kbitmap, i)) {
            base += (cnt + 1) * BLOCKSIZE;
            cnt = 0;
            continue;
        }

        if(++cnt == blockCount) {
            uint64_t slot = firstFreeAllocationSlot();

            allocationBitmap[slot].size = blockCount;
            allocationBitmap[slot].block = (uint64_t)base / 32; 

            for(int64_t j = 0; j < blockCount; j++) {
                set(kbitmap, (uint64_t)base / BLOCKSIZE + j);
            }

            return (void*)((uint64_t)base + kHeapBegin + HIGH_VMA);
        }
    }
    kprintDS("[KMM]", "Error out of pages");
    return NULL;
}

uint64_t kfree(void *addr) {
    uint64_t bitmapBase = ((uint64_t)addr - HIGH_VMA - kHeapBegin) / 32, sizeOfAllocation = 0; 
    
    int64_t i;
    for(i = 0; i < allocationBitmapSize; i++) {
        if(allocationBitmap[i].block == bitmapBase)
            break;
    }

    sizeOfAllocation = allocationBitmap[i].size;

    for(uint64_t j = bitmapBase; j < bitmapBase + sizeOfAllocation; j++) {
        clear(kbitmap, j);
    }

    allocationBitmap[i].size = 0;
    allocationBitmap[i].block = 0;

    return sizeOfAllocation;
}

void *krealloc(void *addr, uint64_t size) {
    uint64_t sizeOfAllocation = kfree(addr);
    void *newAddr = kmalloc(sizeOfAllocation + size);
    memcpy64(newAddr, addr, sizeOfAllocation);
    return newAddr;
}

static int64_t firstFreeSlot() {
    for(int64_t i = 0; i < kbitmapSize; i++) {
        if(!isset(kbitmap, i))
            return i;
    }
    return -1;
}

static int64_t firstFreeAllocationSlot() {
    for(int64_t i = 0; i < allocationBitmapSize; i++) {
        if(allocationBitmap[i].size == 0) {
            return i;
        }
    }
    return -1;
}
