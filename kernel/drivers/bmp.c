#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/drivers/ahci.h>
#include <kernel/drivers/vesa.h>
#include <kernel/drivers/bmp.h>
#include <kernel/fs/bfs.h>
#include <libk/output.h>

void drawBmp(const char *fileName) {
    bmpFileHdr_t *bmpFileHdr = openFile(fileName);

    kprintDS("[KDEBUG]", "bfType %x", bmpFileHdr->bfType);
    kprintDS("[KDEBUG]", "bfSize %x", bmpFileHdr->bfSize);
    kprintDS("[KDEBUG]", "bfOffset %x", bmpFileHdr->bfOffset);
    kprintDS("[KDEBUG]", "Width %d Height %d", bmpFileHdr->biWidth, bmpFileHdr->biHeight);
    kprintDS("[KDEBUG]", "bpp %d", bmpFileHdr->biBitCount);
    kprintDS("[KDEBUG]", "blue mask %x", bmpFileHdr->blueMask);
    kprintDS("[KDEBUG]", "red mask %x", bmpFileHdr->redMask);
    kprintDS("[KDEBUG]", "green mask %x", bmpFileHdr->greenMask);

    uint32_t *data = (void*)((uint64_t)bmpFileHdr + bmpFileHdr->bfOffset);

    uint64_t x = 0, y = bmpFileHdr->biHeight;

    uint32_t inc = 0;
    for(uint64_t i = 0; i < ROUNDUP(bmpFileHdr->bfSize, 0x200); i++) {
        uint32_t *colourBuffer = (void*)((uint64_t)data + 2);
        inc += 1;

        for(uint32_t i = 0; i < (0x200 / 4); i++) {
            setPixel(x++, y, colourBuffer[i]);
            if(x == bmpFileHdr->biWidth) {
                x = 0;
                y--;
            }
        }
        fpInc(fileName, data, inc);
    } 
}
