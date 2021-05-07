SECTIONS
{
	. = 0x0; /* armstub loading address */
	.text : { *(.text); *(.text.*); *(.gnu.linkonce.t*); }
	.rodata : { *(.rodata); *(.rodata.*) *(.gnu.linkonce.r*); }
	.data : { *(.data); *(.data.*); *(.gnu.linkonce.d*); }
	.bss (NOLOAD) : { . = ALIGN(16); __bss_start = .; *(.bss); *(.bss.*); *(COMMON); __bss_end = .; }
	__bss_size = (__bss_end - __bss_start) >> 3;
	_end = .;
	/DISCARD/ : { *(.comment); *(.gnu*); *(.note*); *(.eh_frame*); }
}