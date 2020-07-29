#include <kernel/mm/virtualPageManager.h>
#include <kernel/sched/scheduler.h>
#include <kernel/sched/smp.h>
#include <kernel/sched/hpet.h>
#include <kernel/mm/kHeap.h>
#include <kernel/int/apic.h>
#include <lib/asmUtils.h>
#include <lib/memUtils.h>
#include <lib/output.h>

#include <stddef.h>

static int64_t findFreeIndex();

cpuInfo_t *cpuInfo;
task_t *tasks;

uint64_t numberOfTasks = 0, maxNumberOfTasks = 10;

void schedulerMain(regs_t *regs) {
    asm volatile ("cli");

    static char lock = 0;
    spinLock(&lock);

    if(numberOfTasks == 0) {
        spinRelease(&lock); 
        asm volatile ("sti");
        return;
    }

    int64_t oldTask = cpuInfo[regs->core].currentTask;
    int64_t nextTaskIndex = -1, highestWait = 0;

    for(uint64_t i = 0; i < numberOfTasks; i++) {
        if(tasks[i].status == WAITING) {
            tasks[i].waitingTimes++;
            if(highestWait < tasks[i].waitingTimes) {
                highestWait = tasks[i].waitingTimes;
                nextTaskIndex = i;
            }
        }
    }
    
    for(uint64_t i = 0; i < numberOfTasks; i++) {
        if(tasks[i].status == WAITING_TO_START && i != oldTask) {
            nextTaskIndex = i;
            break;
        }
    }

    if(nextTaskIndex == -1) {
        spinRelease(&lock);
        asm volatile ("sti");
        return;
    }

    if(tasks[oldTask].status == RUNNING) {
        tasks[oldTask].rsp = (uint64_t)regs;
        tasks[oldTask].rbp = (uint64_t)regs;
        tasks[oldTask].status = WAITING;
    }

    cpuInfo[regs->core].currentTask = nextTaskIndex;

    if(tasks[nextTaskIndex].status == WAITING_TO_START) {
        tasks[nextTaskIndex].status = RUNNING; 
        tasks[nextTaskIndex].waitingTimes = 0; 
        initAddressSpace(tasks[nextTaskIndex].pml4Index);
        spinRelease((char*)&lock); 
        startTask(tasks[nextTaskIndex].rsp, tasks[nextTaskIndex].entryPoint);
    }

   // kprintDS("[SMP]", "switching task on %d and coming from %d", nextTaskIndex, oldTask);
    tasks[nextTaskIndex].status = RUNNING;
    tasks[nextTaskIndex].waitingTimes = 0; 
    initAddressSpace(tasks[nextTaskIndex].pml4Index);
    spinRelease(&lock); 
    asm volatile ("sti");
    switchTask(tasks[nextTaskIndex].rsp, tasks[nextTaskIndex].rsp);
}

void schedulerInit() {
    cpuInfo = grabCPUinfo();
    tasks = kmalloc(sizeof(task_t) * 10);
}

void createNewTask(uint64_t rsp, uint64_t entryPoint) {
    int64_t currentIndex = findFreeIndex();

    if(currentIndex == -1) {
        tasks = krealloc(tasks, sizeof(task_t) * 10);
        maxNumberOfTasks += 10;
    }

    task_t task = { WAITING_TO_START, 0, createNewAddressSpace(10, 0x3), rsp, rsp,  entryPoint };
    tasks[currentIndex] = task;
    numberOfTasks++; 
}

static int64_t findFreeIndex() {
    for(uint64_t i = 0; i < maxNumberOfTasks; i++) {
        if(tasks[i].rsp == 0) 
            return i;
    }
    return -1;
}

void spinLock(char *ptr) {
    volatile uint64_t deadLockCnt = 0; 
    while (__atomic_test_and_set(ptr, __ATOMIC_ACQUIRE)) {
        if(++deadLockCnt == 100000000) {
            asm volatile ("int $0x15");
        }
    }
}

void spinRelease(char *ptr) {
    __atomic_clear(ptr, __ATOMIC_RELEASE);
}
