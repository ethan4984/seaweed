#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/kHeap.h>
#include <kernel/acpi/madt.h>
#include <kernel/int/apic.h> 
#include <lib/asmUtils.h>
#include <lib/output.h>

madtInfo_t *madtInfo;

uint32_t lapicRead(uint16_t offset) {
    return *(volatile uint32_t*)(madtInfo->lapicAddr + HIGH_VMA + offset);
}

void lapicWrite(uint16_t offset, uint32_t data) {
    *(volatile uint32_t*)(madtInfo->lapicAddr + HIGH_VMA + offset) = data;
}

uint32_t ioapicRead(uint64_t ioapicBase, uint32_t reg) {
    *(volatile uint32_t *)((uint64_t)ioapicBase + HIGH_VMA) = reg;
    return *(volatile uint32_t *)((uint64_t)ioapicBase + 16 + HIGH_VMA);    
}

void ioapicWrite(uint64_t ioapicBase, uint32_t reg, uint32_t data) {
    *(volatile uint32_t*)((uint64_t)ioapicBase + HIGH_VMA) = reg;
    *(volatile uint32_t*)((uint64_t)ioapicBase + 16 + HIGH_VMA) = data;
}

void initAPIC() {
    /* remap pic */
    outb(0x20, 0x11);
    outb(0xa0, 0x11);
    outb(0x21, 0x20);
    outb(0xa1, 0x28);
    outb(0x21, 0x4);
    outb(0xa1, 0x2);
    outb(0x21, 0x1);
    outb(0xa1, 0x1);
    outb(0x21, 0x0);
    outb(0xa1, 0x0);
    /* disable the pic */
    outb(0xa1, 0xff);
    outb(0x21, 0xff);

    madtInfo = grabMadt();

    wrmsr(IA32_APIC_BASE, (1 << 11)); // lapic enable
    lapicWrite(0xf0, lapicRead(0xf0) | 0x1ff); // enavle spurious interrupts

    if(rdmsr(IA32_APIC_BASE) & (1 << 11)) {
        kprintDS("[APIC]", "lapic enabled"); 
    } else {
        kprintDS("[APIC]", "A full bruh momento just occoured, we would not your lapic to work :("); 
    }
}
