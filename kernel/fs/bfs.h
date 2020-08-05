#ifndef BFS_H
#define BFS_H

#define MAGIC_NUMBER

#include <kernel/drivers/ahci.h>
#include <stdint.h>

typedef struct {
    char name[20];
    uint64_t startSector;
    uint64_t sectorCount;
} file_t;

typedef struct {
    uint64_t fileCount;
    uint8_t driveIndex;
    uint8_t *bitmap; 
    file_t *files;
} baseBlock_t;

void initGFS();

void test();

#endif
