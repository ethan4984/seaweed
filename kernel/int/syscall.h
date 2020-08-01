#ifndef SYSCALL_H
#define SYSCALL_H

#include <libk/asmUtils.h>

enum {
    OUTPUT,
    INPUT,
    CALLS
};

enum {
    SERIAL,
    VIDEO
};

enum {
    PTHREAD
};

void syscallMain(regs_t *regs);

#endif
