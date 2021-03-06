#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/sched/hpet.h>
#include <kernel/sched/smp.h>
#include <kernel/acpi/madt.h>
#include <kernel/mm/kHeap.h>
#include <kernel/int/apic.h>
#include <kernel/int/idt.h>
#include <kernel/int/gdt.h>
#include <kernel/int/tss.h>
#include <libk/memUtils.h>
#include <libk/asmUtils.h>
#include <libk/output.h>

madtInfo_t madtInfo;
idtr_t idtr;
gdtPtr_t gdtPtr;

cpuInfo_t *cpuInfo;
uint64_t cpuInfoIndex = 1, numberOfCores = 1;

void prepTrampoline(uint64_t stack, uint64_t pml4, uint64_t entryPoint, uint64_t idt) {
    uint64_t *arguments = (uint64_t*)(0x500 + HIGH_VMA);
    arguments[0] = stack;
    arguments[1] = pml4;
    arguments[2] = entryPoint;
    arguments[3] = idt;
    arguments[4] = numberOfCores++;
}

void kernelMainSMP() {
    lapicWrite(LAPIC_SINT, lapicRead(LAPIC_SINT) | 0x1ff); // enavle spurious interrupts
    
    cpuInfo[cpuInfoIndex].coreID = lapicRead(LAPIC_ID_REG);
    cpuInfo[cpuInfoIndex].currentTask = -1;

    kprintDS("[SMP]", "Core %d fully initalized", cpuInfoIndex++);
    kprintVS("Hi from core %d\n", cpuInfoIndex - 1);

    uint64_t stack = physicalPageAlloc(4) + 0x4000 + KERNEL_HIGH_VMA;
    asm volatile ("movq %0, %%rsp" : "=r"(stack));

    createGenericTSS(stack);
    createNewGDT(cpuInfoIndex - 1, grabTSS(cpuInfoIndex));

    lapicTimerInit(100);

    asm volatile ("sti");

    for(;;);
}

void initSMP() {
    madtInfo = grabMadt();

    cpuInfo = kmalloc(sizeof(cpuInfo_t) * 32); // if you have more than 32 cores youre out of luck

    cpuInfo[0].currentTask = -1;

    memcpy64((uint64_t*)(0x1000 + HIGH_VMA), (uint64_t*)(0x7e00 + HIGH_VMA), 0x200 / 8);

    asm volatile ("sidt %0" :: "m"(idtr));

    for(uint64_t i = 1; i < madtInfo.madtEntry0Count; i++) { 
        uint64_t coreID = madtInfo.madtEntry0[i].apicID;
        if(madtInfo.madtEntry0[i].flags == 1) {
            prepTrampoline(physicalPageAlloc(4) + 0x4000 + HIGH_VMA, grabPML4(), (uint64_t)&kernelMainSMP, (uint64_t)&idtr);
            sendIPI(coreID, 0x500); 
            sendIPI(coreID, 0x600 | (uint32_t)((uint64_t)0x1000 / PAGESIZE));
            ksleep(10); 
        }
    }
}

cpuInfo_t *grabCPUinfo() {
    return cpuInfo;
}
