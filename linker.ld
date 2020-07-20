OUTPUT_FORMAT(binary)
ENTRY(bootMain)

SECTIONS
{
    . = 0xfc00;

    .init : {
        *(.init)
    }

    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        bssBegin = .;
        *(COMMON)
        *(.bss*)
        bssEnd = .;
    }
}
