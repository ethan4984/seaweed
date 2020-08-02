#include <kernel/mm/virtualPageManager.h>
#include <kernel/mm/physicalPageManager.h>
#include <kernel/sched/scheduler.h>
#include <kernel/drivers/vesa.h>
#include <kernel/drivers/pci.h>
#include <kernel/int/syscall.h>
#include <kernel/sched/hpet.h>
#include <kernel/sched/smp.h>
#include <kernel/acpi/rsdp.h>
#include <kernel/acpi/madt.h>
#include <kernel/int/apic.h>
#include <kernel/mm/kHeap.h>
#include <kernel/int/idt.h>
#include <kernel/int/gdt.h>
#include <kernel/int/tss.h>
#include <kernel/bproto.h>
#include <libk/memUtils.h>
#include <libk/output.h>
#include <libu/logs.h>

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

    kprintDS("[KDEBUG]", "%x", sizeof(regs_t));

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
    initAPIC();

    initGDT();
    initPCI();
    
    createGenericTSS(stack);
    createNewGDT(0, grabTSS(0));

    idtInit();

    asm volatile ("sti");

    lapicTimerInit(100);

    initSMP();

    schedulerInit();

    createNewTask(0x10, physicalPageAlloc(1) + HIGH_VMA + 0x1000, 0x8, (uint64_t)&task1, 10);
    createNewTask(0x23, physicalPageAlloc(1) + HIGH_VMA + 0x1000, 0x1b, (uint64_t)&task2, 9);
    createNewTask(0x23, physicalPageAlloc(1) + HIGH_VMA + 0x1000, 0x1b, (uint64_t)&task3, 8);
    createNewTask(0x23, physicalPageAlloc(1) + HIGH_VMA + 0x1000, 0x1b, (uint64_t)&task4, 7);
    createNewTask(0x23, physicalPageAlloc(1) + HIGH_VMA + 0x1000, 0x1b, (uint64_t)&task5, 6);
    createNewTask(0x23, physicalPageAlloc(1) + HIGH_VMA + 0x1000, 0x1b, (uint64_t)&task6, 5);

    for(;;);
}

void task1() {
    while(1) {
        for(uint64_t i = 0; i < 100000000; i++);
//        asm volatile ("movq $0x69, %rax\n" "int $0x69");
    }
}

void task2() {
    while(1) {
        for(uint64_t i = 0; i < 10000000; i++);
        printf("hi from userspace");
    }
}

void task3() {
    while(1) {
        for(uint64_t i = 0; i < 100000000; i++);
        asm volatile ("movq $0x69420, %rax\n" "int $0x69");
    }
}

void task4() {
    while(1) {
        for(uint64_t i = 0; i < 100000000; i++);
        asm volatile ("movq $0x1, %rax\n" "int $0x69");
    }

}

void task5() {
    while(1) {
        for(uint64_t i = 0; i < 100000000; i++);
        asm volatile ("movq $0x2, %rax\n" "int $0x69");
    }
}

void task6() {
    while(1) {
        for(uint64_t i = 0; i < 100000000; i++);
        asm volatile ("movq $0x3, %rax\n" "int $0x69");
    }
}
