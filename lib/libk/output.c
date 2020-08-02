#include <kernel/sched/scheduler.h>
#include <kernel/drivers/vesa.h>
#include <kernel/mm/kHeap.h>
#include <libk/asmUtils.h>
#include <libk/output.h>
#include <libk/string.h>

#include <stdarg.h>
#include <stdbool.h>

typedef struct {
    const char *prefix;
    uint8_t prefixColour;
    uint8_t textColour;
} prefixList_t;

prefixList_t prefixList[] = {   { "[KDEBUG]", GREEN, YELLOW },
                                { "[KMM]", GREEN, LIGHTRED },
                                { "[ACPI]", MAGENTA, CYAN },
                                { "[APIC]", RED, GREEN },
                                { "[SMP]", YELLOW, LIGHTBLUE },
                                { "[PCI]", BLUE, LIGHTGREEN }
                            };
                            
const char *bashColours[] = {   "\e[39m", "\e[30m", "\e[31m", "\e[32m",
                                "\e[33m", "\e[34m", "\e[35m", "\e[36m",
                                "\e[37m", "\e[90m", "\e[91m", "\e[92m",
                                "\e[93m", "\e[94m", "\e[95m", "\e[96m",
                                "\e[97m"
                            };

uint64_t terminalRow, terminalColumn;
uint32_t terminalBG, terminalFG, height, width;
uint32_t *referenceColumn;

static void serialWriteString(const char *str);
static bool endOfScreenV();
static bool endOfScreenH();

void kprintDS(const char *prefix, const char *str, ...) { // debug serial
    static char lock = 0;
    spinLock(&lock);

    uint64_t hold = 0;
    char *string;
    char character;

    va_list arg;
    va_start(arg, str);

    for(uint64_t i = 0; i < sizeof(prefixList) / sizeof(prefixList_t); i++) {
        if(strcmp(prefixList[i].prefix, prefix) == 0) {
            serialWriteString(bashColours[prefixList[i].prefixColour]);
            serialWriteString(prefix);
            serialWriteString(bashColours[prefixList[i].textColour]);
            serialWrite(' ');
            break;
        }

        if(i == sizeof(prefixList) / sizeof(prefixList_t) - 1) {
            serialWriteString(bashColours[DEFAULT]);
        }
    }


    for(uint64_t i = 0; i < strlen(str); i++) {
        if(str[i] != '%')
            serialWrite(str[i]);
        else {
            i++;
            switch(str[i]) {
                case 'd':
                    hold = va_arg(arg, long);
                    string = itob(hold, 10);
                    for(size_t i = 0; i < strlen(string); i++)
                        serialWrite(string[i]);
                    break;
                case 's':
                    string = va_arg(arg, char*);
                    for(size_t i = 0; i < strlen(string); i++)
                        serialWrite(string[i]);
                    break;
                case 'c':
                    character = va_arg(arg, int);
                    serialWrite(character);
                    break; 
                case 'x':
                    hold = va_arg(arg, uint64_t);
                    string = itob(hold, 16);
                    for(size_t i = 0; i < strlen(string); i++)
                        serialWrite(string[i]);
                    break;
                case 'a':
                    hold = va_arg(arg, uint64_t);
                    string = itob(hold, 16);
                    int offset_zeros = 16 - strlen(string);
                    for(int i = 0; i < offset_zeros; i++)
                        serialWrite('0');
                    for(size_t i = 0; i < strlen(string); i++)
                        serialWrite(string[i]);
                    break;
            }
        }
    }
    va_end(arg);
    serialWrite('\n'); 
    
    spinRelease(&lock);
}

void initVESAtext(uint32_t fg, uint32_t bg, uint64_t x, uint64_t y) { 
    terminalBG = bg;
    terminalFG = fg;

    height = y;
    width = x;

    referenceColumn = kmalloc(0x2000);
}

void putchar(char c) {
    static bool isBack = false;
    switch(c) {
        case '\n':
            referenceColumn[terminalRow] = terminalColumn;
            terminalRow = 0;
            terminalColumn += 8;
            break;
        case '\t':
            for(int i = 0; i < TABSIZE; i++)
                putchar(' ');
            break;
        case '\b':
            isBack = true;
            if(terminalRow == 0) {
                if(terminalColumn == 0)
                    break;
                terminalColumn -= 8;
                terminalRow = width;
                terminalRow -= 8;
                putchar(' ');
                terminalRow -= 8;
                break;
            }
            terminalRow -= 8;
            putchar(' ');
            terminalRow -= 8;
            isBack = false;
            break;
        default:
            renderChar(terminalRow, terminalColumn, terminalFG, terminalBG, c);
            terminalRow += 8;
            if(terminalRow == width && !(isBack)) {
                terminalRow = 0;
                terminalColumn += 8;
                if(terminalColumn == height)
                    terminalColumn = 0;
            }
            break;
    }
}

void kprintVS(const char *str, ...) {
    static char lock = 0; 
    spinLock((char*)&lock);

    uint64_t hold = 0;
    char *string;
    char character;

    va_list arg;
    va_start(arg, str);

    for(uint64_t i = 0; i < strlen(str); i++) {
        if(str[i] != '%')
            putchar(str[i]);
        else {
            i++;
            switch(str[i]) {
                case 'd':
                    hold = va_arg(arg, long);
                    string = itob(hold, 10);
                    for(size_t i = 0; i < strlen(string); i++)
                        putchar(string[i]);
                    break;
                case 's':
                    string = va_arg(arg, char*);
                    for(size_t i = 0; i < strlen(string); i++)
                        putchar(string[i]);
                    break;
                case 'c':
                    character = va_arg(arg, int);
                    putchar(character);
                    break; 
                case 'x':
                    hold = va_arg(arg, uint64_t);
                    string = itob(hold, 16);
                    for(size_t i = 0; i < strlen(string); i++)
                        putchar(string[i]);
                    break;
                case 'a':
                    hold = va_arg(arg, uint64_t);
                    string = itob(hold, 16);
                    int offset_zeros = 16 - strlen(string);
                    for(int i = 0; i < offset_zeros; i++)
                        putchar('0');
                    for(size_t i = 0; i < strlen(string); i++)
                        putchar(string[i]);
                    break;
            }
        }
    }    
    va_end(arg);
    spinRelease((char*)&lock);
}

static void serialWriteString(const char *str) {
    for(uint64_t i = 0; i < strlen(str); i++) {
        serialWrite(str[i]);
    }
}
