#include <kernel/sched/scheduler.h>
#include <kernel/int/syscall.h>
#include <libk/asmUtils.h>
#include <libk/output.h>

void syscallMain(regs_t *regs) {
    static char lock = 0;
    spinLock(&lock);
    
    kprintDS("[KDEBUG]", "Hi syscall, nice rax btw %x", regs->rax);

    spinRelease(&lock);
}
