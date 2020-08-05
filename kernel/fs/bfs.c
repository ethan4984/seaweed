#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/kHeap.h>
#include <kernel/drivers/ahci.h> 
#include <kernel/fs/bfs.h>
#include <libk/memUtils.h>
#include <libk/string.h>
#include <libk/output.h>

#include <stdbool.h>

#define SUPER_BLOCK_SECTOR 0x4000

static drives_t drives;
static superBlock_t *superBlock;
static int64_t inodeLookUp(const char *fileName);

void initGFS() {
    drives = getDrives();
    
    superBlock = (void*)(physicalPageAlloc(0x200) + HIGH_VMA);

    for(uint64_t i = 0; i < 0x1000; i++) {
        sataRW(&drives.drive[0], SUPER_BLOCK_SECTOR + i, 1, (void*)((uint64_t)superBlock + (i * 0x200)), 0); // read the superblock in from disk
    }

    if(superBlock->signature != MAGIC_NUMBER) {
        kprintDS("[FS]", "Fatal error: signature not found %x", superBlock->signature); 
        return;
    }

    superBlock->blockCount = (drives.drive[0].sectorCount * 512) / superBlock->blockSize;
    superBlock->unallocatedBlockCnt = (drives.drive[0].sectorCount * 512) / superBlock->blockSize;
    superBlock->unallocatedInode = (0x200000 / superBlock->inodeSize);
    superBlock->inodeCount = (0x200000 / superBlock->inodeSize);

    kprintDS("[FS]", "possible block count: %d", superBlock->blockCount);
    kprintDS("[FS]", "unallocated inodes: %d", superBlock->unallocatedInode);
    kprintDS("[FS]", "unallocated blocks: %d", superBlock->unallocatedBlockCnt);
    kprintDS("[FS]", "Block size: %d", superBlock->blockSize);
    kprintDS("[FS]", "inode size %d", superBlock->inodeSize);

    for(uint64_t i = 0; i < superBlock->inodeCount; i++) {
        if((superBlock->inodes[i].fileName[i] != 0) && (superBlock->inodes[i].present == 0)) {
            superBlock->unallocatedInode--;
        }
    }

    int64_t fileLocation = inodeLookUp("bruh");

    if(fileLocation == -1) {
        kprintDS("[FS]", "file not found");
    } else {
        kprintDS("[FS]", "file found at index %d", fileLocation);
    }
}

static int64_t inodeLookUp(const char *fileName) {
    for(uint64_t i = 0; i < superBlock->inodeCount - superBlock->unallocatedInode; i++) {
        if(strcmp(superBlock->inodes[i].fileName, fileName) == 0) 
            return i;
    }
    return -1;
}

void test() {
    uint16_t *buffer = kmalloc(512);
    
    memset(buffer, 0, 0x200);
    sataRW(&drives.drive[0], 0, 1, buffer, 0);

    for(uint64_t i = 0; i < 0x200 / 2; i++) {
        kprintDS("[KDEBUG]", "%x ", buffer[i]);
    }

    kprintDS("[KDEBUG]", "BRUH LEL");
    kprintDS("[KDEBUG]", "BRUH LEL");

    memset(buffer, 0, 0x200);
    buffer[0] = 0x69;
    sataRW(&drives.drive[0], 0, 1, buffer, 1);

    memset(buffer, 0, 0x200);
    sataRW(&drives.drive[0], 0, 1, buffer, 0);

    for(uint64_t i = 0; i < 0x200 / 2; i++) {
        kprintDS("[KDEBUG]", "%x ", buffer[i]);
    }
}
