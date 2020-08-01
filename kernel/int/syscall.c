#include <kernel/sched/scheduler.h>
#include <kernel/int/syscall.h>
#include <libk/asmUtils.h>
#include <libk/output.h>

void syscallMain(regs_t *regs) {
    static char lock = 0;
    spinLock(&lock);

//    kprintDS("[KDEBUG]", "Here lel");

    switch(regs->rax) { // rax = type
        case OUTPUT:
            switch(regs->rcx) {
                case SERIAL:
 //                   kprintDS("[KDEBUG]", "Trying to print %d", (char)regs->rdx);
                    serialWrite((char)regs->rdx);
                    break;
                case VIDEO:
                    break;
            }
            break;
        case INPUT:
            break;
    }

    spinRelease(&lock);
}
