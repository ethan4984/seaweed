#ifndef MADT_H
#define MADT_H

#include <kernel/acpi/rsdp.h>

typedef struct {
    acpihdr_t acpihdr;
    uint32_t lapicAddr;
    uint32_t flags;
    uint8_t entries[];
} __attribute__((packed)) madt_t;

typedef struct {
    uint8_t acpiProcessorID;
    uint8_t apicID;
    uint32_t flags;
} __attribute__((packed)) madtEntry0_t;

typedef struct {
    uint8_t ioapicID;
    uint8_t reserved;
    uint32_t ioapicAddr;
    uint32_t gsiBase;
} __attribute__((packed)) madtEntry1_t;

typedef struct {
    uint8_t busSrc;
    uint8_t irqSrc;
    uint32_t gsi;
    uint16_t flags;
} __attribute__((packed)) madtEntry2_t;

typedef struct {
    uint8_t acpiProcessorID;
    uint16_t flags; 
    uint8_t lint;
} __attribute__((packed)) madtEntry4_t;

typedef struct {
    uint16_t reserved;
    uint8_t lapicOverride;
} __attribute__((packed)) madtEntry5_t;

typedef struct {
    uint8_t madtEntry0Count;
    uint8_t madtEntry1Count;
    uint8_t madtEntry2Count;
    uint8_t madtEntry4Count;
    uint8_t madtEntry5Count;

    madtEntry0_t *madtEntry0;
    madtEntry1_t *madtEntry1;
    madtEntry2_t *madtEntry2;
    madtEntry4_t *madtEntry4;
    madtEntry5_t *madtEntry5;

    uint32_t lapicAddr;
} __attribute__((packed)) madtInfo_t;

void madtInit();

madtInfo_t grabMadt();

#endif
