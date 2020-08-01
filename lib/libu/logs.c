#include <libu/syscalls.h>
#include <libk/string.h>
#include <libu/logs.h>

#include <stdint.h>
#include <stdarg.h>

void printf(const char *str, ...) { 
    static char lock = 0;
    spinLock(&lock);

    uint64_t hold = 0;
    char *string;
    char character;

    va_list arg;
    va_start(arg, str);

    for(uint64_t i = 0; i < strlen(str); i++) {
        if(str[i] != '%') { 
            printSerial(str[i]);
        } else {
            i++;
            switch(str[i]) {
                case 'd':
                    hold = va_arg(arg, long);
                    string = itob(hold, 10);
                    for(size_t i = 0; i < strlen(string); i++)
                        printSerial(string[i]);
                    break;
                case 's':
                    string = va_arg(arg, char*);
                    for(size_t i = 0; i < strlen(string); i++)
                        printSerial(string[i]);
                    break;
                case 'c':
                    character = va_arg(arg, int);
                    printSerial(character);
                    break; 
                case 'x':
                    hold = va_arg(arg, uint64_t);
                    string = itob(hold, 16);
                    for(size_t i = 0; i < strlen(string); i++)
                        printSerial(string[i]);
                    break;
                case 'a':
                    hold = va_arg(arg, uint64_t);
                    string = itob(hold, 16);
                    int offset_zeros = 16 - strlen(string);
                    for(int i = 0; i < offset_zeros; i++)
                        printSerial('0');
                    for(size_t i = 0; i < strlen(string); i++)
                        printSerial(string[i]);
                    break; 
            }
        }
    }
    va_end(arg);

    printSerial('\n');

    spinRelease(&lock);
}
