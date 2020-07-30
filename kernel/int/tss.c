#include <kernel/mm/physicalPageManager.h>
#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/kHeap.h>
#include <kernel/int/tss.h>

tss_t *tss;

void initTSS() {
    tss = kmalloc(sizeof(tss_t) * 8);
}

uint64_t grabTSS(uint64_t index) {
    return (uint64_t)&tss[index];
}

void createGenericTSS(uint64_t rsp0) {
    tss_t newTSS = { 0 };
    newTSS.rsp0 = rsp0;
    addNewTSS(newTSS);
}

void addNewTSS(tss_t newTSS) {
    static uint64_t max = 8, current = 0;
    if(++current > max) {
        tss = krealloc(tss, sizeof(tss_t) * 8);
        max += 8;
    }
    tss[current - 1] = newTSS;
}
