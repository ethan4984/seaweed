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

uint8_t scheduledCores[32] = { 2 };

cpuInfo_t *cpuInfo;
task_t *tasks;

uint64_t numberOfTasks = 0, maxNumberOfTasks = 10;

void schedulerMain(regs_t *regs) {
    asm volatile ("cli");

    static uint64_t lock = 0;
    spinLock((uint64_t)&lock);

    kprintDS("[SMP]", "Hi from core %d", regs->core);

    uint64_t oldTask = cpuInfo[regs->core].currentTask;
    uint64_t nextTaskIndex = oldTask;

    kprintDS("[SMP]", "oldTask %x", oldTask);
    if(oldTask != 0x420) {
        kprintDS("[SMP]", "Here");
        tasks[oldTask].rsp = (uint64_t)regs;
        tasks[oldTask].rbp = (uint64_t)regs;
        tasks[oldTask].status = WAITING;
    }

    if(oldTask == 0x420)
        nextTaskIndex = 0;

    cpuInfo[regs->core].currentTask = nextTaskIndex;

    bool flow = true;

    if(oldTask + 1 == numberOfTasks) {
        flow = false;
    } 

    if(oldTask - 1 < 0) {
        flow = true;
    }

    if(flow) {
        nextTaskIndex++;
    } else {
        nextTaskIndex--;
    }

    kprintDS("[SMP]", "Next task index %d", nextTaskIndex);

    task_t *nextTask = &tasks[nextTaskIndex];

    if(nextTask->status == WAITING_TO_START) {
        nextTask->status == RUNNING;
        lock = 0;
        kprintDS("[SMP]", "bruh");
        asm volatile ("sti");
        startTask(nextTask->rsp, nextTask->entryPoint);
    }

    lock = 0;
    asm volatile ("sti");
    kprintDS("[SMP]", "bruhthh");
    switchTask(nextTask->rsp, nextTask->rsp); 
}

static uint64_t findNextTaskToRun(uint64_t currentTask) {
    static bool flow = true;
    if(flow + 1 == numberOfTasks) {
        flow = false; 
    } else if(flow - 1 < 0) {
        flow = true;
    } 

    if(flow) {
        return ++currentTask;
    } else {
        return --currentTask;
    }
}

/* choose whatever core and start that task there */

static void startTaskSMP(task_t task, uint64_t core) {
    static uint64_t lock = 0;
    spinLock((uint64_t)&lock);

    setParameters(WAITING_TO_START, task.rsp, task.rbp, task.entryPoint);
    
    sendIPI(core, 69);
    lock = 0;
}

static void switchTaskSMP(task_t task, uint64_t core, uint64_t core1) {
    static uint64_t lock = 0;
    spinLock((uint64_t)&lock);

    uint64_t core1Stack = grabCoresStack(core1);
    uint64_t core2Stack = grabCoresStack(core);

    kprintDS("[SMP]", "Switching core %d with core %d with stacks of %x and %x", core, core1, core1Stack, core2Stack);

    setParameters(SWITCH_TASK, core1Stack, core1Stack, 0);
    sendIPI(core, 69);
    ksleep(1);

    setParameters(SWITCH_TASK, core2Stack, core2Stack, 0);
    sendIPI(core1, 69);
    ksleep(1);

    lock = 0;
}

static uint64_t grabCoresStack(uint64_t core) {
    *(uint64_t*)0x500 = GIVE_ME_YOUR_STACK_AND_WAIT;
    sendIPI(core, 69);
    ksleep(1);
    return *(uint64_t*)0x550;
}

static uint64_t setParameters(uint8_t status, uint64_t rsp, uint64_t rbp, uint64_t entryPoint) {
    uint64_t *parameters = (uint64_t*)0x500;
    parameters[0] = status;
    parameters[1] = rsp;
    parameters[2] = rbp;
    parameters[3] = entryPoint;
}

/* find the core with the least ammount of tasks on it */

static int8_t findCore() {
    for(uint8_t i = 0; i < 32; i++) {
        if(scheduledCores[i] == 0) {
            return i;
        }
    }
    return -1;
}

/* an isr hanlder designed to be exeuted upon IPI */

void rescheduleCore(regs_t *regs) {
    uint64_t *parameters = (uint64_t*)0x500;  

    kprintDS("[SMP]", "Hi from core %x", lapicRead(LAPIC_ID_REG));

    uint64_t type = parameters[0];
    
    switch(type) {
        case START_TASK:
            goto startTask;
        case SWITCH_TASK:
            goto switchTask;
        case GIVE_ME_YOUR_STACK_AND_WAIT:
            goto handItOver;
    }

    startTask: { 
        uint64_t rsp = parameters[1];
        uint64_t entryPoint = parameters[3]; 
        startTask(rsp, entryPoint);
    }

    switchTask: {
        uint64_t rsp = parameters[1];
        uint64_t rbp = parameters[2];
        switchTask(rsp, rbp);
    }

    handItOver: { // gives the stack
        *(uint64_t*)0x550 = (uint64_t)regs; 
        for(;;); // waiting
    }
}

void schedulerInit() {
    cpuInfo = grabCPUinfo();

    scheduledCores[0] = 2;
    for(uint64_t i = 1; i < 32; i++) {
        if(cpuInfo[i].coreID != 0) {
            scheduledCores[i] = 0;
        } else {
            break;
        }
    }

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
    numberOfTasks++; 
}

static int64_t findFreeIndex() {
    for(uint64_t i = 0; i < maxNumberOfTasks; i++) {
        if(tasks[i].rsp == 0) 
            return i;
    }
    return -1;
}
