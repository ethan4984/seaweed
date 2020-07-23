#include <kernel/mm/virtualPageManager.h>
#include <kernel/acpi/madt.h>
#include <kernel/mm/kHeap.h>
#include <lib/asmUtils.h>
#include <lib/output.h>

madtInfo_t madtInfo = { 0 };

void madtInit() {
    madt_t *madt = (madt_t*)findSDT("APIC");

    madtInfo.lapicAddr = madt->lapicAddr;
    kprintDS("[APIC]", "lapic address %x", madtInfo.lapicAddr);

    madtInfo.madtEntry0 = kmalloc(sizeof(madtEntry0_t) * 50);
    madtInfo.madtEntry1 = kmalloc(sizeof(madtEntry1_t) * 50);
    madtInfo.madtEntry2 = kmalloc(sizeof(madtEntry2_t) * 50);
    madtInfo.madtEntry4 = kmalloc(sizeof(madtEntry4_t) * 50);
    madtInfo.madtEntry5 = kmalloc(sizeof(madtEntry5_t) * 50);

    for(uint64_t i = 0; i < madt->acpihdr.length - (sizeof(madt->acpihdr) + sizeof(madt->lapicAddr) + sizeof(madt->flags)); i++) {
        uint8_t entryType = madt->entries[i++];
        uint8_t entrySize = madt->entries[i++];

        switch(entryType) {
            case 0:
                madtInfo.madtEntry0[madtInfo.madtEntry0Count++] = ((madtEntry0_t*)&(madt->entries[i]))[0];
                break;
            case 1:
                madtInfo.madtEntry1[madtInfo.madtEntry1Count++] = ((madtEntry1_t*)&(madt->entries[i]))[0];
                break;
            case 2:
                madtInfo.madtEntry2[madtInfo.madtEntry2Count++] = ((madtEntry2_t*)&(madt->entries[i]))[0];
                break;
            case 4:
                madtInfo.madtEntry4[madtInfo.madtEntry4Count++] = ((madtEntry4_t*)&(madt->entries[i]))[0];
                break;
            case 5:
                madtInfo.madtEntry5[madtInfo.madtEntry5Count++] = ((madtEntry5_t*)&(madt->entries[i]))[0];
        }
        i += entrySize - 3;
    }

    for(uint64_t i = 0; i < madtInfo.madtEntry0Count; i++) {
        kprintDS("[APIC]", "Parsing madt0 entry %d", i);
        kprintDS("[APIC]", "Processor %ds lapic", i);
        kprintDS("[APIC]", "ACPI processor ID: %x", madtInfo.madtEntry0[i].acpiProcessorID);
        kprintDS("[APIC]", "APIC ID: %x", madtInfo.madtEntry0[i].apicID);
        kprintDS("[APIC]", "Flags: %x", madtInfo.madtEntry0[i].flags);
    }

    for(uint64_t i = 0; i < madtInfo.madtEntry1Count; i++) {
        kprintDS("[APIC]", "Parsing madt1 entry %d", i);
        kprintDS("[APIC]", "I/O APIC ID: %x", madtInfo.madtEntry1[i].ioapicID);
        kprintDS("[APIC]", "I/O APIC address: %x", madtInfo.madtEntry1[i].ioapicAddr);
        kprintDS("[APIC]", "GSI base %x", madtInfo.madtEntry1[i].gsiBase);
    }

    for(uint64_t i = 0; i < madtInfo.madtEntry2Count; i++) {
        kprintDS("[APIC]", "Parsing madt2 entry %d", i);
        kprintDS("[APIC]", "bus source: %x", madtInfo.madtEntry2[i].busSrc);
        kprintDS("[APIC]", "irq source: %x", madtInfo.madtEntry2[i].irqSrc);
        kprintDS("[APIC]", "gsi %x", madtInfo.madtEntry2[i].gsi);
        kprintDS("[APIC]", "flags %x", madtInfo.madtEntry2[i].flags);
    }

    for(uint64_t i = 0; i < madtInfo.madtEntry4Count; i++) {
        kprintDS("[APIC]", "Parsing madt4 entry %d", i);
        kprintDS("[APIC]", "ACPI processor ID %x", madtInfo.madtEntry4[i].acpiProcessorID);
        kprintDS("[APIC]", "flags %x", madtInfo.madtEntry4[i].flags);
        kprintDS("[APIC]", "lint %x", madtInfo.madtEntry4[i].lint);
    }

    for(uint64_t i = 0; i < madtInfo.madtEntry5Count; i++) {
        kprintDS("[APIC]", "Parsing madt5 entry %d", i);
        kprintDS("[APIC]", "lapic override %x", madtInfo.madtEntry5[i].lapicOverride);
    }

    kprintDS("[APIC]", "System core count: %d", madtInfo.madtEntry0Count);
}

madtInfo_t grabMadt() {
    return madtInfo;
}
