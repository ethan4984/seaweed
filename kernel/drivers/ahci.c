#include <kernel/drivers/ahci.h>
#include <kernel/drivers/pci.h>
#include <libk/output.h>

pciInfo_t pciInfo;

void initAHCI() {
    pciInfo = grabPCIDevices();

    for(uint64_t i = 0; i < pciInfo.totalDevices; i++) {
        if(pciInfo.pciDevices[i].classCode == 1) {
            switch(pciInfo.pciDevices[i].classCode) {
                case 0:
                    kprintDS("[AHCI]", "Detected a Vendor Sepcific Interface (get a new pc retard)");
                    break;
                case 1:
                    kprintDS("[AHCI]", "Detected AHCI 1.0");
                    break;
                case 2:
                    kprintDS("[AHCI]", "Detected a Serial Storage Bus");
            }
        }
    }
}
