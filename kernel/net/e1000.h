#ifndef E1000_H
#define E1000_H

#include <stdint.h>
#include <stdbool.h> 

typedef struct {
    uint8_t mac[6];
    uint64_t regBase;
} NICinfo_t;

void initEthernet();

#endif
