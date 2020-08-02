#ifndef PCI_H
#define PCI_H

#include <stdint.h>

typedef struct {
    uint8_t classCode;
    uint8_t subclass;
    uint8_t progIF;
    uint16_t deviceID;
    uint16_t vendorID;
    uint8_t bus;
    uint8_t device;
    uint8_t function;
} pci_t;

uint32_t pciRead(uint8_t bus, uint8_t device, uint8_t func, uint8_t reg);

void pciWrite(uint32_t data, uint8_t bus, uint8_t device, uint8_t func, uint8_t reg);

void pciScanBus(uint8_t bus);

void checkDevice(uint8_t bus, uint8_t device, uint8_t function);

void initPCI();

#endif
