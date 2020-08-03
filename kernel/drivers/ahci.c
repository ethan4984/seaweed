#include <kernel/mm/virtualPageManager.h>
#include <kernel/drivers/ahci.h>
#include <kernel/drivers/pci.h>
#include <libk/output.h>

pciInfo_t pciInfo;

static pciBar_t bar;

void initAHCI() {
    pciInfo = grabPCIDevices();
    pci_t device;

    for(uint64_t i = 0; i < pciInfo.totalDevices; i++) {
        if(pciInfo.pciDevices[i].classCode == 1) {
            switch(pciInfo.pciDevices[i].classCode) {
                case 0:
                    kprintDS("[AHCI]", "Detected a Vendor Sepcific Interface (get a new pc retard)");
                    return;
                case 1:
                    kprintDS("[AHCI]", "Detected AHCI 1.0");
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

    hbaPorts_t *hbaPorts = (hbaPorts_t*)((uint64_t)bar.base + HIGH_VMA);
}
