ENTRY(_boot)

SECTIONS
{
    . = 0x80000; /*KERNEL_START_ADDRESS*/
    .text : { *(.text*); }
    .bss : { . = ALIGN(16); __bss_start = .; *(.bss*); *(COMMON) __bss_end = .; }
    __bss_size = (__bss_end - __bss_start) >> 3;
    .rodata : { *(.rodata*); }
    .data : { *(.data*); }
    /DISCARD/ : { *(.comment) *(.eh_frame); }
}