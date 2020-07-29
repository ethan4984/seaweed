#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <lib/asmUtils.h>

#include <stdbool.h>
#include <stdint.h>

extern void startTask(uint64_t rsp, uint64_t entryPoint);
extern void switchTask(uint64_t rsp, uint64_t rbp);
//extern void spinLock(uint64_t lock);
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
    uint64_t waitingTimes;
    uint64_t pml4Index;
    uint64_t rbp;
    uint64_t rsp;
    uint64_t entryPoint;
} task_t;

void schedulerInit();

void rescheduleCore(regs_t *regs);

void schedulerMain(regs_t *regs);

void createNewTask(uint64_t rsp, uint64_t entryPoint);

void spinLock(char *ptr);

void spinRelease(char *ptr);

#endif
