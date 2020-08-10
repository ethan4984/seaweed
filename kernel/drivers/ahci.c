#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/sched/scheduler.h>
#include <kernel/drivers/ahci.h>
#include <kernel/drivers/pci.h>
#include <kernel/mm/kHeap.h>
#include <kernel/fs/bfs.h>
#include <libk/asmUtils.h>
#include <libk/memUtils.h>
#include <libk/output.h>

#include <stdbool.h>

pciInfo_t pciInfo;

static pciBar_t bar;
drive_t *drive;
drives_t drives;

uint64_t driveCount = 0;

static uint32_t findCMD(volatile hbaPorts_t *hbaPort);
static void initSATAdevice(volatile hbaPorts_t *hbaPort);
static void sendCommand(volatile hbaPorts_t *hbaPort, uint32_t CMDslot);
static void addDrive(uint64_t sectorCount, volatile hbaPorts_t *hbaPort);

void initAHCI() {
    pciInfo = grabPCIDevices();
    pci_t device;

    for(uint64_t i = 0; i < pciInfo.totalDevices; i++) {
        if((pciInfo.pciDevices[i].classCode == 1) && (pciInfo.pciDevices[i].subclass == 6)) {
            kprintVS("%d\n", i);
            switch(pciInfo.pciDevices[i].progIF) {
                case 0:
                    kprintDS("[AHCI]", "Detected a Vendor Sepcific Interface (get a new pc retard)");
                    return;
                case 1:
                    kprintVS("bruh %d\n", i);
                    kprintDS("[AHCI]", "Detected an AHCI 1.0 device");
                    device = pciInfo.pciDevices[i];
    
                    if(!(pciRead(device.bus, device.device, device.function, 0x4) & (1 << 2))) {
                        pciWrite(pciRead(device.bus, device.device, device.function, 0x4) | (1 << 2), device.bus, device.device, device.function, 0x4);
                    }

                    break;
                case 2:
                    kprintDS("[AHCI]", "Detected a Serial Storage Bus");
                    return;
            }
        }
    }
    
    drive = kmalloc(sizeof(drives_t) * 32);

    bar = getBAR(device, 5);

    kprintVS("BAR5 base %x | size %x\n", bar.base, bar.size);
    
    volatile GHC_t *GHC = (volatile GHC_t*)((uint64_t)bar.base + HIGH_VMA - 0x1000);

    for(uint64_t i = 0; i < 32; i++) {
        if(GHC->pi & (1 << i)) {
            volatile hbaPorts_t *hbaPorts = &GHC->hbaPorts[i];

            switch(hbaPorts->sig) {
                case SATA_ATA:
                    //kprintDS("[AHCI]", "Sata drive found on port %d", i);
                    kprintVS("Sata drive found on port %d\n", i);
                    initSATAdevice(hbaPorts);
                    break;
                case SATA_ATAPI:
                    kprintVS("ATAPI drive found on port %d\n", i);
                    break;
                case SATA_SEMB:
                    kprintVS("Enclosure management bridge found on port %d\n", i);
                    break; 
                case SATA_PM:
                    kprintVS("Port multiplier found on port %d\n", i);
            }
        }
    }

    drives.drive = drive;
    drives.driveCnt = driveCount;
}

static void initSATAdevice(volatile hbaPorts_t *hbaPort) {
    static uint64_t driveCnt = 0;

    uint32_t CMDslot = findCMD(hbaPort);
    uint16_t *identify = (uint16_t*)physicalPageAlloc(1);
    volatile hbaCMDhdr_t *hbaCMDhdr = (volatile hbaCMDhdr_t*)((uint64_t)hbaPort->clb + HIGH_VMA);

    hbaCMDhdr += CMDslot;
    hbaCMDhdr->cfl = sizeof(volatile fisH2D_t) / sizeof(uint32_t);
    hbaCMDhdr->w = 0;
    hbaCMDhdr->prdtl = 1;

    volatile hbaCommandTable_t *cmdtbl = (volatile hbaCommandTable_t*)((uint64_t)hbaCMDhdr->ctba + HIGH_VMA);
    memset((void*)((uint64_t)hbaCMDhdr->ctba + HIGH_VMA), 0, sizeof(volatile hbaCommandTable_t));

    cmdtbl->PRDT[0].dba = (uint32_t)(uint64_t)identify;
    cmdtbl->PRDT[0].dbc = 511;
    cmdtbl->PRDT[0].i = 1;

    fisH2D_t *cmdfis = (fisH2D_t*)(((uint64_t)cmdtbl->cfis));
    memset((void*)(((uint64_t)cmdtbl->cfis)), 0, sizeof(fisH2D_t));

    cmdfis->command = 0xec;
    cmdfis->c = 1;
    cmdfis->device = 0;
    cmdfis->pmport = 0;
    cmdfis->fisType = FIS_REG_H2D;

    sendCommand(hbaPort, CMDslot);
    
    //kprintDS("[AHCI]", "Drive %d: Total sector count: %d", driveCnt++, *((uint64_t*)((uint64_t)&identify[100] + HIGH_VMA)));
    kprintVS("Drive %d: Total sector count: %d\n", driveCnt++, *((uint64_t*)((uint64_t)&identify[100] + HIGH_VMA)));

    addDrive(*((uint64_t*)((uint64_t)&identify[100] + HIGH_VMA)), hbaPort);
//    kprintVS("Drive %d: Total sector count: %d\n", driveCnt++, *((uint64_t*)((uint64_t)&identify[100] + HIGH_VMA)));
}

void sataRW(drive_t *drive, uint64_t start, uint32_t count, void *buffer, bool w) {
    static char lock = 0;
    spinLock(&lock);

    uint32_t CMDslot = findCMD(drive->hbaPort);

    volatile hbaCMDhdr_t *hbaCMDhdr = (volatile hbaCMDhdr_t*)((uint64_t)drive->hbaPort->clb + HIGH_VMA);
    memset((void*)((uint64_t)drive->hbaPort->clb + HIGH_VMA), 0, sizeof(volatile hbaCMDhdr_t));

    hbaCMDhdr += CMDslot;
    hbaCMDhdr->cfl = sizeof(volatile fisH2D_t) / sizeof(uint32_t);
    hbaCMDhdr->w = (w) ? 1 : 0;
    hbaCMDhdr->prdtl = 1;
    
    volatile hbaCommandTable_t *cmdtbl = (volatile hbaCommandTable_t*)((uint64_t)hbaCMDhdr->ctba + HIGH_VMA);
    memset((void*)((uint64_t)hbaCMDhdr->ctba + HIGH_VMA), 0, sizeof(volatile hbaCommandTable_t));

    cmdtbl->PRDT[0].dba = (uint32_t)((uint64_t)buffer - HIGH_VMA);
    cmdtbl->PRDT[0].dbc = (count * 512) - 1;
    cmdtbl->PRDT[0].i = 1;
    
    fisH2D_t *cmdfis = (fisH2D_t*)(((uint64_t)cmdtbl->cfis));
    memset((void*)(((uint64_t)cmdtbl->cfis)), 0, sizeof(fisH2D_t));
    
    cmdfis->fisType = FIS_REG_H2D;
    cmdfis->c = 1;
    cmdfis->command = (w) ? 0x35 : 0x25;
    
    cmdfis->lba0 = (uint8_t)((uint32_t)start & 0x000000000000ff);
    cmdfis->lba1 = (uint8_t)(((uint32_t)start & 0x0000000000ff00) >> 8);
    cmdfis->lba2 = (uint8_t)(((uint32_t)start & 0x00000000ff0000) >> 16);
    
    cmdfis->device = 1 << 6;
    
    cmdfis->lba3 = (uint8_t)(((uint32_t)start & 0x000000ff000000) >> 24);
    cmdfis->lba4 = (uint8_t)(((start >> 32) & 0x0000ff00000000));
    cmdfis->lba5 = (uint8_t)(((start >> 32) & 0x00ff0000000000) >> 8);
    
    cmdfis->countl = (uint8_t)count;
    cmdfis->counth = (uint8_t)(count >> 8);
    
    sendCommand(drive->hbaPort, CMDslot);
    
    spinRelease(&lock);
}

static void sendCommand(volatile hbaPorts_t *hbaPort, uint32_t CMDslot) {
    while((hbaPort->tfd & (0x80 | 0x8)));

    hbaPort->cmd &= ~HBA_CMD_ST;

    while(hbaPort->cmd & HBA_CMD_CR);

    hbaPort->cmd |= HBA_CMD_FRE;
    hbaPort->cmd |= HBA_CMD_ST;
    
    hbaPort->ci = 1 << CMDslot;
    
    while(1) {
        if(!(hbaPort->ci & (1 << CMDslot)))
            break;
    }
    
    hbaPort->cmd &= ~HBA_CMD_ST;
    while (hbaPort->cmd & HBA_CMD_CR);
    hbaPort->cmd &= ~HBA_CMD_FRE;
}

static uint32_t findCMD(volatile hbaPorts_t *hbaPort) {
    for(int i = 0; i < 32; i++) {
        if(((hbaPort->sact | hbaPort->ci) & (1 << i)) == 0)
            return i;
    }
    return 0;
}

static void addDrive(uint64_t sectorCount, volatile hbaPorts_t *hbaPort) {
    if(driveCount + 1 == 32) {
        kprintDS("[ACPI]", "Extended drive limit");
    }
    drive[driveCount++] = (drive_t) { sectorCount, hbaPort };
}

drives_t getDrives() {
    return drives;
}
