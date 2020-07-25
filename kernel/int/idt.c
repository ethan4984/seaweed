#include <kernel/sched/scheduler.h>
#include <kernel/int/apic.h>
#include <kernel/int/idt.h>
#include <lib/asmUtils.h>
#include <lib/output.h>

#include <stddef.h>

eventHandlers_t eventHandlers[];
const char *exceptionMessages[];

idtEntry_t idt[256];
idtr_t idtr;

void setIDTentry(uint8_t codeSelector, uint8_t index, uint8_t typesAndAttributes, uint64_t offset) {
    idt[index].offsetLow = (uint16_t)(offset >> 0);
    idt[index].selector = codeSelector;
    idt[index].zero8 = 0;
    idt[index].attributes = typesAndAttributes;
    idt[index].offsetMid = (uint16_t)(offset >> 16);
    idt[index].offsetHigh = (uint32_t)(offset >> 32);
    idt[index].zero32 = 0;
}

extern void isrHandlerMain(regs_t *stack) {
    if(eventHandlers[stack->isrNumber] != NULL) {
        eventHandlers[stack->isrNumber](stack);
    }

    if(stack->isrNumber < 32) {
        uint64_t cr2;
        asm volatile ("cli\n" "mov %%cr2, %0" : "=a"(cr2)); 
        kprintDS("[KDEBUG]", "Congrates: you fucked up with a nice <%s> have fun debugging this", exceptionMessages[stack->isrNumber]);
        kprintDS("[KDEBUG]", "RAX: %a | RBX: %a | RCX: %a | RDX: %a", stack->rax, stack->rbx, stack->rcx, stack->rdx);
        kprintDS("[KDEBUG]", "RSI: %a | RDI: %a | RBP: %a | RSP: %a", stack->rsi, stack->rdi, stack->rbp, stack->rsp);
        kprintDS("[KDEBUG]", "r8:  %a | r9:  %a | r10: %a | r11: %a", stack->r8, stack->r9, stack->r10, stack->r11); 
        kprintDS("[KDEBUG]", "r12: %a | r13: %a | r14: %a | r15: %a", stack->r12, stack->r13, stack->r14, stack->r15); 
        kprintDS("[KDEBUG]", "cr2: %a | rip: %a", cr2, stack->rip);
        for(;;);
    }
    lapicWrite(LAPIC_EOI, 0);
}

void idtInit() {
    setIDTentry(40, 0, 0x8f, (uint64_t)isr0);
    setIDTentry(40, 1, 0x8f, (uint64_t)isr1);
    setIDTentry(40, 2, 0x8f, (uint64_t)isr2);
    setIDTentry(40, 3, 0x8f, (uint64_t)isr3);
    setIDTentry(40, 4, 0x8f, (uint64_t)isr4);
    setIDTentry(40, 5, 0x8f, (uint64_t)isr5);
    setIDTentry(40, 6, 0x8f, (uint64_t)isr6);
    setIDTentry(40, 7, 0x8f, (uint64_t)isr7);
    setIDTentry(40, 8, 0x8f, (uint64_t)isr8);
    setIDTentry(40, 9, 0x8f, (uint64_t)isr9);
    setIDTentry(40, 10, 0x8f, (uint64_t)isr10);
    setIDTentry(40, 11, 0x8f, (uint64_t)isr11);
    setIDTentry(40, 12, 0x8f, (uint64_t)isr12);
    setIDTentry(40, 13, 0x8f, (uint64_t)isr13);
    setIDTentry(40, 14, 0x8f, (uint64_t)isr14);
    setIDTentry(40, 15, 0x8f, (uint64_t)isr15);
    setIDTentry(40, 16, 0x8f, (uint64_t)isr16);
    setIDTentry(40, 17, 0x8f, (uint64_t)isr17);
    setIDTentry(40, 18, 0x8f, (uint64_t)isr18);
    setIDTentry(40, 19, 0x8f, (uint64_t)isr19);
    setIDTentry(40, 20, 0x8f, (uint64_t)isr20);
    setIDTentry(40, 21, 0x8f, (uint64_t)isr21);
    setIDTentry(40, 22, 0x8f, (uint64_t)isr22);
    setIDTentry(40, 23, 0x8f, (uint64_t)isr23);
    setIDTentry(40, 24, 0x8f, (uint64_t)isr24);
    setIDTentry(40, 25, 0x8f, (uint64_t)isr25);
    setIDTentry(40, 26, 0x8f, (uint64_t)isr26);
    setIDTentry(40, 27, 0x8f, (uint64_t)isr27);
    setIDTentry(40, 28, 0x8f, (uint64_t)isr28);
    setIDTentry(40, 29, 0x8f, (uint64_t)isr29);
    setIDTentry(40, 30, 0x8f, (uint64_t)isr30);
    setIDTentry(40, 31, 0x8f, (uint64_t)isr31);
    setIDTentry(40, 32, 0x8f, (uint64_t)isr32);
    setIDTentry(40, 33, 0x8f, (uint64_t)isr33);
    setIDTentry(40, 34, 0x8f, (uint64_t)isr34);
    setIDTentry(40, 35, 0x8f, (uint64_t)isr35);
    setIDTentry(40, 36, 0x8f, (uint64_t)isr36);
    setIDTentry(40, 37, 0x8f, (uint64_t)isr37);
    setIDTentry(40, 38, 0x8f, (uint64_t)isr38);
    setIDTentry(40, 39, 0x8f, (uint64_t)isr39);
    setIDTentry(40, 40, 0x8f, (uint64_t)isr40);
    setIDTentry(40, 41, 0x8f, (uint64_t)isr41);
    setIDTentry(40, 42, 0x8f, (uint64_t)isr42);
    setIDTentry(40, 43, 0x8f, (uint64_t)isr43);
    setIDTentry(40, 44, 0x8f, (uint64_t)isr44);
    setIDTentry(40, 45, 0x8f, (uint64_t)isr45);
    setIDTentry(40, 46, 0x8f, (uint64_t)isr46);
    setIDTentry(40, 47, 0x8f, (uint64_t)isr47);
    setIDTentry(40, 48, 0x8f, (uint64_t)isr48);
    setIDTentry(40, 49, 0x8f, (uint64_t)isr49);
    setIDTentry(40, 50, 0x8f, (uint64_t)isr50);
    setIDTentry(40, 51, 0x8f, (uint64_t)isr51);
    setIDTentry(40, 52, 0x8f, (uint64_t)isr52);
    setIDTentry(40, 53, 0x8f, (uint64_t)isr53);
    setIDTentry(40, 54, 0x8f, (uint64_t)isr54);
    setIDTentry(40, 55, 0x8f, (uint64_t)isr55);
    setIDTentry(40, 56, 0x8f, (uint64_t)isr56);
    setIDTentry(40, 57, 0x8f, (uint64_t)isr57);
    setIDTentry(40, 58, 0x8f, (uint64_t)isr58);
    setIDTentry(40, 59, 0x8f, (uint64_t)isr59);
    setIDTentry(40, 60, 0x8f, (uint64_t)isr60);
    setIDTentry(40, 61, 0x8f, (uint64_t)isr61);
    setIDTentry(40, 62, 0x8f, (uint64_t)isr62);
    setIDTentry(40, 63, 0x8f, (uint64_t)isr63);
    setIDTentry(40, 64, 0x8f, (uint64_t)isr64);
    setIDTentry(40, 65, 0x8f, (uint64_t)isr65);
    setIDTentry(40, 66, 0x8f, (uint64_t)isr66);
    setIDTentry(40, 67, 0x8f, (uint64_t)isr67);
    setIDTentry(40, 68, 0x8f, (uint64_t)isr68);
    setIDTentry(40, 69, 0x8f, (uint64_t)isr69);
    setIDTentry(40, 70, 0x8f, (uint64_t)isr70);
    setIDTentry(40, 71, 0x8f, (uint64_t)isr71);
    setIDTentry(40, 72, 0x8f, (uint64_t)isr72);
    setIDTentry(40, 73, 0x8f, (uint64_t)isr73);
    setIDTentry(40, 74, 0x8f, (uint64_t)isr74);
    setIDTentry(40, 75, 0x8f, (uint64_t)isr75);
    setIDTentry(40, 76, 0x8f, (uint64_t)isr76);
    setIDTentry(40, 77, 0x8f, (uint64_t)isr77);
    setIDTentry(40, 78, 0x8f, (uint64_t)isr78);
    setIDTentry(40, 79, 0x8f, (uint64_t)isr79);
    setIDTentry(40, 80, 0x8f, (uint64_t)isr80);
    setIDTentry(40, 81, 0x8f, (uint64_t)isr81);
    setIDTentry(40, 82, 0x8f, (uint64_t)isr82);
    setIDTentry(40, 83, 0x8f, (uint64_t)isr83);
    setIDTentry(40, 84, 0x8f, (uint64_t)isr84);
    setIDTentry(40, 85, 0x8f, (uint64_t)isr85);
    setIDTentry(40, 86, 0x8f, (uint64_t)isr86);
    setIDTentry(40, 87, 0x8f, (uint64_t)isr87);
    setIDTentry(40, 88, 0x8f, (uint64_t)isr88);
    setIDTentry(40, 89, 0x8f, (uint64_t)isr89);
    setIDTentry(40, 90, 0x8f, (uint64_t)isr90);
    setIDTentry(40, 91, 0x8f, (uint64_t)isr91);
    setIDTentry(40, 92, 0x8f, (uint64_t)isr92);
    setIDTentry(40, 93, 0x8f, (uint64_t)isr93);
    setIDTentry(40, 94, 0x8f, (uint64_t)isr94);
    setIDTentry(40, 95, 0x8f, (uint64_t)isr95);
    setIDTentry(40, 96, 0x8f, (uint64_t)isr96);
    setIDTentry(40, 97, 0x8f, (uint64_t)isr97);
    setIDTentry(40, 98, 0x8f, (uint64_t)isr98);
    setIDTentry(40, 99, 0x8f, (uint64_t)isr99);
    setIDTentry(40, 100, 0x8f, (uint64_t)isr100);
    setIDTentry(40, 101, 0x8f, (uint64_t)isr101);
    setIDTentry(40, 102, 0x8f, (uint64_t)isr102);
    setIDTentry(40, 103, 0x8f, (uint64_t)isr103);
    setIDTentry(40, 104, 0x8f, (uint64_t)isr104);
    setIDTentry(40, 105, 0x8f, (uint64_t)isr105);
    setIDTentry(40, 106, 0x8f, (uint64_t)isr106);
    setIDTentry(40, 107, 0x8f, (uint64_t)isr107);
    setIDTentry(40, 108, 0x8f, (uint64_t)isr108);
    setIDTentry(40, 109, 0x8f, (uint64_t)isr109);
    setIDTentry(40, 110, 0x8f, (uint64_t)isr110);
    setIDTentry(40, 111, 0x8f, (uint64_t)isr111);
    setIDTentry(40, 112, 0x8f, (uint64_t)isr112);
    setIDTentry(40, 113, 0x8f, (uint64_t)isr113);
    setIDTentry(40, 114, 0x8f, (uint64_t)isr114);
    setIDTentry(40, 115, 0x8f, (uint64_t)isr115);
    setIDTentry(40, 116, 0x8f, (uint64_t)isr116);
    setIDTentry(40, 117, 0x8f, (uint64_t)isr117);
    setIDTentry(40, 118, 0x8f, (uint64_t)isr118);
    setIDTentry(40, 119, 0x8f, (uint64_t)isr119);
    setIDTentry(40, 120, 0x8f, (uint64_t)isr120);
    setIDTentry(40, 121, 0x8f, (uint64_t)isr121);
    setIDTentry(40, 122, 0x8f, (uint64_t)isr122);
    setIDTentry(40, 123, 0x8f, (uint64_t)isr123);
    setIDTentry(40, 124, 0x8f, (uint64_t)isr124);
    setIDTentry(40, 125, 0x8f, (uint64_t)isr125);
    setIDTentry(40, 126, 0x8f, (uint64_t)isr126);
    setIDTentry(40, 127, 0x8f, (uint64_t)isr127);
    setIDTentry(40, 128, 0x8f, (uint64_t)isr128);
    setIDTentry(40, 129, 0x8f, (uint64_t)isr129);
    setIDTentry(40, 130, 0x8f, (uint64_t)isr130);
    setIDTentry(40, 131, 0x8f, (uint64_t)isr131);
    setIDTentry(40, 132, 0x8f, (uint64_t)isr132);
    setIDTentry(40, 133, 0x8f, (uint64_t)isr133);
    setIDTentry(40, 134, 0x8f, (uint64_t)isr134);
    setIDTentry(40, 135, 0x8f, (uint64_t)isr135);
    setIDTentry(40, 136, 0x8f, (uint64_t)isr136);
    setIDTentry(40, 137, 0x8f, (uint64_t)isr137);
    setIDTentry(40, 138, 0x8f, (uint64_t)isr138);
    setIDTentry(40, 139, 0x8f, (uint64_t)isr139);
    setIDTentry(40, 140, 0x8f, (uint64_t)isr140);
    setIDTentry(40, 141, 0x8f, (uint64_t)isr141);
    setIDTentry(40, 142, 0x8f, (uint64_t)isr142);
    setIDTentry(40, 143, 0x8f, (uint64_t)isr143);
    setIDTentry(40, 144, 0x8f, (uint64_t)isr144);
    setIDTentry(40, 145, 0x8f, (uint64_t)isr145);
    setIDTentry(40, 146, 0x8f, (uint64_t)isr146);
    setIDTentry(40, 147, 0x8f, (uint64_t)isr147);
    setIDTentry(40, 148, 0x8f, (uint64_t)isr148);
    setIDTentry(40, 149, 0x8f, (uint64_t)isr149);
    setIDTentry(40, 150, 0x8f, (uint64_t)isr150);
    setIDTentry(40, 151, 0x8f, (uint64_t)isr151);
    setIDTentry(40, 152, 0x8f, (uint64_t)isr152);
    setIDTentry(40, 153, 0x8f, (uint64_t)isr153);
    setIDTentry(40, 154, 0x8f, (uint64_t)isr154);
    setIDTentry(40, 155, 0x8f, (uint64_t)isr155);
    setIDTentry(40, 156, 0x8f, (uint64_t)isr156);
    setIDTentry(40, 157, 0x8f, (uint64_t)isr157);
    setIDTentry(40, 158, 0x8f, (uint64_t)isr158);
    setIDTentry(40, 159, 0x8f, (uint64_t)isr159);
    setIDTentry(40, 160, 0x8f, (uint64_t)isr160);
    setIDTentry(40, 161, 0x8f, (uint64_t)isr161);
    setIDTentry(40, 162, 0x8f, (uint64_t)isr162);
    setIDTentry(40, 163, 0x8f, (uint64_t)isr163);
    setIDTentry(40, 164, 0x8f, (uint64_t)isr164);
    setIDTentry(40, 165, 0x8f, (uint64_t)isr165);
    setIDTentry(40, 166, 0x8f, (uint64_t)isr166);
    setIDTentry(40, 167, 0x8f, (uint64_t)isr167);
    setIDTentry(40, 168, 0x8f, (uint64_t)isr168);
    setIDTentry(40, 169, 0x8f, (uint64_t)isr169);
    setIDTentry(40, 170, 0x8f, (uint64_t)isr170);
    setIDTentry(40, 171, 0x8f, (uint64_t)isr171);
    setIDTentry(40, 172, 0x8f, (uint64_t)isr172);
    setIDTentry(40, 173, 0x8f, (uint64_t)isr173);
    setIDTentry(40, 174, 0x8f, (uint64_t)isr174);
    setIDTentry(40, 175, 0x8f, (uint64_t)isr175);
    setIDTentry(40, 176, 0x8f, (uint64_t)isr176);
    setIDTentry(40, 177, 0x8f, (uint64_t)isr177);
    setIDTentry(40, 178, 0x8f, (uint64_t)isr178);
    setIDTentry(40, 179, 0x8f, (uint64_t)isr179);
    setIDTentry(40, 180, 0x8f, (uint64_t)isr180);
    setIDTentry(40, 181, 0x8f, (uint64_t)isr181);
    setIDTentry(40, 182, 0x8f, (uint64_t)isr182);
    setIDTentry(40, 183, 0x8f, (uint64_t)isr183);
    setIDTentry(40, 184, 0x8f, (uint64_t)isr184);
    setIDTentry(40, 185, 0x8f, (uint64_t)isr185);
    setIDTentry(40, 186, 0x8f, (uint64_t)isr186);
    setIDTentry(40, 187, 0x8f, (uint64_t)isr187);
    setIDTentry(40, 188, 0x8f, (uint64_t)isr188);
    setIDTentry(40, 189, 0x8f, (uint64_t)isr189);
    setIDTentry(40, 190, 0x8f, (uint64_t)isr190);
    setIDTentry(40, 191, 0x8f, (uint64_t)isr191);
    setIDTentry(40, 192, 0x8f, (uint64_t)isr192);
    setIDTentry(40, 193, 0x8f, (uint64_t)isr193);
    setIDTentry(40, 194, 0x8f, (uint64_t)isr194);
    setIDTentry(40, 195, 0x8f, (uint64_t)isr195);
    setIDTentry(40, 196, 0x8f, (uint64_t)isr196);
    setIDTentry(40, 197, 0x8f, (uint64_t)isr197);
    setIDTentry(40, 198, 0x8f, (uint64_t)isr198);
    setIDTentry(40, 199, 0x8f, (uint64_t)isr199);
    setIDTentry(40, 200, 0x8f, (uint64_t)isr200);
    setIDTentry(40, 201, 0x8f, (uint64_t)isr201);
    setIDTentry(40, 202, 0x8f, (uint64_t)isr202);
    setIDTentry(40, 203, 0x8f, (uint64_t)isr203);
    setIDTentry(40, 204, 0x8f, (uint64_t)isr204);
    setIDTentry(40, 205, 0x8f, (uint64_t)isr205);
    setIDTentry(40, 206, 0x8f, (uint64_t)isr206);
    setIDTentry(40, 207, 0x8f, (uint64_t)isr207);
    setIDTentry(40, 208, 0x8f, (uint64_t)isr208);
    setIDTentry(40, 209, 0x8f, (uint64_t)isr209);
    setIDTentry(40, 210, 0x8f, (uint64_t)isr210);
    setIDTentry(40, 211, 0x8f, (uint64_t)isr211);
    setIDTentry(40, 212, 0x8f, (uint64_t)isr212);
    setIDTentry(40, 213, 0x8f, (uint64_t)isr213);
    setIDTentry(40, 214, 0x8f, (uint64_t)isr214);
    setIDTentry(40, 215, 0x8f, (uint64_t)isr215);
    setIDTentry(40, 216, 0x8f, (uint64_t)isr216);
    setIDTentry(40, 217, 0x8f, (uint64_t)isr217);
    setIDTentry(40, 218, 0x8f, (uint64_t)isr218);
    setIDTentry(40, 219, 0x8f, (uint64_t)isr219);
    setIDTentry(40, 220, 0x8f, (uint64_t)isr220);
    setIDTentry(40, 221, 0x8f, (uint64_t)isr221);
    setIDTentry(40, 222, 0x8f, (uint64_t)isr222);
    setIDTentry(40, 223, 0x8f, (uint64_t)isr223);
    setIDTentry(40, 224, 0x8f, (uint64_t)isr224);
    setIDTentry(40, 225, 0x8f, (uint64_t)isr225);
    setIDTentry(40, 226, 0x8f, (uint64_t)isr226);
    setIDTentry(40, 227, 0x8f, (uint64_t)isr227);
    setIDTentry(40, 228, 0x8f, (uint64_t)isr228);
    setIDTentry(40, 229, 0x8f, (uint64_t)isr229);
    setIDTentry(40, 230, 0x8f, (uint64_t)isr230);
    setIDTentry(40, 231, 0x8f, (uint64_t)isr231);
    setIDTentry(40, 232, 0x8f, (uint64_t)isr232);
    setIDTentry(40, 233, 0x8f, (uint64_t)isr233);
    setIDTentry(40, 234, 0x8f, (uint64_t)isr234);
    setIDTentry(40, 235, 0x8f, (uint64_t)isr235);
    setIDTentry(40, 236, 0x8f, (uint64_t)isr236);
    setIDTentry(40, 237, 0x8f, (uint64_t)isr237);
    setIDTentry(40, 238, 0x8f, (uint64_t)isr238);
    setIDTentry(40, 239, 0x8f, (uint64_t)isr239);
    setIDTentry(40, 240, 0x8f, (uint64_t)isr240);
    setIDTentry(40, 241, 0x8f, (uint64_t)isr241);
    setIDTentry(40, 242, 0x8f, (uint64_t)isr242);
    setIDTentry(40, 243, 0x8f, (uint64_t)isr243);
    setIDTentry(40, 244, 0x8f, (uint64_t)isr244);
    setIDTentry(40, 245, 0x8f, (uint64_t)isr245);
    setIDTentry(40, 246, 0x8f, (uint64_t)isr246);
    setIDTentry(40, 247, 0x8f, (uint64_t)isr247);
    setIDTentry(40, 248, 0x8f, (uint64_t)isr248);
    setIDTentry(40, 249, 0x8f, (uint64_t)isr249);
    setIDTentry(40, 250, 0x8f, (uint64_t)isr250);
    setIDTentry(40, 251, 0x8f, (uint64_t)isr251);
    setIDTentry(40, 252, 0x8f, (uint64_t)isr252);
    setIDTentry(40, 253, 0x8f, (uint64_t)isr253);
    setIDTentry(40, 254, 0x8f, (uint64_t)isr254);

    idtr.limit = 256 * sizeof(idtEntry_t) - 1;
    idtr.offset = (uint64_t)&idt;

    asm volatile ("lidtq %0" : "=m"(idtr));
}

const char *exceptionMessages[] = { "Divide by zero",
                                    "Debug",
                                    "NMI",
                                    "Breakpoint",
                                    "Overflow",
                                    "Bound Range Exceeded",
                                    "Invaild Opcode",
                                    "Device Not Available", 
                                    "Double fault", 
                                    "Co-processor Segment Overrun",
                                    "Invaild TSS",
                                    "Segment not present",
                                    "Stack-Segment Fault",
                                    "GPR",
                                    "Page Fault",
                                    "Reserved",
                                    "x87 Floating Point Exception",
                                    "allignement check",
                                    "Machine check",
                                    "SIMD floating-point exception",
                                    "Virtualization Excpetion",
                                    "Reserved",
                                    "Reserved",
                                    "Reserved",
                                    "Reserved",
                                    "Reserved",
                                    "Reserved",
                                    "Reserved",
                                    "Reserved",
                                    "Reserved",
                                    "Reserved",
                                    "Security Exception",
                                    "Reserved",
                                    "Triple Fault", 
                                    "FPU error"
                                  };

eventHandlers_t eventHandlers[] =   {
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // isr 0 -> isr 7
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // isr 7 -> 15
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // isr 15 -> 23
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // isr 23 -> 31
                                        schedulerMain, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // isr 31 -> 38
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, rescheduleCore, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
                                    };
