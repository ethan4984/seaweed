#include <kernel/sched/scheduler.h>
#include <kernel/int/syscall.h>
#include <kernel/int/apic.h>
#include <kernel/int/idt.h>
#include <libk/asmUtils.h>
#include <libk/output.h>

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
    lapicWrite(LAPIC_EOI, 0);
    if(eventHandlers[stack->isrNumber] != NULL) {
        eventHandlers[stack->isrNumber](stack);
    }

    if(stack->isrNumber < 32) {
        uint64_t cr2;
        asm volatile ("cli\n" "mov %%cr2, %0" : "=a"(cr2)); 
        kprintVS("Congrats: you fucked up with a nice <%s> on core %d and error code %x, have fun debugging this\n", exceptionMessages[stack->isrNumber], stack->core, stack->errorCode);
        kprintVS("RAX: %a | RBX: %a | RCX: %a | RDX: %a\n", stack->rax, stack->rbx, stack->rcx, stack->rdx);
        kprintVS("RSI: %a | RDI: %a | RBP: %a | RSP: %a\n", stack->rsi, stack->rdi, stack->rbp, stack->rsp);
        kprintVS("r8:  %a | r9:  %a | r10: %a | r11: %a\n", stack->r8, stack->r9, stack->r10, stack->r11); 
        kprintVS("r12: %a | r13: %a | r14: %a | r15: %a\n", stack->r12, stack->r13, stack->r14, stack->r15); 
        kprintVS("cs:  %a | ss:  %a | cr2: %a | rip: %a\n", stack->cs, stack->ss, cr2, stack->rip);
        for(;;);
    }
}

void idtInit() {
    eventHandlers[0x69] = syscallMain;
    setIDTentry(8, 0, 0x8f, (uint64_t)isr0);
    setIDTentry(8, 1, 0x8f, (uint64_t)isr1);
    setIDTentry(8, 2, 0x8f, (uint64_t)isr2);
    setIDTentry(8, 3, 0x8f, (uint64_t)isr3);
    setIDTentry(8, 4, 0x8f, (uint64_t)isr4);
    setIDTentry(8, 5, 0x8f, (uint64_t)isr5);
    setIDTentry(8, 6, 0x8f, (uint64_t)isr6);
    setIDTentry(8, 7, 0x8f, (uint64_t)isr7);
    setIDTentry(8, 8, 0x8f, (uint64_t)errorIsr8);
    setIDTentry(8, 9, 0x8f, (uint64_t)isr9);
    setIDTentry(8, 10, 0x8f, (uint64_t)errorIsr10);
    setIDTentry(8, 11, 0x8f, (uint64_t)errorIsr11);
    setIDTentry(8, 12, 0x8f, (uint64_t)errorIsr12);
    setIDTentry(8, 13, 0x8f, (uint64_t)errorIsr13);
    setIDTentry(8, 14, 0x8f, (uint64_t)errorIsr14);
    setIDTentry(8, 15, 0x8f, (uint64_t)isr15);
    setIDTentry(8, 16, 0x8f, (uint64_t)isr16);
    setIDTentry(8, 17, 0x8f, (uint64_t)isr17);
    setIDTentry(8, 18, 0x8f, (uint64_t)isr18);
    setIDTentry(8, 19, 0x8f, (uint64_t)isr19);
    setIDTentry(8, 20, 0x8f, (uint64_t)isr20);
    setIDTentry(8, 21, 0x8f, (uint64_t)isr21);
    setIDTentry(8, 22, 0x8f, (uint64_t)isr22);
    setIDTentry(8, 23, 0x8f, (uint64_t)isr23);
    setIDTentry(8, 24, 0x8f, (uint64_t)isr24);
    setIDTentry(8, 25, 0x8f, (uint64_t)isr25);
    setIDTentry(8, 26, 0x8f, (uint64_t)isr26);
    setIDTentry(8, 27, 0x8f, (uint64_t)isr27);
    setIDTentry(8, 28, 0x8f, (uint64_t)isr28);
    setIDTentry(8, 29, 0x8f, (uint64_t)isr29);
    setIDTentry(8, 30, 0x8f, (uint64_t)isr30);
    setIDTentry(8, 31, 0x8f, (uint64_t)isr31);
    setIDTentry(8, 32, 0x8f, (uint64_t)isr32);
    setIDTentry(8, 33, 0x8f, (uint64_t)isr33);
    setIDTentry(8, 34, 0x8f, (uint64_t)isr34);
    setIDTentry(8, 35, 0x8f, (uint64_t)isr35);
    setIDTentry(8, 36, 0x8f, (uint64_t)isr36);
    setIDTentry(8, 37, 0x8f, (uint64_t)isr37);
    setIDTentry(8, 38, 0x8f, (uint64_t)isr38);
    setIDTentry(8, 39, 0x8f, (uint64_t)isr39);
    setIDTentry(8, 40, 0x8f, (uint64_t)isr40);
    setIDTentry(8, 41, 0x8f, (uint64_t)isr41);
    setIDTentry(8, 42, 0x8f, (uint64_t)isr42);
    setIDTentry(8, 43, 0x8f, (uint64_t)isr43);
    setIDTentry(8, 44, 0x8f, (uint64_t)isr44);
    setIDTentry(8, 45, 0x8f, (uint64_t)isr45);
    setIDTentry(8, 46, 0x8f, (uint64_t)isr46);
    setIDTentry(8, 47, 0x8f, (uint64_t)isr47);
    setIDTentry(8, 48, 0x8f, (uint64_t)isr48);
    setIDTentry(8, 49, 0x8f, (uint64_t)isr49);
    setIDTentry(8, 50, 0x8f, (uint64_t)isr50);
    setIDTentry(8, 51, 0x8f, (uint64_t)isr51);
    setIDTentry(8, 52, 0x8f, (uint64_t)isr52);
    setIDTentry(8, 53, 0x8f, (uint64_t)isr53);
    setIDTentry(8, 54, 0x8f, (uint64_t)isr54);
    setIDTentry(8, 55, 0x8f, (uint64_t)isr55);
    setIDTentry(8, 56, 0x8f, (uint64_t)isr56);
    setIDTentry(8, 57, 0x8f, (uint64_t)isr57);
    setIDTentry(8, 58, 0x8f, (uint64_t)isr58);
    setIDTentry(8, 59, 0x8f, (uint64_t)isr59);
    setIDTentry(8, 60, 0x8f, (uint64_t)isr60);
    setIDTentry(8, 61, 0x8f, (uint64_t)isr61);
    setIDTentry(8, 62, 0x8f, (uint64_t)isr62);
    setIDTentry(8, 63, 0x8f, (uint64_t)isr63);
    setIDTentry(8, 64, 0x8f, (uint64_t)isr64);
    setIDTentry(8, 65, 0x8f, (uint64_t)isr65);
    setIDTentry(8, 66, 0x8f, (uint64_t)isr66);
    setIDTentry(8, 67, 0x8f, (uint64_t)isr67);
    setIDTentry(8, 68, 0x8f, (uint64_t)isr68);
    setIDTentry(8, 69, 0x8f, (uint64_t)isr69);
    setIDTentry(8, 70, 0x8f, (uint64_t)isr70);
    setIDTentry(8, 71, 0x8f, (uint64_t)isr71);
    setIDTentry(8, 72, 0x8f, (uint64_t)isr72);
    setIDTentry(8, 73, 0x8f, (uint64_t)isr73);
    setIDTentry(8, 74, 0x8f, (uint64_t)isr74);
    setIDTentry(8, 75, 0x8f, (uint64_t)isr75);
    setIDTentry(8, 76, 0x8f, (uint64_t)isr76);
    setIDTentry(8, 77, 0x8f, (uint64_t)isr77);
    setIDTentry(8, 78, 0x8f, (uint64_t)isr78);
    setIDTentry(8, 79, 0x8f, (uint64_t)isr79);
    setIDTentry(8, 80, 0x8f, (uint64_t)isr80);
    setIDTentry(8, 81, 0x8f, (uint64_t)isr81);
    setIDTentry(8, 82, 0x8f, (uint64_t)isr82);
    setIDTentry(8, 83, 0x8f, (uint64_t)isr83);
    setIDTentry(8, 84, 0x8f, (uint64_t)isr84);
    setIDTentry(8, 85, 0x8f, (uint64_t)isr85);
    setIDTentry(8, 86, 0x8f, (uint64_t)isr86);
    setIDTentry(8, 87, 0x8f, (uint64_t)isr87);
    setIDTentry(8, 88, 0x8f, (uint64_t)isr88);
    setIDTentry(8, 89, 0x8f, (uint64_t)isr89);
    setIDTentry(8, 90, 0x8f, (uint64_t)isr90);
    setIDTentry(8, 91, 0x8f, (uint64_t)isr91);
    setIDTentry(8, 92, 0x8f, (uint64_t)isr92);
    setIDTentry(8, 93, 0x8f, (uint64_t)isr93);
    setIDTentry(8, 94, 0x8f, (uint64_t)isr94);
    setIDTentry(8, 95, 0x8f, (uint64_t)isr95);
    setIDTentry(8, 96, 0x8f, (uint64_t)isr96);
    setIDTentry(8, 97, 0x8f, (uint64_t)isr97);
    setIDTentry(8, 98, 0x8f, (uint64_t)isr98);
    setIDTentry(8, 99, 0x8f, (uint64_t)isr99);
    setIDTentry(8, 100, 0x8f, (uint64_t)isr100);
    setIDTentry(8, 101, 0x8f, (uint64_t)isr101);
    setIDTentry(8, 102, 0x8f, (uint64_t)isr102);
    setIDTentry(8, 103, 0x8f, (uint64_t)isr103);
    setIDTentry(8, 104, 0x8f, (uint64_t)isr104);
    setIDTentry(8, 105, 0xef, (uint64_t)isr105);
    setIDTentry(8, 106, 0x8f, (uint64_t)isr106);
    setIDTentry(8, 107, 0x8f, (uint64_t)isr107);
    setIDTentry(8, 108, 0x8f, (uint64_t)isr108);
    setIDTentry(8, 109, 0x8f, (uint64_t)isr109);
    setIDTentry(8, 110, 0x8f, (uint64_t)isr110);
    setIDTentry(8, 111, 0x8f, (uint64_t)isr111);
    setIDTentry(8, 112, 0x8f, (uint64_t)isr112);
    setIDTentry(8, 113, 0x8f, (uint64_t)isr113);
    setIDTentry(8, 114, 0x8f, (uint64_t)isr114);
    setIDTentry(8, 115, 0x8f, (uint64_t)isr115);
    setIDTentry(8, 116, 0x8f, (uint64_t)isr116);
    setIDTentry(8, 117, 0x8f, (uint64_t)isr117);
    setIDTentry(8, 118, 0x8f, (uint64_t)isr118);
    setIDTentry(8, 119, 0x8f, (uint64_t)isr119);
    setIDTentry(8, 120, 0x8f, (uint64_t)isr120);
    setIDTentry(8, 121, 0x8f, (uint64_t)isr121);
    setIDTentry(8, 122, 0x8f, (uint64_t)isr122);
    setIDTentry(8, 123, 0x8f, (uint64_t)isr123);
    setIDTentry(8, 124, 0x8f, (uint64_t)isr124);
    setIDTentry(8, 125, 0x8f, (uint64_t)isr125);
    setIDTentry(8, 126, 0x8f, (uint64_t)isr126);
    setIDTentry(8, 127, 0x8f, (uint64_t)isr127);
    setIDTentry(8, 128, 0x8f, (uint64_t)isr128);
    setIDTentry(8, 129, 0x8f, (uint64_t)isr129);
    setIDTentry(8, 130, 0x8f, (uint64_t)isr130);
    setIDTentry(8, 131, 0x8f, (uint64_t)isr131);
    setIDTentry(8, 132, 0x8f, (uint64_t)isr132);
    setIDTentry(8, 133, 0x8f, (uint64_t)isr133);
    setIDTentry(8, 134, 0x8f, (uint64_t)isr134);
    setIDTentry(8, 135, 0x8f, (uint64_t)isr135);
    setIDTentry(8, 136, 0x8f, (uint64_t)isr136);
    setIDTentry(8, 137, 0x8f, (uint64_t)isr137);
    setIDTentry(8, 138, 0x8f, (uint64_t)isr138);
    setIDTentry(8, 139, 0x8f, (uint64_t)isr139);
    setIDTentry(8, 140, 0x8f, (uint64_t)isr140);
    setIDTentry(8, 141, 0x8f, (uint64_t)isr141);
    setIDTentry(8, 142, 0x8f, (uint64_t)isr142);
    setIDTentry(8, 143, 0x8f, (uint64_t)isr143);
    setIDTentry(8, 144, 0x8f, (uint64_t)isr144);
    setIDTentry(8, 145, 0x8f, (uint64_t)isr145);
    setIDTentry(8, 146, 0x8f, (uint64_t)isr146);
    setIDTentry(8, 147, 0x8f, (uint64_t)isr147);
    setIDTentry(8, 148, 0x8f, (uint64_t)isr148);
    setIDTentry(8, 149, 0x8f, (uint64_t)isr149);
    setIDTentry(8, 150, 0x8f, (uint64_t)isr150);
    setIDTentry(8, 151, 0x8f, (uint64_t)isr151);
    setIDTentry(8, 152, 0x8f, (uint64_t)isr152);
    setIDTentry(8, 153, 0x8f, (uint64_t)isr153);
    setIDTentry(8, 154, 0x8f, (uint64_t)isr154);
    setIDTentry(8, 155, 0x8f, (uint64_t)isr155);
    setIDTentry(8, 156, 0x8f, (uint64_t)isr156);
    setIDTentry(8, 157, 0x8f, (uint64_t)isr157);
    setIDTentry(8, 158, 0x8f, (uint64_t)isr158);
    setIDTentry(8, 159, 0x8f, (uint64_t)isr159);
    setIDTentry(8, 160, 0x8f, (uint64_t)isr160);
    setIDTentry(8, 161, 0x8f, (uint64_t)isr161);
    setIDTentry(8, 162, 0x8f, (uint64_t)isr162);
    setIDTentry(8, 163, 0x8f, (uint64_t)isr163);
    setIDTentry(8, 164, 0x8f, (uint64_t)isr164);
    setIDTentry(8, 165, 0x8f, (uint64_t)isr165);
    setIDTentry(8, 166, 0x8f, (uint64_t)isr166);
    setIDTentry(8, 167, 0x8f, (uint64_t)isr167);
    setIDTentry(8, 168, 0x8f, (uint64_t)isr168);
    setIDTentry(8, 169, 0x8f, (uint64_t)isr169);
    setIDTentry(8, 170, 0x8f, (uint64_t)isr170);
    setIDTentry(8, 171, 0x8f, (uint64_t)isr171);
    setIDTentry(8, 172, 0x8f, (uint64_t)isr172);
    setIDTentry(8, 173, 0x8f, (uint64_t)isr173);
    setIDTentry(8, 174, 0x8f, (uint64_t)isr174);
    setIDTentry(8, 175, 0x8f, (uint64_t)isr175);
    setIDTentry(8, 176, 0x8f, (uint64_t)isr176);
    setIDTentry(8, 177, 0x8f, (uint64_t)isr177);
    setIDTentry(8, 178, 0x8f, (uint64_t)isr178);
    setIDTentry(8, 179, 0x8f, (uint64_t)isr179);
    setIDTentry(8, 180, 0x8f, (uint64_t)isr180);
    setIDTentry(8, 181, 0x8f, (uint64_t)isr181);
    setIDTentry(8, 182, 0x8f, (uint64_t)isr182);
    setIDTentry(8, 183, 0x8f, (uint64_t)isr183);
    setIDTentry(8, 184, 0x8f, (uint64_t)isr184);
    setIDTentry(8, 185, 0x8f, (uint64_t)isr185);
    setIDTentry(8, 186, 0x8f, (uint64_t)isr186);
    setIDTentry(8, 187, 0x8f, (uint64_t)isr187);
    setIDTentry(8, 188, 0x8f, (uint64_t)isr188);
    setIDTentry(8, 189, 0x8f, (uint64_t)isr189);
    setIDTentry(8, 190, 0x8f, (uint64_t)isr190);
    setIDTentry(8, 191, 0x8f, (uint64_t)isr191);
    setIDTentry(8, 192, 0x8f, (uint64_t)isr192);
    setIDTentry(8, 193, 0x8f, (uint64_t)isr193);
    setIDTentry(8, 194, 0x8f, (uint64_t)isr194);
    setIDTentry(8, 195, 0x8f, (uint64_t)isr195);
    setIDTentry(8, 196, 0x8f, (uint64_t)isr196);
    setIDTentry(8, 197, 0x8f, (uint64_t)isr197);
    setIDTentry(8, 198, 0x8f, (uint64_t)isr198);
    setIDTentry(8, 199, 0x8f, (uint64_t)isr199);
    setIDTentry(8, 200, 0x8f, (uint64_t)isr200);
    setIDTentry(8, 201, 0x8f, (uint64_t)isr201);
    setIDTentry(8, 202, 0x8f, (uint64_t)isr202);
    setIDTentry(8, 203, 0x8f, (uint64_t)isr203);
    setIDTentry(8, 204, 0x8f, (uint64_t)isr204);
    setIDTentry(8, 205, 0x8f, (uint64_t)isr205);
    setIDTentry(8, 206, 0x8f, (uint64_t)isr206);
    setIDTentry(8, 207, 0x8f, (uint64_t)isr207);
    setIDTentry(8, 208, 0x8f, (uint64_t)isr208);
    setIDTentry(8, 209, 0x8f, (uint64_t)isr209);
    setIDTentry(8, 210, 0x8f, (uint64_t)isr210);
    setIDTentry(8, 211, 0x8f, (uint64_t)isr211);
    setIDTentry(8, 212, 0x8f, (uint64_t)isr212);
    setIDTentry(8, 213, 0x8f, (uint64_t)isr213);
    setIDTentry(8, 214, 0x8f, (uint64_t)isr214);
    setIDTentry(8, 215, 0x8f, (uint64_t)isr215);
    setIDTentry(8, 216, 0x8f, (uint64_t)isr216);
    setIDTentry(8, 217, 0x8f, (uint64_t)isr217);
    setIDTentry(8, 218, 0x8f, (uint64_t)isr218);
    setIDTentry(8, 219, 0x8f, (uint64_t)isr219);
    setIDTentry(8, 220, 0x8f, (uint64_t)isr220);
    setIDTentry(8, 221, 0x8f, (uint64_t)isr221);
    setIDTentry(8, 222, 0x8f, (uint64_t)isr222);
    setIDTentry(8, 223, 0x8f, (uint64_t)isr223);
    setIDTentry(8, 224, 0x8f, (uint64_t)isr224);
    setIDTentry(8, 225, 0x8f, (uint64_t)isr225);
    setIDTentry(8, 226, 0x8f, (uint64_t)isr226);
    setIDTentry(8, 227, 0x8f, (uint64_t)isr227);
    setIDTentry(8, 228, 0x8f, (uint64_t)isr228);
    setIDTentry(8, 229, 0x8f, (uint64_t)isr229);
    setIDTentry(8, 230, 0x8f, (uint64_t)isr230);
    setIDTentry(8, 231, 0x8f, (uint64_t)isr231);
    setIDTentry(8, 232, 0x8f, (uint64_t)isr232);
    setIDTentry(8, 233, 0x8f, (uint64_t)isr233);
    setIDTentry(8, 234, 0x8f, (uint64_t)isr234);
    setIDTentry(8, 235, 0x8f, (uint64_t)isr235);
    setIDTentry(8, 236, 0x8f, (uint64_t)isr236);
    setIDTentry(8, 237, 0x8f, (uint64_t)isr237);
    setIDTentry(8, 238, 0x8f, (uint64_t)isr238);
    setIDTentry(8, 239, 0x8f, (uint64_t)isr239);
    setIDTentry(8, 240, 0x8f, (uint64_t)isr240);
    setIDTentry(8, 241, 0x8f, (uint64_t)isr241);
    setIDTentry(8, 242, 0x8f, (uint64_t)isr242);
    setIDTentry(8, 243, 0x8f, (uint64_t)isr243);
    setIDTentry(8, 244, 0x8f, (uint64_t)isr244);
    setIDTentry(8, 245, 0x8f, (uint64_t)isr245);
    setIDTentry(8, 246, 0x8f, (uint64_t)isr246);
    setIDTentry(8, 247, 0x8f, (uint64_t)isr247);
    setIDTentry(8, 248, 0x8f, (uint64_t)isr248);
    setIDTentry(8, 249, 0x8f, (uint64_t)isr249);
    setIDTentry(8, 250, 0x8f, (uint64_t)isr250);
    setIDTentry(8, 251, 0x8f, (uint64_t)isr251);
    setIDTentry(8, 252, 0x8f, (uint64_t)isr252);
    setIDTentry(8, 253, 0x8f, (uint64_t)isr253);
    setIDTentry(8, 254, 0x8f, (uint64_t)isr254);

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
                                    "Deadlock",
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
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        schedulerMain, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
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
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
                                        NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
                                    };
