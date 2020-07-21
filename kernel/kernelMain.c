#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/kHeap.h>
#include <kernel/bproto.h>
#include <lib/output.h>
#include <lib/memUtils.h>

#include <stdint.h>
#include <stddef.h>

extern symbol bssBegin;
extern symbol bssEnd;

__attribute__((section(".init")))
void bootMain(bproto_t *bproto) {
    memset(bssBegin, 0, bssEnd - bssBegin); // zero out .bss

    kprintDS("[KDEBUG]", "mmap address: %x", bproto->mmapAddress);
    kprintDS("[KDEBUG]", "e820 entries: %d", bproto->mmapEntries); 
    kprintDS("[KDEBUG]", "rsdp address: %x", bproto->rsdp);
    kprintDS("[KDEBUG]", "screen pitch: %d", bproto->pitch);
    kprintDS("[KDEBUG]", "screen width: %d", bproto->width);
    kprintDS("[KDEBUG]", "screen height: %d", bproto->height);
    kprintDS("[KDEBUG]", "screen bpp: %d", bproto->bpp);
    kprintDS("[KDEBUG]", "framebuffer: %x", bproto->framebuffer);

    mmapEntry_t *mmapEntries = (mmapEntry_t*)bproto->mmapAddress;
    for(uint8_t i = 0; i < bproto->mmapEntries; i++) {
        kprintDS("[KDEBUG]", "[%x -> %x] : length %x type %x", mmapEntries[i].base, mmapEntries[i].base + mmapEntries[i].length, mmapEntries[i].length, mmapEntries[i].type);
    }

    initPMM(bproto);

    register uint64_t rsp asm ("rsp") = physicalPageAlloc(4) + 0x4000 + KERNEL_HIGH_VMA;

    kHeapInit();

    uint64_t *bruh1 = kmalloc(33);
    kfree(bruh1);
    uint64_t *bruh2 = kmalloc(1);
    uint64_t *bruh3 = kmalloc(4);

    kprintDS(NULL, "bruh1 : %x", bruh1); 
    kprintDS(NULL, "bruh2 : %x", bruh2); 
    kprintDS(NULL, "bruh3 : %x", bruh3); 
    //    initVMM();

    for(;;);
}
