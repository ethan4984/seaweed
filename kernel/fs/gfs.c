#include <kernel/mm/kHeap.h>
#include <kernel/drivers/ahci.h> 
#include <kernel/fs/gfs.h>
#include <libk/memUtils.h>
#include <libk/output.h>

drives_t *drives;
uint64_t driveCount = 0;

void addDrive(uint64_t sectorCount, volatile hbaPorts_t *hbaPort) {
    if(driveCount + 1 == 32) {
        kprintDS("[ACPI]", "Extended drive limit");
    }
    drives[driveCount++] = (drives_t) { sectorCount, hbaPort };
}

void initGFS() {
    drives = kmalloc(sizeof(drives_t) * 32);
}

void test() {
    uint16_t *buffer = kmalloc(512);
    
    memset(buffer, 0, 0x200);
    sataRW(&drives[0], 0, 1, buffer, 0);

    for(uint64_t i = 0; i < 0x200 / 2; i++) {
        kprintDS("[KDEBUG]", "%x ", buffer[i]);
    }

    kprintDS("[KDEBUG]", "BRUH LEL");
    kprintDS("[KDEBUG]", "BRUH LEL");

    memset(buffer, 0, 0x200);
    buffer[0] = 0x69;
    sataRW(&drives[0], 0, 1, buffer, 1);

    memset(buffer, 0, 0x200);
    sataRW(&drives[0], 0, 1, buffer, 0);

    for(uint64_t i = 0; i < 0x200 / 2; i++) {
        kprintDS("[KDEBUG]", "%x ", buffer[i]);
    }
}
