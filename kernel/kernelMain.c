#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/physicalPageManager.h>
#include <kernel/sched/scheduler.h>
#include <kernel/drivers/vesa.h>
#include <kernel/sched/hpet.h>
#include <kernel/sched/smp.h>
#include <kernel/acpi/rsdp.h>
#include <kernel/acpi/madt.h>
#include <kernel/int/apic.h>
#include <kernel/mm/kHeap.h>
#include <kernel/int/idt.h>
#include <kernel/bproto.h>
#include <lib/memUtils.h>
#include <lib/output.h>

#include <stdint.h>
#include <stddef.h>

extern symbol bssBegin;
extern symbol bssEnd;

extern void testDiv();

void task1();
void task2();
void task3();
void task4();
void task5();
void task6();

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

    uint64_t stack = physicalPageAlloc(4) + 0x4000 + KERNEL_HIGH_VMA;
    asm volatile ("movq %0, %%rsp" : "=r"(stack)); 

    kHeapInit();
    initVMM();

    initVESA(bproto);
    initVESAtext(0xffffffff, 0, bproto->width, bproto->height);

    kprintVS("Welcome to seaweed\n");

    rsdpInit((uint64_t*)(bproto->rsdp + HIGH_VMA));

    initHPET();
    madtInit();
    idtInit();
    initAPIC();
    
    asm volatile ("sti");

    initSMP();

    schedulerInit();

    for(;;);
}

void task1() {
    uint64_t bruh = 0;
    while(1) {
        for(int i = 0; i < 1000000; i++);
        serialWrite('1');
        serialWrite('\n');
        bruh += 69420;
    }
}

void task2() {
    uint64_t bruh = 0;
    while(1) {
       for(uint64_t i = 0; i < 1000000; i++);
        serialWrite('2');
        serialWrite('\n');
       bruh += 1;
    }
}

void task3() {
    uint64_t bruh = 0;
    while(1) {
       for(int i = 0; i < 1000000; i++);
       serialWrite('3');
        serialWrite('\n');
       bruh += 2;
    }
}

void task4() {
    uint64_t bruh = 0;
    while(1) {
        for(int i = 0; i < 1000000; i++);
        serialWrite('4');
        serialWrite('\n');
        bruh += 420;
    }
}

void task5() {
    uint64_t bruh = 0;
    while(1) {
        for(int i = 0; i < 1000000; i++);
        serialWrite('5');
        serialWrite('\n');
        bruh += 69;
    }
}

void task6() {
    uint64_t bruh = 0;
    while(1) {
        for(int i = 0; i < 1000000; i++);
        serialWrite('6');
        serialWrite('\n');
        bruh += 69;
    }
}
