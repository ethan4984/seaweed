#ifndef TSS_H
#define TSS_H

typedef struct {
    uint32_t reserved;
    uint32_t rsp0Low;
    uint32_t rsp0High;
    uint32_t rsp1Low;
    uint32_t rsp1High;
    uint32_t rsp2Low;
    uint32_t rsp2High;
    uint64_t reserved1;
    uint32_t ist1Low;
    uint32_t ist1High;
    uint32_t ist2Low;
    uint32_t ist2High;
    uint32_t ist3Low;
    uint32_t ist3High;
    uint32_t ist4Low;
    uint32_t ist4High;
    uint32_t ist5Low;
    uint32_t ist5High;
    uint32_t ist6Low;
    uint32_t ist6High;
    uint32_t ist7Low;
    uint32_t ist7High;
    uint64_t reserved3;
    uint32_t reserved4;
} __attribute__((packed)) tss_t;

#endif
