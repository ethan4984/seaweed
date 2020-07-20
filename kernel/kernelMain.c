#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/physicalPageManager.h>
#include <kernel/bproto.h>
#include <lib/output.h>
#include <lib/memUtils.h>

#include <stdint.h>

extern symbol bssBegin;
extern symbol bssEnd;

__attribute__((section(".init")))
void bootMain(bproto_t *bproto) {
    memset(bssBegin, 0, bssEnd - bssBegin); // zero out .bss

    kprintDS("mmap address: %x", bproto->mmapAddress);
    kprintDS("e820 entries: %d", bproto->mmapEntries); 
    kprintDS("rsdp address: %x", bproto->rsdp);
    kprintDS("screen pitch: %d", bproto->pitch);
    kprintDS("screen width: %d", bproto->width);
    kprintDS("screen height: %d", bproto->height);
    kprintDS("screen bpp: %d", bproto->bpp);
    kprintDS("framebuffer: %x", bproto->framebuffer);

    mmapEntry_t *mmapEntries = (mmapEntry_t*)bproto->mmapAddress;
    for(uint8_t i = 0; i < bproto->mmapEntries; i++) {
        kprintDS("[%x -> %x] : length %x type %x", mmapEntries[i].base, mmapEntries[i].base + mmapEntries[i].length, mmapEntries[i].length, mmapEntries[i].type);
    }

    initPMM(bproto);
    initVMM();

    for(;;);
}
