#include <kernel/mm/kHeap.h>
#include <kernel/int/tss.h>
#include <kernel/int/gdt.h>
#include <lib/memUtils.h>
#include <lib/output.h>

gdt_t *gdt;

void initGDT() {
    gdt = kmalloc(sizeof(gdt_t) * 8);
    memset(gdt, 0, sizeof(gdt_t) * 8);
    initTSS();
}

void createNewGDT(uint64_t core, uint64_t tssAddr) {
    /* code 64 */

    gdt[core].gdtEntries[1].limit = 0;
    gdt[core].gdtEntries[1].baseLow = 0;
    gdt[core].gdtEntries[1].baseMid = 0;
    gdt[core].gdtEntries[1].access = 0b10011010; 
    gdt[core].gdtEntries[1].granularity = 0b00100000;
    gdt[core].gdtEntries[1].baseHigh = 0;

    /* data 64 */

    gdt[core].gdtEntries[2].limit = 0;
    gdt[core].gdtEntries[2].baseLow = 0;
    gdt[core].gdtEntries[2].baseMid = 0;
    gdt[core].gdtEntries[2].access = 0b10010010; 
    gdt[core].gdtEntries[2].granularity = 0;
    gdt[core].gdtEntries[2].baseHigh = 0;

    /* user code 64 */

    gdt[core].gdtEntries[3].limit = 0;
    gdt[core].gdtEntries[3].baseLow = 0;
    gdt[core].gdtEntries[3].baseMid = 0;
    gdt[core].gdtEntries[3].access = 0b11111101; 
    gdt[core].gdtEntries[3].granularity = 0b10101111;
    gdt[core].gdtEntries[3].baseHigh = 0;

    /* user data 64 */

    gdt[core].gdtEntries[4].limit = 0;
    gdt[core].gdtEntries[4].baseLow = 0;
    gdt[core].gdtEntries[4].baseMid = 0;
    gdt[core].gdtEntries[4].access = 0b11110011; 
    gdt[core].gdtEntries[4].granularity = 0b11001111;
    gdt[core].gdtEntries[4].baseHigh = 0;

    gdt[core].tss.length = 104;
    gdt[core].tss.baseLow = (uint16_t)tssAddr;
    gdt[core].tss.baseMid = (uint8_t)(tssAddr >> 16);
    gdt[core].tss.flags1 = 0b10001001; 
    gdt[core].tss.flags2 = 0;
    gdt[core].tss.baseHigh = (uint8_t)(tssAddr >> 24);
    gdt[core].tss.baseHigh32 = (uint32_t)(tssAddr >> 32);

    gdt[core].gdtPtr.size = sizeof(gdt_t) - sizeof(gdtPtr_t) - 1;
    gdt[core].gdtPtr.addr = (uint64_t)&gdt[core];

    lgdt((uint64_t)&gdt[core].gdtPtr);
}
