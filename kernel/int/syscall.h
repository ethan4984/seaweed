#ifndef SYSCALL_H
#define SYSCALL_H

#include <libk/asmUtils.h>

enum {
    OUTPUT,
    INPUT
};

enum {
    SERIAL,
    VIDEO
};

void syscallMain(regs_t *regs);

#endif
