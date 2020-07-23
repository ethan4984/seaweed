#include <lib/output.h>
#include <lib/asmUtils.h>
#include <lib/string.h>

#include <stdarg.h>

typedef struct {
    const char *prefix;
    uint8_t prefixColour;
    uint8_t textColour;
} prefixList_t;

prefixList_t prefixList[] = {   { "[KDEBUG]", GREEN, YELLOW },
                                { "[KMM]", GREEN, LIGHTRED },
                                { "[ACPI]", MAGENTA, CYAN }
                            };
                            
const char *bashColours[] = {   "\e[39m", "\e[30m", "\e[31m", "\e[32m",
                                "\e[33m", "\e[34m", "\e[35m", "\e[36m",
                                "\e[37m", "\e[90m", "\e[91m", "\e[92m",
                                "\e[93m", "\e[94m", "\e[95m", "\e[96m",
                                "\e[97m"
                            };

static void serialWriteString(const char *str);

void kprintDS(const char *prefix, const char *str, ...) { // debug serial
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
            }
        }
    }
    va_end(arg);
    serialWrite('\n'); 
}

static void serialWriteString(const char *str) {
    for(uint64_t i = 0; i < strlen(str); i++) {
        serialWrite(str[i]);
    }
}
