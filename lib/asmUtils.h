#ifndef ASMUTILS_H
#define ASMUTILS_H

#include <stdint.h>

#define COM1 0x3f8 
#define COM2 0x2f8
#define COM3 0x3e8
#define COM4 0x2e8

uint8_t serialRead();

void serialWrite(uint8_t data);

void outb(uint16_t port, uint8_t data);

void outw(uint16_t port, uint16_t data);

void outd(uint16_t port, uint32_t data);

uint8_t inb(uint16_t port);

uint16_t inw(uint16_t port);

uint32_t ind(uint16_t port);

#endif
