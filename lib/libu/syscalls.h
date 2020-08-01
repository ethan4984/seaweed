#ifndef SYSCALLS_H
#define SYSCALLS_H

#include <stdint.h>

extern void printSerial(uint64_t character);

extern void pthread(uint64_t taskIndex, uint64_t entryPoint);


#endif
