#include <kernel/int/idt.h>

idtEntry_t idt[256];

void setIDTentry(uint8_t codeSelector, uint8_t index, uint8_t typesAndAttributes, uint64_t offset) {
    idt[index].offsetLow = (uint16_t)(offset >> 0);
    idt[index].selector = codeSelector;
    idt[index].zero8 = 0;
    idt[index].attributes = typesAndAttributes;
    idt[index].offsetMid = (uint16_t)(offset >> 16);
    idt[index].offsetHigh = (uint32_t)(offset >> 32);
    idt[index].zero32 = 0;
}
