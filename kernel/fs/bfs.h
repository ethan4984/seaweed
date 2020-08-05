#ifndef BFS_H
#define BFS_H

#define MAGIC_NUMBER 0x69694200

#include <kernel/drivers/ahci.h>
#include <stdint.h>

typedef struct {
    char fileName[20];
    uint32_t blockNumber;
    uint32_t blockCount;
    uint8_t present;
} __attribute__((packed)) inode_t;

typedef struct {
    uint32_t signature;
    uint32_t blockCount;
    uint32_t inodeCount;
    uint32_t unallocatedInode;
    uint32_t unallocatedBlockCnt;
    uint32_t blockSize;
    uint32_t inodeSize;
    uint8_t reserved[0x200 - 28];
    inode_t inodes[];
} __attribute__((packed)) superBlock_t;

void initGFS();

void test();

#endif
