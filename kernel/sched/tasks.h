#ifndef TASKS_H
#define TASKS_H

#include <libk/asmUtils.h>

#include <stdint.h> 

typedef struct {
    uint8_t status;
    uint64_t pml4Index;
    regs_t regs;
    uint64_t kernelStack;
    uint64_t entryPoint;
    uint64_t idleTime;
} task_t;

#endif
