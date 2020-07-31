#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <lib/asmUtils.h>

#include <stdbool.h>
#include <stdint.h>

extern void startTask(uint64_t ss, uint64_t rsp, uint64_t cs, uint64_t entryPoint);
extern void switchTask(uint64_t rsp, uint64_t dataSegment);
extern void endOfInterrupt();

enum {
    WAITING_TO_START = 1, 
    RUNNING,
    WAITING,
    START_TASK,
    SWITCH_TASK,
    GIVE_ME_YOUR_STACK_AND_WAIT
};

typedef struct {
    uint8_t status;
    uint64_t pml4Index;
    uint64_t rsp;
    uint16_t cs;
    uint16_t ss;
    uint64_t kernelStack;
    uint64_t entryPoint;
    uint64_t idleTime;
} task_t;

void schedulerInit();

void rescheduleCore(regs_t *regs);

void schedulerMain(regs_t *regs);

void createNewTask(uint16_t ss, uint64_t rsp, uint16_t cs, uint64_t entryPoint, uint64_t size);

void spinLock(char *ptr);

void spinRelease(char *ptr);

#endif
