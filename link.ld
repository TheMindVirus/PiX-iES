ENTRY(main2)
/*PHDRS
{
    null PT_NULL AT(0) FLAGS(0); *//* No way to specify alignment? *//*
    text PT_LOAD FILEHDR PHDRS AT(0) FLAGS(5); *//* -nmagic *//*
}*/
MEMORY /* PSP3000 Specific */
{
           Zero (r)   : ORIGIN = 0x00000000, LENGTH = 0x0000FFFF
     Scratchpad (rw)  : ORIGIN = 0x00010000, LENGTH = 0x00003FFF
    Framebuffer (rw)  : ORIGIN = 0x04000000, LENGTH = 0x001FFFFF
           User (rwx) : ORIGIN = 0x08804000, LENGTH = 0x037FBFFF
     Peripheral (rwx) : ORIGIN = 0x1C000000, LENGTH = 0x0FFFFFFF
         Kernel (rx)  : ORIGIN = 0x88000000, LENGTH = 0x03FFFFFF
}
SECTIONS
{
    . = ORIGIN(User);
    .text . : ALIGN(0) SUBALIGN(0) { *(.text) *(.text.*) } >User /* :text */
    .rodata.sceModuleInfo : { *(.rodata.sceModuleInfo) } >User
    .rodata . : { *(.rodata) *(.rodata.*) } >User
    __executable_start = 0x0;
    _gp = ALIGN(16) + 0x7FF0;
    _lib_ent_bottom = .;
    /DISCARD/ : { *(.comment) *(.pdr) *(.note.GNU-stack) }
    /DISCARD/ : { *(.MIPS.abiflags) *(.reginfo) *(.gnu.attributes) }
    /DISCARD/ : { *(.rodata.sceModuleInfo) *(.strtab) }
}
