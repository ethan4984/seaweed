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

static int8_t findCore();
static void startTaskSMP(task_t task, uint64_t core);
static int64_t findFreeIndex();
static void switchTaskSMP(task_t task, uint64_t core, uint64_t core1);
static uint64_t grabCoresStack(uint64_t core);
static uint64_t setParameters(uint8_t status, uint64_t rsp, uint64_t rbp, uint64_t entryPoint);
static uint64_t findNextTaskToRun();

cpuInfo_t *cpuInfo;
task_t *tasks;

uint64_t numberOfTasks = 0, maxNumberOfTasks = 10;

void schedulerMain(regs_t *regs) {
    asm volatile ("cli");

    static uint64_t lock = 0;
    spinLock((uint64_t)&lock);

    if(numberOfTasks == 0) {
        lock = 0;
        return;
    }

    //kprintDS("[SMP]", "Hi from core %d", regs->core);
    for(uint64_t i = 0; i < numberOfTasks; i++) {
        kprintDS("[SMP]", "task %d with stack of %x and status of %d", i, tasks[i].rsp, tasks[i].status);
    }

    int64_t oldTask = cpuInfo[regs->core].currentTask;
    uint64_t nextTaskIndex = 0;
    bool flow = true;
    
    for(uint64_t i = 0; i < numberOfTasks; i++) {     
        if(nextTaskIndex + 1 > numberOfTasks) {
            flow = false;
        } 

        if(nextTaskIndex - 1 <= 0) {
            flow = true;
        }

        if(flow) {
            nextTaskIndex++;
        } else {
            nextTaskIndex--;
        }

        //kprintDS("[SMP]", "%d", nextTaskIndex);

        if(tasks[nextTaskIndex - 1].status == WAITING || tasks[nextTaskIndex - 1].status == WAITING_TO_START)
            break;

        if(i + 1 == numberOfTasks) {
            //kprintDS("[SMP]", "Cant find any tasks to schedule");
            lock = 0;
            asm volatile ("sti");
            return;
        }
    }

    nextTaskIndex--;

    if(tasks[oldTask].status == RUNNING) {
        kprintDS("[SMP]", "Saving tasks state %d", oldTask);
        tasks[oldTask].rsp = (uint64_t)regs;
        tasks[oldTask].rbp = (uint64_t)regs;
        tasks[oldTask].status = WAITING;
    }

    cpuInfo[regs->core].currentTask = nextTaskIndex;

    kprintDS("[SMP]", "Next task index %d", nextTaskIndex);

    if(tasks[nextTaskIndex].status == WAITING_TO_START) {
        tasks[nextTaskIndex].status = RUNNING; 
        kprintDS("[SMP]", "starting task %d with stack %x on core %d", nextTaskIndex, tasks[nextTaskIndex].rsp, regs->core);
        lock = 0;
        startTask(tasks[nextTaskIndex].rsp, tasks[nextTaskIndex].entryPoint);
    }

    kprintDS("[SMP]", "switching to task index %d with stack %x on core %d", nextTaskIndex, tasks[nextTaskIndex].rsp, regs->core);
    tasks[nextTaskIndex].status = RUNNING;
    lock = 0;
    switchTask(tasks[nextTaskIndex].rsp, tasks[nextTaskIndex].rbp);
}

void schedulerInit() {
    cpuInfo = grabCPUinfo();
    tasks = kmalloc(sizeof(task_t) * 10);
}

void createNewTask(uint64_t rsp, uint64_t entryPoint) {
    uint64_t currentIndex = findFreeIndex();

    if(currentIndex == -1) {
        tasks = krealloc(tasks, sizeof(task_t) * 10);
        maxNumberOfTasks += 10;
    }

    task_t task = { WAITING_TO_START, 0, rsp, rsp,  entryPoint };
    tasks[currentIndex] = task;
    kprintDS("[SMP]", "Increasing number of tasks to %d", numberOfTasks + 1);
    numberOfTasks++; 
}

static int64_t findFreeIndex() {
    for(uint64_t i = 0; i < maxNumberOfTasks; i++) {
        if(tasks[i].rsp == 0) 
            return i;
    }
    return -1;
}
