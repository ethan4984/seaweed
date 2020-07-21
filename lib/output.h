#ifndef OUTPUT_H
#define OUTPUT_H

#include <stdint.h>

#define bashDefault \e[39m
#define bashBlack \e[30m
#define bashRed \e[31m
#define bashGreen \e[32m
#define bashYellow \e[33m
#define bashBlue \e[34m
#define bashMegenta \e[35m
#define bashCyan \e[36m
#define bashLightGray \e[37m
#define bashDarkGray \e[90m
#define bashLightRed \e[91m
#define bashLightGreen \e[92m
#define bashLightYellow \e[93m
#define bashLightBlue \e[94m
#define bashLightMagenta \e[95m
#define bashLightCyan \e[96m
#define bashLightWhite \e[97m

enum {
    DEFAULT,
    BLACK,
    RED,
    GREEN,
    YELLOW,
    BLUE,
    MAGENTA,
    CYAN,
    LIGHTGRAY,
    RIGHTGRAY,
    LIGHTRED,
    LIGHTGREEN,
    LIGHTYELLOW,
    LIGHTBLUE,
    LIGHTMAGENTA,
    LIGHTCYAN,
    WHITE
};

void kprintDS(const char *prefix, const char *str, ...);

#endif
