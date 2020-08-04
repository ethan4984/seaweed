#include <kernel/mm/kHeap.h>
#include <kernel/drivers/ahci.h> 
#include <kernel/fs/gfs.h>
#include <libk/memUtils.h>
#include <libk/output.h>

directory_t *rootDir;
static drives_t drives;

void initGFS() {
    drives = getDrives();
    rootDir = kmalloc(sizeof(directory_t));

/*    uint16_t *buffer = kmalloc(0x200);
    for(uint64_t i = 0; i < drives.driveCnt; i++) {
        for(uint64_t j = 0; j < drives.drive[i].sectorCount; j++) {
            memset(buffer, 0, 0x200);
            sataRW(&drives.drive[i], j, 1, buffer, 0);
            if(*(uint32_t*)buffer == MAGIC_NUMBER) { 
                 
            }
        }
    }*/
}

void test() {
    uint16_t *buffer = kmalloc(512);
    
    memset(buffer, 0, 0x200);
    sataRW(&drives.drive[0], 0, 1, buffer, 0);

    for(uint64_t i = 0; i < 0x200 / 2; i++) {
        kprintDS("[KDEBUG]", "%x ", buffer[i]);
    }

    kprintDS("[KDEBUG]", "BRUH LEL");
    kprintDS("[KDEBUG]", "BRUH LEL");

    memset(buffer, 0, 0x200);
    buffer[0] = 0x69;
    sataRW(&drives.drive[0], 0, 1, buffer, 1);

    memset(buffer, 0, 0x200);
    sataRW(&drives.drive[0], 0, 1, buffer, 0);

    for(uint64_t i = 0; i < 0x200 / 2; i++) {
        kprintDS("[KDEBUG]", "%x ", buffer[i]);
    }
}
