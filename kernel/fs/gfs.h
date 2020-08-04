#ifndef GFS_H
#define GFS_H

#define MAGIC_NUMBER 0x69dab420

#include <kernel/drivers/ahci.h>
#include <stdint.h>

typedef struct {
    char *fileName;
    uint8_t driveIndex;
    uint64_t start;
    uint64_t sectorCount;
} file_t;

typedef struct directory {
    file_t *files;
    struct directory *subDirectories;
} directory_t;

void initGFS();

void test();

#endif
