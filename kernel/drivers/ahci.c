#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/drivers/ahci.h>
#include <kernel/drivers/pci.h>
#include <libk/asmUtils.h>
#include <libk/memUtils.h>
#include <libk/output.h>

pciInfo_t pciInfo;

static pciBar_t bar;

static void startCMD(volatile hbaPorts_t *hbaPort);
static uint32_t findCMD(volatile hbaPorts_t *hbaPort);
static void stopCMD(volatile hbaPorts_t *hbaPort);
static void initSATAdevice(volatile hbaPorts_t *hbaPort);

void initAHCI() {
    pciInfo = grabPCIDevices();
    pci_t device;

    uint8_t found = 0;

    for(uint64_t i = 0; i < pciInfo.totalDevices; i++) {
        if((pciInfo.pciDevices[i].classCode == 1) && (pciInfo.pciDevices[i].subclass == 6)) {
            switch(pciInfo.pciDevices[i].progIF) {
                case 0:
                    kprintDS("[AHCI]", "Detected a Vendor Sepcific Interface (get a new pc retard)");
                    return;
                case 1:
                    kprintDS("[AHCI]", "Detected an AHCI 1.0 device");
                    device = pciInfo.pciDevices[i];
                    break;
                case 2:
                    kprintDS("[AHCI]", "Detected a Serial Storage Bus");
                    return;
            }
        }
    }

    bar = getBAR(device, 5);

    kprintDS("[AHCI]", "BAR5 base %x | size %x", bar.base, bar.size);
    
    volatile GHC_t *GHC = (volatile GHC_t*)((uint64_t)bar.base + HIGH_VMA - bar.size);

    for(uint64_t i = 0; i < 32; i++) {
        if(GHC->pi & (1 << i)) {
            volatile hbaPorts_t *hbaPorts = &GHC->hbaPorts[i];

            if((((hbaPorts->ssts >> 8) & 0xf) != 1) || ((hbaPorts->ssts & 0xf) != 3))
                continue;

            switch(hbaPorts->sig) {
                case SATA_ATA:
                    kprintDS("[AHCI]", "Sata drive found on port %d", i);
                    initSATAdevice(hbaPorts);
                    break;
                case SATA_ATAPI:
                    kprintDS("[AHCI]", "ATAPI drive found on port %d", i);
                    break;
                case SATA_SEMB:
                    kprintDS("[AHCI]", "Enclosure management bridge found on port %d", i);
                    break; 
                case SATA_PM:
                    kprintDS("[AHCI]", "Port multiplier found on port %d", i);
            }
        }
    }
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

    while((hbaPort->tfd & (0x80 | 0x8)));

    startCMD(hbaPort);
    hbaPort->ci = 1 << CMDslot;

    while(1) {
        if(!(hbaPort->ci & (1 << CMDslot)))
            break;
    }

    stopCMD(hbaPort);
    
    kprintDS("[AHCI]", "Drive %d: Total sector count: %d", driveCnt++, *((uint64_t*)((uint64_t)&identify[100] + HIGH_VMA)));
} 

static void startCMD(volatile hbaPorts_t *hbaPort) {
    hbaPort->cmd &= ~HBA_CMD_ST;

    while(hbaPort->cmd & HBA_CMD_CR);

    hbaPort->cmd |= HBA_CMD_FRE;
    hbaPort->cmd |= HBA_CMD_ST;
}

static uint32_t findCMD(volatile hbaPorts_t *hbaPort) {
    for(int i = 0; i < 32; i++) {
        if(((hbaPort->sact | hbaPort->ci) & (1 << i)) == 0)
            return i;
    }
}

static void stopCMD(volatile hbaPorts_t *hbaPort) {
    hbaPort->cmd &= ~HBA_CMD_ST;
    while (hbaPort->cmd & HBA_CMD_CR);
    hbaPort->cmd &= ~HBA_CMD_FRE;
}
