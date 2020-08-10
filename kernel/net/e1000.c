#include <kernel/drivers/pci.h>
#include <kernel/net/e1000.h>
#include <libk/output.h>

static pciInfo_t pciInfo;
static pciBar_t bar0;

void initEthernet() {
    pciInfo = grabPCIDevices(); 
    pci_t device;

    for(uint64_t i = 0; i < pciInfo.totalDevices; i++) {
        if((pciInfo.pciDevices[i].classCode == 2) && (pciInfo.pciDevices[i].subclass == 0)) {
            device = pciInfo.pciDevices[i];
            kprintDS("[NET]", "Ethernet adapater found from vendor %x and deviceID %x", device.vendorID, device.deviceID);
            kprintVS("Ethernet adapater found from vendor %x and deviceID %x\n", device.vendorID, device.deviceID);
            break;
        }

        if(i == pciInfo.totalDevices - 1) {
            kprintDS("[NET]", "No ethernet adapter can be found");
            return;
        }
    }
}
