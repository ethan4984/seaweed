#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/acpi/madt.h>
#include <kernel/int/apic.h>
#include <lib/memUtils.h>
#include <lib/asmUtils.h>
#include <lib/output.h>

madtInfo_t madtInfo;

void prepTrampoline(uint64_t stack, uint64_t pml4, uint64_t entryPoint) {
    uint64_t *arguments = (uint64_t*)(0x500 + HIGH_VMA);
    arguments[0] = stack;
    arguments[1] = pml4;
    arguments[2] = entryPoint;
}

void kernelMainSMP() {
    wrmsr(IA32_APIC_BASE, (1 << 11)); // lapic enable
    lapicWrite(0xf0, lapicRead(0xf0) | 0x1ff); // enavle spurious interrupts

    asm volatile ("sti");

    for(;;);
}

void initSMP() {
    madtInfo = grabMadt();

    memcpy64((uint64_t*)(0x1000 + HIGH_VMA), (uint64_t*)(0x7e00 + HIGH_VMA), 0x200 / 8);

    for(uint64_t i = 1; i < madtInfo.madtEntry0Count; i++) { 
        prepTrampoline(physicalPageAlloc(4) + 0x4000 + HIGH_VMA, grabPML4(), (uint64_t)&kernelMainSMP);
        sendIPI(i, 0x500);
        sendIPI(i, 0x600 | (0x1000 / 0x1000));
        kprintDS("[APIC]", "core %d initalized", i);
    }
}
