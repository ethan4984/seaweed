#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/drivers/ahci.h>
#include <kernel/drivers/vesa.h>
#include <kernel/drivers/bmp.h>
#include <kernel/fs/bfs.h>
#include <libk/output.h>

static drives_t drives;

void drawBmp(const char *fileName) {
    drives = getDrives(); 
    uint32_t *data = (void*)(physicalPageAlloc(0x300) + HIGH_VMA);

    for(uint32_t i = 0; i < (0x300 * 8) + 2; i++) {
        sataRW(&drives.drive[0], i+ 0x7800, 1, (void*)((uint64_t)data + (i * 0x200)), 0); 
    }

    bmpFileHdr_t *bmpFileHdr = (bmpFileHdr_t*)data;

    kprintDS("[KDEBUG]", "bfType %x", bmpFileHdr->bfType);
    kprintDS("[KDEBUG]", "bfSize %x", bmpFileHdr->bfSize);
    kprintDS("[KDEBUG]", "bfOffset %x", bmpFileHdr->bfOffset);
    kprintDS("[KDEBUG]", "Width %d Height %d", bmpFileHdr->biWidth, bmpFileHdr->biHeight);
    kprintDS("[KDEBUG]", "bpp %d", bmpFileHdr->biBitCount);
    kprintDS("[KDEBUG]", "blue mask %x", bmpFileHdr->blueMask);
    kprintDS("[KDEBUG]", "red mask %x", bmpFileHdr->redMask);
    kprintDS("[KDEBUG]", "green mask %x", bmpFileHdr->greenMask);

    data = (void*)((uint64_t)data + bmpFileHdr->bfOffset);

    uint64_t x = 0, y = bmpFileHdr->biHeight;

    for(uint32_t i = 0; i < bmpFileHdr->biHeight; i++) {
        for(uint32_t j = 0; j < 1024; j++)
            setPixel(x++, y, data[j + (i * 1024)]); 
        x = 0;
        y--;
    }
}
