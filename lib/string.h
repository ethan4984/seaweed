#ifndef STRINGUTILS_H
#define STRINGUTILS_H

#include <stdint.h>
#include <stddef.h>

char *strcpy(char *dest, const char *src);

int strcmp(const char *a, const char *b);

int strncmp(const char *str1, const char *str2, uint64_t n);

uint64_t strlen(const char *str);

char *itob(uint64_t num, int base);

#endif
