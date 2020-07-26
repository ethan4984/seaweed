#include <kernel/mm/virtualPageManager.h>
#include <kernel/sched/scheduler.h>
#include <kernel/sched/smp.h>
#include <kernel/sched/hpet.h>
#include <kernel/mm/kHeap.h>
#include <kernel/int/apic.h>
#include <lib/asmUtils.h>
#include <lib/memUtils.h>
#include <lib/output.h>

static uint64_t findCore();
static void startTaskSMP(task_t task, uint64_t core, uint8_t status, regs_t *regs);
static int64_t findFreeIndex();

cpuInfo_t *cpuInfo;
task_t *tasks;

uint64_t numberOfTasks = 0, maxNumberOfTasks = 10;

void schedulerMain(regs_t *regs) {
    asm volatile ("cli");

    static bool bruh = false;
    if(!bruh) {
        kprintDS("[SMP]", "Starting a task with rsp %x", tasks[0].rsp);
        kprintDS("[SMP]", "Starting a task with rsp %x", tasks[1].rsp);
        startTaskSMP(tasks[0], 1, START_TASK, regs);
        ksleep(10);
        startTaskSMP(tasks[1], 2, START_TASK, regs);
        ksleep(10);
        startTaskSMP(tasks[2], 3, START_TASK, regs);
        bruh = true;
    } 

    asm volatile ("sti");
}

/* choose whatever core and start that task there */

static void startTaskSMP(task_t task, uint64_t core, uint8_t status, regs_t *regs) {
    static uint64_t lock = 0;
    spinLock((uint64_t)&lock);

    uint64_t *parameters = (uint64_t*)0x500;
    parameters[0] = status;
    parameters[1] = task.rsp;
    parameters[2] = task.rbp;
    parameters[3] = task.entryPoint;

    kprintDS("[SMP]", "here");
    sendIPI(core, 69);
    lock = 0;
}

void switchTaskSMP(task_t task, uint64_t core, uint8_t status, regs_t *regs) {
    static uint64_t lock = 0;
    spinLock((uint64_t)&lock);

    uint64_t *parameters = (uint64_t*)0x500;
    parameters[0] = status;
   
    lock = 0;
}

/* find the core with the least ammount of tasks on it */

static uint64_t findCore() {
    uint64_t core = cpuInfo[0].numberOfTasks, override = 0;
    for(uint64_t i = 0; i < 32; i++) {
        if(cpuInfo[i].numberOfTasks < core) {
            override = i;
            core = cpuInfo[i].numberOfTasks;
        }

        if(cpuInfo[i + 1].coreID == 0) {
            core = cpuInfo[i].coreID;
            break;
        }
    }
    
    return override;
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

    kprintDS("[SMP]", "Current index %d", currentIndex);

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
