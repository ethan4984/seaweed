#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/sched/hpet.h>
#include <kernel/acpi/madt.h>
#include <kernel/mm/kHeap.h>
#include <kernel/int/apic.h> 
#include <libk/asmUtils.h>
#include <libk/memUtils.h>
#include <libk/output.h>

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

uint32_t ioapicRead(uint64_t base, uint32_t reg) {
    *(volatile uint32_t*)(base + 16 + HIGH_VMA) = reg;
    return *(volatile uint32_t*)(base + 18 + HIGH_VMA);
}

void ioapicWrite(uint64_t base, uint32_t reg, uint32_t data) {
    *(volatile uint32_t*)(base + HIGH_VMA) = reg;
    *(volatile uint32_t*)(base + 16 + HIGH_VMA) = data;
}

uint32_t getMaxGSIs(uint64_t ioapic_base) {
    uint32_t data = ioapicRead(ioapic_base, 1) >> 16; // Read register 1
    return data & ~(1<<7);
}

uint64_t findVaildIOAPIC(uint64_t gsi) {
    uint64_t i;
    for(i = 0; i < madtInfo.madtEntry1Count; i++) {
        uint32_t maxGSIs = getMaxGSIs(madtInfo.madtEntry1[i].ioapicAddr) + madtInfo.madtEntry1[i].gsiBase;
        if(madtInfo.madtEntry1[i].gsiBase <= gsi && maxGSIs >= gsi)
            break;
        if(i == madtInfo.madtEntry1Count)
            return ERROR; // error code
    }
    return i;
}

uint64_t writeRedirectionTable(uint32_t gsi, uint64_t data) {
    uint64_t ioapicIndex = findVaildIOAPIC(gsi);

    if(ioapicIndex == ERROR) { // error code
        kprintDS("[APIC]", "Bad GSI");
        return 0;
    }

    uint32_t reg = ((gsi - madtInfo.madtEntry1[ioapicIndex].gsiBase) * 2) + 16;
    ioapicWrite(madtInfo.madtEntry1[ioapicIndex].ioapicAddr, reg, (uint32_t)data);
    ioapicWrite(madtInfo.madtEntry1[ioapicIndex].ioapicAddr, reg + 1, (uint32_t)(data >> 32));
    return 1;
}

uint64_t readRedirectionTable(uint32_t gsi) {
    uint64_t ioapicIndex = findVaildIOAPIC(gsi);

    if(ioapicIndex == ERROR) {
        kprintDS("[APIC]", "Bad GSI");
        return 69420;
    }

    uint32_t reg = ((gsi - madtInfo.madtEntry1[ioapicIndex].gsiBase) * 2) + 16;
    uint64_t data = (uint64_t)ioapicRead(madtInfo.madtEntry1[ioapicIndex].ioapicAddr, reg);
    return data | ((uint64_t)(ioapicRead(madtInfo.madtEntry1[ioapicIndex].ioapicAddr, reg + 1)) << 32);
}

void maskGSI(uint32_t gsi) {
    uint64_t redirectionTable = readRedirectionTable(gsi);
    if(redirectionTable == ERROR) {
        kprintDS("[APIC]", "Bad redirection table : unable to mask GSI %x", gsi);
        return;
    }
    writeRedirectionTable(gsi, redirectionTable | (1 << 16));
}

void lapicTimerInit(uint64_t ticksPerMS) { 
    ticksPerMS -= 10; // make up for ksleep 10

    lapicWrite(LAPIC_TIMER_DIVIDE_CONF, 0x3);
    ksleep(10);
    lapicWrite(LAPIC_TIMER_INITAL_COUNT, 0xffffffff);

    ksleep(ticksPerMS);

    uint32_t ticks = 0xffffffff - lapicRead(LAPIC_TIMER_CURRENT_COUNT);

    lapicWrite(LAPIC_TIMER_LVT, 32 | 0x20000);
    lapicWrite(LAPIC_TIMER_DIVIDE_CONF, 0x3);
    lapicWrite(LAPIC_TIMER_INITAL_COUNT, ticks);
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

    kprintVS("Detected cpu cores %d\n", madtInfo.madtEntry0Count);

    for(uint64_t i = 0; i < madtInfo.madtEntry1Count; i++) {
        for(uint64_t j = madtInfo.madtEntry1[i].gsiBase; j < getMaxGSIs(madtInfo.madtEntry1[i].ioapicAddr); j++)
            maskGSI(j);
    }

    for(uint64_t i = 0; i < 16; i++) {
        writeRedirectionTable(i, i + 32);
    }
    
    lapicWrite(LAPIC_SINT, lapicRead(LAPIC_SINT) | 0x1ff); // enavle spurious interrupts

    if(rdmsr(MSR_APIC_BASE) & (1 << 11)) {
        kprintVS("lapic enabled\n"); 
    } else {
        kprintVS("A full bruh momento just occoured, we could not make your lapic to work :(\n"); 
    }

    asm volatile("mov %0, %%cr8\n" :: "r"((uint64_t)0)); // set the TPR and also sti
}
