#ifndef AHCI_H
#define AHCI_H 

#define FIS_REG_H2D 0x27
#define FIS_REG_D2H 0x34
#define FIS_DMA_ENABLE 0x39
#define FIS_DMA_SETUP 0x41
#define FIS_DATA 0x46
#define FIS_BIST 0x58
#define FIS_PIO_SETUP 0x5f
#define FIS_DEVICE_BITS 0xa1

void initAHCI();

#endif
