#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/kHeap.h>
#include <kernel/acpi/madt.h>
#include <kernel/int/apic.h> 
#include <lib/asmUtils.h>
#include <lib/memUtils.h>
#include <lib/output.h>

madtInfo_t madtInfo;

uint32_t lapicRead(uint16_t offset) {
    return *(volatile uint32_t*)(madtInfo.lapicAddr + HIGH_VMA + offset);
}

void lapicWrite(uint16_t offset, uint32_t data) {
    *(volatile uint32_t*)(madtInfo.lapicAddr + HIGH_VMA + offset) = data;
}

void sendIPI(uint8_t ap, uint32_t ipi) {
    lapicWrite(LAPIC_ICRH, (ap << 24));
    lapicWrite(LAPIC_ICRL, ipi);
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

    wrmsr(MSR_APIC_BASE, (1 << 11)); // lapic enable
    lapicWrite(LAPIC_SINT, lapicRead(LAPIC_SINT) | 0x1ff); // enavle spurious interrupts

    kprintDS("[APIC]", "Detected cpu cores %d", madtInfo.madtEntry0Count);

    if(rdmsr(MSR_APIC_BASE) & (1 << 11)) {
        kprintDS("[APIC]", "lapic enabled"); 
    } else {
        kprintDS("[APIC]", "A full bruh momento just occoured, we could not make your lapic to work :("); 
    }

    asm volatile ( "mov %0, %%cr8\n" "sti" :: "r"((uint64_t)0)); // set the TPR and also sti
}
