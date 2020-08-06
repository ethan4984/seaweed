#include <kernel/mm/virtualPageManager.h>
#include <kernel/drivers/ahci.h> 
#include <kernel/drivers/vesa.h>
#include <kernel/mm/kHeap.h>
#include <kernel/fs/bfs.h>
#include <libk/memUtils.h>
#include <libk/string.h>
#include <libk/output.h>

#include <stdbool.h>

#define SUPER_BLOCK_SECTOR 0x4000
#define FILES_SECTOR (0x4000 + (0x200000 / 0x200))

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
}

static int64_t inodeLookUp(const char *fileName) {
    for(uint64_t i = 0; i < superBlock->inodeCount - superBlock->unallocatedInode; i++) {
        if(strcmp(superBlock->inodes[i].fileName, fileName) == 0) 
            return i;
    }
    return -1;
}

void *openFile(const char *fileName) {
    int64_t inodeIndex = inodeLookUp(fileName);
    if(inodeIndex == -1) {
        kprintDS("[FS]", "No file by the name of %s", fileName);
        return NULL;
    }

    void *filePtr = kmalloc(0x200);
    sataRW(&drives.drive[0], FILES_SECTOR + superBlock->inodes[inodeIndex].blockNumber, 1, filePtr, 0);

    return filePtr;
}

void fpInc(const char *fileName, void *fp, uint32_t num) {
    int64_t inodeIndex = inodeLookUp(fileName);
    if(inodeIndex == -1) {
        kprintDS("[FS]", "No file by the name of %s", fileName);
        return;
    }

    sataRW(&drives.drive[0], FILES_SECTOR + superBlock->inodes[inodeIndex].blockNumber + num, 1, fp, 0);
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
