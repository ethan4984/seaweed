#ifndef GFS_H
#define GFS_H

#include <kernel/drivers/ahci.h>
#include <stdint.h>

typedef struct { 
    uint64_t sectorCount; 
    volatile hbaPorts_t *hbaPort;
} drives_t;

void addDrive(uint64_t sectorCount, volatile hbaPorts_t *hbaPort); 

void initGFS();
void test();

#endif
