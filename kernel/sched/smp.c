#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/sched/hpet.h>
#include <kernel/sched/smp.h>
#include <kernel/acpi/madt.h>
#include <kernel/mm/kHeap.h>
#include <kernel/int/apic.h>
#include <kernel/int/idt.h>
#include <lib/memUtils.h>
#include <lib/asmUtils.h>
#include <lib/output.h>

madtInfo_t madtInfo;
idtr_t idtr;

cpuInfo_t *cpuInfo;
uint64_t cpuInfoIndex, numberOfCores = 1;

void prepTrampoline(uint64_t stack, uint64_t pml4, uint64_t entryPoint, uint64_t idt) {
    uint64_t *arguments = (uint64_t*)(0x500 + HIGH_VMA);
    arguments[0] = stack;
    arguments[1] = pml4;
    arguments[2] = entryPoint;
    arguments[3] = idt;
    arguments[4] = numberOfCores++;
}

void kernelMainSMP() {
    wrmsr(MSR_APIC_BASE, (1 << 11)); // lapic enable
    lapicWrite(LAPIC_SINT, lapicRead(LAPIC_SINT) | 0x1ff); // enavle spurious interrupts

    asm volatile ("sti");

    cpuInfo[cpuInfoIndex++].coreID = lapicRead(LAPIC_ID_REG);

    for(;;);
}

void initSMP() {
    madtInfo = grabMadt();

    cpuInfo = kmalloc(sizeof(cpuInfo_t) * 32); // if you have more than 32 cores youre out of luck

    memcpy64((uint64_t*)(0x1000 + HIGH_VMA), (uint64_t*)(0x7e00 + HIGH_VMA), 0x200 / 8);

    asm volatile ("sidt %0" :: "m"(idtr));

    for(uint64_t i = 1; i < madtInfo.madtEntry0Count; i++) { 
        prepTrampoline(physicalPageAlloc(4) + 0x4000 + HIGH_VMA, grabPML4(), (uint64_t)&kernelMainSMP, (uint64_t)&idtr);
        sendIPI(i, 0x500);
        sendIPI(i, 0x600 | (0x1000 / PAGESIZE));
        kprintDS("[SMP]", "core %d initalized", i);
        ksleep(10); 
    }
}

cpuInfo_t *grabCPUinfo() {
    return cpuInfo;
}
