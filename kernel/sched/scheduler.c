#include <kernel/mm/virtualPageManager.h>
#include <kernel/sched/scheduler.h>
#include <kernel/sched/smp.h>
#include <kernel/mm/kHeap.h>
#include <kernel/int/apic.h>
#include <lib/asmUtils.h>
#include <lib/memUtils.h>
#include <lib/output.h>

static uint64_t findCore();
static void scheduleTask(task_t task, uint64_t core, uint8_t status);

cpuInfo_t *cpuInfo;
task_t *tasks;

uint64_t numberOfTasks = 0;

void schedulerMain(regs_t *regs) {
    asm volatile ("cli");

    kprintDS("[SMP]", "Hello");

    for(uint64_t i = 0; i < numberOfTasks; i++) { /* start tasks for the first time that havent */
        if(tasks[i].status == WAITING_TO_START) {
            scheduleTask(tasks[i], findCore(), START_TASK); 
        }
    }

    asm volatile ("sti");
}

static void scheduleTask(task_t task, uint64_t core, uint8_t status) {
    uint64_t *parameters = (uint64_t*)0x500;
    parameters[0] = status;
    parameters[1] = task.rsp;
    parameters[2] = task.rbp;
    parameters[3] = task.entryPoint;

    sendIPI(core, 69);
}

static uint64_t findCore() {
    uint64_t core = cpuInfo[0].numberOfTasks, i;
    for(i = 0; i < 32; i++) {
        if(cpuInfo[i].coreID == 0) {
            core = cpuInfo[i].coreID;
            break;
        }
        if(cpuInfo[i].numberOfTasks < core)
            core = cpuInfo[i].numberOfTasks;
    }
    return i;
}

void rescheduleCore(regs_t *regs) {
    uint64_t *parameters = (uint64_t*)0x500;  

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
    lapicTimerInit(100);
}
