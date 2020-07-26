#ifndef SMP_H
#define SMP_H

#include <stdint.h>

typedef struct {
    uint64_t coreID;
    uint64_t numberOfTasks;
    uint64_t currentTask;
} cpuInfo_t;

void initSMP();

cpuInfo_t *grabCPUinfo();

#endif
