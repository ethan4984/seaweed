#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/sched/scheduler.h>
#include <kernel/sched/smp.h>
#include <kernel/sched/hpet.h>
#include <kernel/mm/kHeap.h>
#include <kernel/int/apic.h>
#include <kernel/int/tss.h>
#include <libk/asmUtils.h>
#include <libk/memUtils.h>
#include <libk/output.h>

#include <stddef.h>

static int64_t findFreeIndex();
static void setKernelStack(uint64_t currentCoreNumber, uint64_t newKernelStack);

cpuInfo_t *cpuInfo;
task_t *tasks;

uint64_t numberOfTasks = 0, maxNumberOfTasks = 10;

void schedulerMain(regs_t *regs) {
    static char lock = 0;
    spinLock(&lock);

    if(numberOfTasks == 0) 
        goto end;

    int64_t nextTask = -1, lastTask = cpuInfo[regs->core].currentTask;
    uint64_t cnt = 0;
    
    for(uint64_t i = 0; i < numberOfTasks; i++) { /* find the next task to run */
        if(tasks[i].status == WAITING) {
            if(cnt < ++tasks[i].idleTime) {
                cnt = tasks[i].idleTime;
                nextTask = i; 
            }
        }

        if(tasks[i].status == WAITING_TO_START) {
            nextTask = i; 
            break;
        }
    }

    if(nextTask == -1) 
        goto end;

    if(lastTask != -1) { // potental bug
        tasks[lastTask].regs = *regs;
        tasks[lastTask].status = WAITING;
    }

    cpuInfo[regs->core].currentTask = nextTask;

    tasks[nextTask].idleTime = 0;
    initAddressSpace(tasks[nextTask].pml4Index); // pml4 as whatever its supposed tp be
    setKernelStack(regs->core, tasks[nextTask].kernelStack); 

    if(tasks[nextTask].status == WAITING_TO_START) {
        tasks[nextTask].status = RUNNING;
        spinRelease(&lock);
        startTask(tasks[nextTask].regs.ss, tasks[nextTask].regs.rsp, tasks[nextTask].regs.cs, tasks[nextTask].entryPoint);
    }

    if(tasks[nextTask].status == WAITING) {
        tasks[nextTask].status = RUNNING;
        spinRelease(&lock);
        switchTask((uint64_t)&tasks[nextTask].regs, tasks[nextTask].regs.ss);
    }

end:
    spinRelease(&lock);
    return;
}

void schedulerInit() {
    cpuInfo = grabCPUinfo();
    tasks = kmalloc(sizeof(task_t) * 10);
}

void createNewTask(uint16_t ss, uint64_t rsp, uint16_t cs, uint64_t entryPoint, uint64_t pageCnt) {
    int64_t index = findFreeIndex();

    if(index == -1) {
        tasks = krealloc(tasks, sizeof(task_t) * 10);
        maxNumberOfTasks += 10;
    }

    regs_t regs;

    regs.rsp = rsp;
    regs.ss = ss;
    regs.cs = cs;

    task_t newTask = {  WAITING_TO_START, // status
                        createNewAddressSpace(pageCnt, (1 << 2) | 0x3), // pml4Index
                        regs,
                        physicalPageAlloc(2) + 0x2000 + HIGH_VMA, // kernelStack
                        entryPoint,
                        0
                     };

    tasks[index] = newTask;
    numberOfTasks++; 
}

static int64_t findFreeIndex() {
    for(uint64_t i = 0; i < maxNumberOfTasks; i++) {
        if(tasks[i].kernelStack == 0) 
            return i;
    }
    return -1;
}

static void setKernelStack(uint64_t currentCoreNumber, uint64_t newKernelStack) {
    tss_t *tss = (tss_t*)grabTSS(currentCoreNumber);
    tss[currentCoreNumber].rsp0 = newKernelStack;
  //  kprintDS("[KDEBUG]", "tss address %x", (uint64_t)tss);
    tss->rsp0 =  newKernelStack;
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
