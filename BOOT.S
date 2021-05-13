#include "boot.h"
.section .text

.global _reset
.global _vector
.global _start
.global _halt

.global _gpio_setup
.global _uart_setup
.global _uart_write_byte

.org 0x0

_reset:
	.org 0x0
	bl	_start

.org 0x800

_vector:
	//Current Exception Level and Stack Pointer Zero
	.org 0x800			//Synchronous Exception (SyncEx)
	bl	interrupt_synch
	eret
	.org 0x880			//Interrupt Request Queue (IRQ)
	bl	interrupt_request
	eret
	.org 0x900			//Fast Interrupt Queue (FIQ)
	bl	interrupt_fast
	eret
	.org 0x980			//System Error (SError)
	bl	interrupt_system
	eret

	//Current Exception Level and Current Stack Pointer
	.org 0xA00			//Synchronous Exception (SyncEx)
	bl	interrupt_synch
	eret
	.org 0xA80			//Interrupt Request Queue (IRQ)
	bl	interrupt_request
	eret
	.org 0xB00			//Fast Interrupt Queue (FIQ)
	bl	interrupt_fast
	eret
	.org 0xB80			//System Error (SError)
	bl	interrupt_system
	eret

	//Lower Exception Level (Aarch64)
	.org 0xC00			//Synchronous Exception (SyncEx)
	bl	interrupt_synch
	eret
	.org 0xC80			//Interrupt Request Queue (IRQ)
	bl	interrupt_request
	eret
	.org 0xD00			//Fast Interrupt Queue (FIQ)
	bl	interrupt_fast
	eret
	.org 0xD80			//System Error (SError)
	bl	interrupt_system
	eret

	//Lower Exception Level (Aarch32)
	.org 0xE00			//Synchronous Exception (SyncEx)
	bl	interrupt_synch
	eret
	.org 0xE80			//Interrupt Request Queue (IRQ)
	bl	interrupt_request
	eret
	.org 0xF00			//Fast Interrupt Queue (FIQ)
	bl	interrupt_fast
	eret
	.org 0xF80			//System Error (SError)
	bl	interrupt_system
	eret

.org 0x80000

_start:
	mrs	x5, CurrentEL		//Load Vector Table
	ldr	x7, =_vector
	msr	vbar_el1, x7
	msr	vbar_el2, x7
	msr	vbar_el3, x7
	msr	daifset, #0b1100
	msr	daifclr, #0b0011

	mov	x0, #0x33ff		//Enable FPU/SIMD
	msr	CPTR_EL3, x0

	mrs	x1, mpidr_el1		//Processor ID Check
	and	x1, x1, #3
	cbz	x1, 1f
	bl	_halt			//Not Core 0 - Halt

1:	ldr	x1, =_start		//Core 0 - Continue Booting
	mov	sp, x1			//Move Stack to the end after all labels

	ldr	x1, =__bss_start	//Clean the BSS Section
	ldr	w2, =__bss_size
2:	cbz	w2, 3f			//Quit Loop If Zero
	str	xzr, [x1]
	add	x1, x1, #16
	sub	w2, w2, #1
	cbnz	w2, 2b			//Loop If Non-Zero

3:	bl	_gpio_setup		//Hardware Initialisation
	bl	_uart_setup

	mrs	x6, CurrentEL		//Write Current Exception Levels
	mov	x6, x6, lsr #2
	add	x6, x6, #0x30
	
	mov	x5, x5, lsr #2
	add	x5, x5, #0x30

	mov	w0, #0x45
	bl	_uart_write_byte
	mov	w0, #0x4C
	bl	_uart_write_byte
	mov	w0, w5
	bl	_uart_write_byte
	mov	w0, #0x0D
	bl	_uart_write_byte
	mov	w0, #0x0A
	bl	_uart_write_byte

	mov	w0, #0x45
	bl	_uart_write_byte
	mov	w0, #0x4C
	bl	_uart_write_byte
	mov	w0, w6
	bl	_uart_write_byte
	mov	w0, #0x0D
	bl	_uart_write_byte
	mov	w0, #0x0A
	bl	_uart_write_byte

	bl	main			//Branch to main() in C/C++

	mov	w0, #0x41		//Write ARM64\r\n to the Serial Port
	bl	_uart_write_byte
	mov	w0, #0x52
	bl	_uart_write_byte
	mov	w0, #0x4D
	bl	_uart_write_byte
	mov	w0, #0x36
	bl	_uart_write_byte
	mov	w0, #0x34
	bl	_uart_write_byte
	mov	w0, #0x0D
	bl	_uart_write_byte
	mov	w0, #0x0A
	bl	_uart_write_byte

	bl	main			//Branch to main() in C/C++

	bl	_halt			//main() returned - Halt

_gpio_setup:
	mov	x1, #0x0004		//GPFSEL1
	movk	x1, #0xFE20, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	ldr	w3, [x1]
	mov	w2, #0xFFFC
	movk	w2, #0x0FFF, lsl #16
	and	w3, w3, w2
	mov	w2, #0x2000		//Pins 14 & 15 in ALT5
	movk	w2, #0x0001, lsl #16
	orr	w3, w3, w2
	str	w2, [x1]

	mov	x1, #0x00E4		//GPIO_PUP_PDN_CNTRL_REG0
	movk	x1, #0xFE20, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	mov	w2, #0xFFFF		//Bits 28-31 in PULL_NONE
	movk	w2, #0x0FFF, lsl #16
	ldr	w3, [x1]
	and	w2, w3, w2
	str	w2, [x1]

	ret

_uart_setup:
	mov	x1, #0x5004		//AUX_ENABLES
	movk	x1, #0xFE21, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	mov	w2, #0x0001
	movk	w2, #0x0000, lsl #16
	str	w2, [x1]

	mov	x1, #0x5060		//AUX_MU_CNTL_REG
	movk	x1, #0xFE21, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	mov	w2, #0x0000
	movk	w2, #0x0000, lsl #16
	str	w2, [x1]

	mov	x1, #0x504C		//AUX_MU_LCR_REG
	movk	x1, #0xFE21, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	mov	w2, #0x0003		//<--potentially read only
	movk	w2, #0x0000, lsl #16
	str	w2, [x1]

	mov	x1, #0x5048		//AUX_MU_IIR_REG
	movk	x1, #0xFE21, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	mov	w2, #0x00C6		//<--potentially read only
	movk	w2, #0x0000, lsl #16
	str	w2, [x1]

	mov	x1, #0x5068		//AUX_MU_BAUD_REG
	movk	x1, #0xFE21, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	mov	w2, #0x021D		//hex((500000000 / (115200 * 8)) - 1) //8x Oversampling
	movk	w2, #0x0000, lsl #16
	str	w2, [x1]

	mov	x1, #0x5060		//AUX_MU_CNTL_REG
	movk	x1, #0xFE21, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	mov	w2, #0x0003
	movk	w2, #0x0000, lsl #16
	str	w2, [x1]

	ret

_uart_write_byte:
1:	mov	x1, #0x5054		//Wait for UART to be idle
	movk	x1, #0xFE21, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	ldr	w2, [x1]
	and	w2, w2, #0x20
	cbz	w2, 1b

	mov	x1, #0x5040		//Write 1 byte
	movk	x1, #0xFE21, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	str	w0, [x1]

	ret

_halt:
1:	wfe				//Infinite Event Loop
	b	1b
	ret