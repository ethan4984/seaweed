#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/kHeap.h>
#include <kernel/drivers/ahci.h> 
#include <kernel/fs/bfs.h>
#include <libk/memUtils.h>
#include <libk/string.h>
#include <libk/output.h>

#include <stdbool.h>

#define FILEINFOSECTOR 0x4000

static drives_t drives;

baseBlock_t *baseBlock;

static uint64_t findFile(const char *name);

void initGFS() {
    drives = getDrives();
   
    baseBlock = kmalloc(0x200);
    baseBlock->bitmap = (uint8_t*)(physicalPageAlloc(128) + HIGH_VMA);
    baseBlock->files = (file_t*)(physicalPageAlloc(16) + HIGH_VMA);

    sataRW(&drives.drive[0], FILEINFOSECTOR, 1, (uint16_t*)baseBlock, 0); // read in the header
    sataRW(&drives.drive[0], FILEINFOSECTOR + 1 + 1024, 1024, (uint16_t*)baseBlock->bitmap, 0); // read in the bitmap
    sataRW(&drives.drive[0], FILEINFOSECTOR + 1 + 1024, 1024, (uint16_t*)baseBlock->files, 0); // read in the file info
}

uint64_t findFreeSector(uint64_t start) {
    if(start >= drives.drive[0].sectorCount) {
        kprintDS("[AHCI]", "stop");
        return 0;
    }

    for(uint64_t i = start; i < drives.drive[0].sectorCount; i++) {
        if(!isset(baseBlock->bitmap, i))
            return i;
    }
}

void createFile(const char *name, uint64_t size) {
    uint64_t start = findFreeSector(0);

    for(uint64_t i = start; i < start + size; i++) { 
        set(baseBlock->bitmap, i);
    }

    uint64_t fileIndex;
    for(fileIndex = 0; fileIndex < baseBlock->fileCount; fileIndex++) {
        if(baseBlock->files[fileIndex].name[0] == 0) {
            break; 
        }
    }

    strcpy(baseBlock->files[fileIndex].name, name);
    baseBlock->files[fileIndex].startSector = start;
    baseBlock->files[fileIndex].sectorCount = size;
}

void readFile(const char *name, void *buffer, uint64_t start) {
    uint64_t fileIndex = findFile(name);
    sataRW(&drives.drive[0], baseBlock->files[fileIndex].startSector + start, 1, buffer, 0);
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

static uint64_t findFile(const char *name) {
    for(uint64_t fileIndex = 0; fileIndex < baseBlock->fileCount; fileIndex++) {
        if(baseBlock->files[fileIndex].name[0] == 0) {
            return fileIndex; 
        }
    }
}
