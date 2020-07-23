#ifndef RSDT_H
#define RSDT_H

#include <stdint.h>

typedef struct {
    char signature[4];
    uint32_t length;
    uint8_t revision;
    uint8_t checksum;
    char OEMID[6];
    char OEMtableid[8];
    uint32_t OEMRevision;
    uint32_t creatorID;
    uint32_t creatorRevision;
} __attribute__((packed)) acpihdr_t;

typedef struct {
    char signature[8];
    uint8_t checksum;
    char OEMID[6];
    uint8_t revision; 
    uint32_t rsdtAddr;
    uint32_t length;
    uint64_t xsdtAddr;
    uint8_t extChecksum;
    uint8_t reserved[3];
} __attribute__((packed)) rsdp_t;

typedef struct {
    acpihdr_t acpihdr;
    uint32_t acpiptr[];
} __attribute__((packed)) rsdt_t;

typedef struct {
    acpihdr_t acpihdr;
    uint64_t acpiptr[];
} __attribute__((packed)) xsdt_t;

void rsdpInit(uint64_t *rsdpAddr);

void *findSDT(char *signature);

#endif
