OUTPUT_FORMAT(binary)
ENTRY(bootMain)

SECTIONS
{
    . = 0xffffffff80100000;

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
