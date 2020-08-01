#include <kernel/sched/scheduler.h>
#include <kernel/int/syscall.h>
#include <libk/asmUtils.h>
#include <libk/output.h>

void syscallMain(regs_t *regs) {
    static char lock = 0;
    spinLock(&lock);

    switch(regs->rax) { // rax = type
        case OUTPUT:
            switch(regs->rcx) {
                case SERIAL:
                    serialWrite((char)regs->rdx);
                    break;
                case VIDEO:
                    break;
            }
            break;
        case INPUT:
            break;
        case CALLS:
            switch(regs->rcx) {
                case PTHREAD:
                    createThread(regs->rdx, regs->rbx); 
                    break;
            }            
    }

    spinRelease(&lock);
}
