#include <kernel/mm/virtualPageManager.h>
#include <kernel/drivers/pci.h>
#include <kernel/net/e1000.h>
#include <kernel/mm/kHeap.h>
#include <libk/output.h>
#include <stdbool.h>

static pciInfo_t pciInfo;
static pciBar_t bar0;

NICinfo_t *NICinfo;

uint32_t eepromRead(uint8_t addr) {
    uint16_t data = 0;
    uint32_t tmp = 0;

    *(uint32_t*)(NICinfo->regBase + 0x14) = (1) | ((uint32_t)(addr) << 8);
    while((!((tmp = *(uint32_t*)(NICinfo->regBase + 0x14)))) & (1 << 4));

    data = (uint16_t)((tmp >> 16) & 0xffff);
    return data;
}

void initEthernet() {
    pciInfo = grabPCIDevices(); 
    pci_t device;

    NICinfo = kmalloc(sizeof(NICinfo_t));

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

    bar0 = getBAR(device, 0);

    kprintDS("[NET]", "BAR0 base %x | size %x", bar0.base, bar0.size);

    NICinfo->regBase = bar0.base + HIGH_VMA;

    uint32_t cnt = 0, tmp;
    for(uint32_t i = 0; i < 3; i++) {
        tmp = eepromRead(i);
        NICinfo->mac[cnt++] = (uint8_t)tmp;
        NICinfo->mac[cnt++] = (uint8_t)(tmp >> 8);
    }

    kprintDS("[NET]", "MAC address: %x:%x:%x:%x:%x:%x", NICinfo->mac[0],
                                                        NICinfo->mac[1],
                                                        NICinfo->mac[2],
                                                        NICinfo->mac[3],
                                                        NICinfo->mac[4],
                                                        NICinfo->mac[5]);
}
