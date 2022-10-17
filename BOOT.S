.section ".text.boot"
.global _reset
.global _vector
.global _start

.global _uart_setup //Nested Calls will not work automatically
.global _uart_flush
.global _uart_newline
.global _uart_write_byte

.global _interrupt
.global _event
.global _halt

.org 0x0

_reset:
	.org 0x0
	bl	_start

.org 0x800

_vector: //Or are there just 4 interrupt tables because there's 4 cores?
	//Current Exception Level and Stack Pointer Zero
	.org 0x800			//Synchronous Exception (SyncEx)
	bl	_interrupt
	eret
	.org 0x880			//Interrupt Request Queue (IRQ)
	bl	_interrupt
	eret
	.org 0x900			//Fast Interrupt Queue (FIQ)
	bl	_interrupt
	eret
	.org 0x980			//System Error (SError)
	bl	_interrupt
	eret

	//Current Exception Level and Current Stack Pointer
	.org 0xA00			//Synchronous Exception (SyncEx)
	bl	_interrupt
	eret
	.org 0xA80			//Interrupt Request Queue (IRQ)
	bl	_interrupt
	eret
	.org 0xB00			//Fast Interrupt Queue (FIQ)
	bl	_interrupt
	eret
	.org 0xB80			//System Error (SError)
	bl	_interrupt
	eret

	//Lower Exception Level (Aarch64)
	.org 0xC00			//Synchronous Exception (SyncEx)
	bl	_interrupt
	eret
	.org 0xC80			//Interrupt Request Queue (IRQ)
	bl	_interrupt
	eret
	.org 0xD00			//Fast Interrupt Queue (FIQ)
	bl	_interrupt
	eret
	.org 0xD80			//System Error (SError)
	bl	_interrupt
	eret

	//Lower Exception Level (Aarch32)
	.org 0xE00			//Synchronous Exception (SyncEx)
	bl	_interrupt
	eret
	.org 0xE80			//Interrupt Request Queue (IRQ)
	bl	_interrupt
	eret
	.org 0xF00			//Fast Interrupt Queue (FIQ)
	bl	_interrupt
	eret
	.org 0xF80			//System Error (SError)
	bl	_interrupt
	eret

//.org 0x8000 //Kernel overwrites 0x80000, Vector Table overwrites 0x800, 0x8000 uses more space

_start:
	mrs	x5, CurrentEL		//Load Vector Table
	ldr	x7, =_vector
	msr	vbar_el1, x7
	msr	vbar_el2, x7
	msr	vbar_el3, x7
	msr	daifset, #0b1100        //Set Interrupt Flags
	msr	daifclr, #0b0011

	mov	x0, #0x33ff		//Enable FPU/SIMD
	msr	CPTR_EL3, x0
	mov 	x6, #0			//Communally Initialise Register

	mrs	x1, mpidr_el1		//Processor ID Check
	and	x1, x1, #3
	cbnz	x1, 0f			//Not Core 0 - Skip to 0:

1:	ldr	x1, =_start		//Core 0 - Continue Booting
	mov	sp, x1			//Move Stack to the end after all labels

	ldr	x1, =__bss_start	//Clean the BSS Section
	ldr	w2, =__bss_size
2:	cbz	w2, 3f			//Quit Loop If Zero
	str	xzr, [x1]
	add	x1, x1, #8              //#16
	sub	w2, w2, #1
	cbnz	w2, 2b			//Loop If Non-Zero

3:	bl	_uart_setup             //Hardware Initialisation
	bl	_uart_flush
	bl	_uart_newline

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
	bl	_uart_newline

	mov	w0, #0x45
	bl	_uart_write_byte
	mov	w0, #0x4C
	bl	_uart_write_byte
	mov	w0, w6
	bl	_uart_write_byte
	bl	_uart_newline

	mov 	x6, #0
	mov 	x4, #0
	b	5f			//Core 0 - Skip to 5:

        //BEGIN SYMMETRIC MULTIPROCESSING TEST

0:	wfe				//Wait for Event

4:	mov	w0, #0x5B		//Write "[AMAC]" to the Serial Port Symmetrically
	bl	_uart_write_byte
	mov	w0, #0x41
	bl	_uart_write_byte
	mov	w0, #0x4D
	bl	_uart_write_byte
	mov	w0, #0x41
	bl	_uart_write_byte
	mov	w0, #0x43
	bl	_uart_write_byte
	mov	w0, #0x5D
	bl	_uart_write_byte
	bl	_uart_newline

	add	x6, x6, #1		//Communally Increment Separate Registers
	cmp	x6, #3
	bne	5f
	mov	x4, #1

5:	mrs	x1, mpidr_el1		//Processor ID Check
	and	x1, x1, #3
	and	x6, x6, #3
	cmp	x1, x6
	bne	0b
	add	x5, x1, #0x30

	mov	w0, #0x5B		//Write "[CORE]: #" to the Serial Port Individually
	bl	_uart_write_byte
	mov	w0, #0x43
	bl	_uart_write_byte
	mov	w0, #0x4F
	bl	_uart_write_byte
	mov	w0, #0x52
	bl	_uart_write_byte
	mov	w0, #0x45
	bl	_uart_write_byte
	mov	w0, #0x5D
	bl	_uart_write_byte
	mov	w0, #0x3A
	bl	_uart_write_byte
	mov	w0, #0x20
	bl	_uart_write_byte
	mov	w0, w5
	bl	_uart_write_byte
	bl	_uart_newline
	
	cmp	x4, #1
	beq	7f

6:	sev				//Wake Up All Other Cores
	b	0b			//Go back to Wait For Event

        //END SYMMETRIC MULTIPROCESSING TEST

7:	//bl	main			//Optionally Branch to main() in C

	mov	w0, #0x41		//Write "ARM64" to the Serial Port
	bl	_uart_write_byte
	mov	w0, #0x52
	bl	_uart_write_byte
	mov	w0, #0x4D
	bl	_uart_write_byte
	mov	w0, #0x36
	bl	_uart_write_byte
	mov	w0, #0x34
	bl	_uart_write_byte
	bl	_uart_newline

	svc	#9			//Throw an Exception
	//sevl				//Wake Up Local Core
	//b	0b			//Go back to Wait For Event
8:	bl	_halt			//Bootloop until power down

_uart_setup:
	mov	x1, #0x0004		//GPFSEL1
	movk	x1, #0xFE20, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	ldr	w3, [x1]
	mov	w2, #0x0FFF
	movk	w2, #0xFFFC, lsl #16
	and	w3, w3, w2
	mov	w2, #0x2000		//Pins 14 & 15 in ALT5
	movk	w2, #0x0001, lsl #16
	orr	w3, w3, w2
	str	w3, [x1]

	mov	x1, #0x00E4		//GPIO_PUP_PDN_CNTRL_REG0
	movk	x1, #0xFE20, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	mov	w2, #0xFFFF		//Bits 28-31 in PULL_NONE
	movk	w2, #0x0FFF, lsl #16
	ldr	w3, [x1]
	and	w2, w3, w2
	str	w2, [x1]

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

_uart_flush:
	mov	w3, #0x05DC

1:	mov	w0, #0x00

2:	mov	x1, #0x5054	         //Wait for UART to be idle
	movk	x1, #0xFE21, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	ldr	w2, [x1]
	and	w2, w2, #0x20
	cbz	w2, 2b

	mov	x1, #0x5040		//Write 1 byte
	movk	x1, #0xFE21, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	str	w0, [x1]

	sub	w3, w3, #1
	cbnz	w3, 1b
	ret

_uart_newline:
	mov	w0, #0x0D

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

	mov	w0, #0x0A

2:	mov	x1, #0x5054		//Wait for UART to be idle
	movk	x1, #0xFE21, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	ldr	w2, [x1]
	and	w2, w2, #0x20
	cbz	w2, 2b

	mov	x1, #0x5040		//Write 1 byte
	movk	x1, #0xFE21, lsl #16
	movk	x1, #0x0000, lsl #32
	movk	x1, #0x0000, lsl #48
	str	w0, [x1]

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

_interrupt:
	msr	ESR_EL1, x0
	mov	x1, #9			//Service Call Dialled 9
	cmp	x1, x0
	bne	2f
	msr	elr_el3, x3		//Previously stored Link

	//Might have to do a core check here...

	mov	w0, #0x21		//Write "!" to the Serial Port

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

	//mrs	x0, elr_el3		//Step Over Faulty Instruction
	//add	x0, x0, #4		//Might not always be required
	//msr	elr_el3, x0

2:	ret				//eret called later

_event:
1:	wfe
	mov	w0, #0x41		//Write "AM" to the Serial Port
	bl	_uart_write_byte
	mov	w0, #0x4D
	bl	_uart_write_byte
	bl	_uart_newline

	//b	1b
	//b	x3			//Store Work Address Here Beforehand, All methods have been Deprecated
	//b	lr
	//mov	pc, lr
	//ldr	pc, [lr]
	//str	lr, [pc]
	//mov	lr, x3
	//ret				//This goes nowhere correct automatically, these registers aren't shared

	//svc	#9			//Using Interrupts/Exceptions for this is just a bad workaround
	//b	1b			//to an architectural situation that should not have happened

	//eret				//This is only for Interrupts/Exceptions, not Events

	mrs	x3, elr_el3		//Read the return address from the coprocessor (hope there's no interrupt)
	mov	lr, x3
	ret				//Coprocessor memory isn't shared, this won't return correctly
	b	1b			//Even GPIO could do this task

_halt:
1:	wfe
	b	1b