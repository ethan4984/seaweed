#include <lib/output.h>
#include <lib/asmUtils.h>
#include <lib/string.h>

#include <stdarg.h>

void kprintDS(const char *str, ...) { // debug serial
    uint64_t hold = 0;
    char *string;
    char character;

    va_list arg;
    va_start(arg, str);

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
            }
        }
    }
    va_end(arg);
    serialWrite('\n'); 
}


