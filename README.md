# PiX-iES
rpi4-osdev working example of Raspberry Pi 4 Framebuffer being controlled by ARM64 core (first boot of PiX GPU)

## Step-Over

### 1) What is Step-Over behaviour of a core and why is it useful?

Here is the output of a sequence which has been stepped-over.
The sequence happens to be part of an MMU enablement test.
```
a[H] = 0x00000000
a[L] = 0x30C50839
b[H] = 0x00000000
b[L] = 0x00000001
c[H] = 0x00000000
c[L] = 0x30C50839
[CALL]: LDUR/STUR

[ISR]: SyncEx

[ISR]: SyncEx

[TEST]: COMPLETE
```
### 2) How did the system get through 2 Synchronous Exceptions and still complete the test?

It has the following code for a vector table in Assembly Language and an interrupt handler in C/C++.
```
//-boot.S--------------------------------------------------------------------------

.section .text
.global _vector
.global _start
//etc...

_reset:
	.org 0x0
	bl	_start

_vector:
	//Current Exception Level and Stack Pointer Zero
	.org 0x800            //Synchronous Exception (SyncEx)
	bl    interrupt_synch
	eret
	.org 0x880            //Interrupt Request Queue (IRQ)
	//etc...

.org 0x80000
_start:
	mrs	x5, CurrentEL		//Get Current Exception Level
	ldr	x7, =_vector		//Load Vector Table
	msr	vbar_el1, x7
	msr	vbar_el2, x7
	msr	vbar_el3, x7		//Must be run in EL3, not EL2 (armstub= instead of kernel=)
	msr	daifset, #0b1100	//DAIF - Debug, Asynchronous, IRQ, FIQ
	msr	daifclr, #0b0011
	//etc...

//-main.cpp------------------------------------------------------------------------

extern "C"
{
    void interrupt_synch()   { uart_write("\n[ISR]: SyncEx\n");
//      unsigned long long a = 1;
//      unsigned long long b = 0;
        asm volatile
        (
//          "mov %[b], %[a];"
            "mrs x0, elr_el3;"		//Read the Exception Link Register
            "add x0, x0, #4;"		//Increment the temporary register by 1 address (0x4)
            "msr elr_el3, x0;"		//Write back to the ELR
            "sub x0, x0, x0;"		//Clear the temporary register used
//          : [b] "=rm"(b)
//          : [a] "rm"(a)
        );
        return;
    }
    void interrupt_request() { uart_write("\n[ISR]: IRQ\n");              return; }
    void interrupt_fast()    { uart_write("\n[ISR]: FIQ\n");              return; }
    void interrupt_system()  { uart_write("\n[ISR]: SError\n"); while(1); return; }
}

//---------------------------------------------------------------------------------
```
### 3.1) What does the above code do?

When an unaligned read/write occurs (triggered by LDUR/STUR in this case), \
a **Synchronous Exception (SyncEx)** is generated.

If there is a handler available at the address for SyncEx in the vector table (0x800) \
then it should be run. In this case it is written in C and is called "interrupt_synch". \
(branch and link - bl - is used instead of just a normal branch)

The interrupt handler uses inline assembly to read and write the **Exception Link Register (ELR)**. \
This is the address that the program should return to when "eret" is called at the end \
of an interrupt handler.

The ELR is incremented by 0x4 because that is (or should be) the next address in the program \
after the faulty instruction that triggered the Synchronous Exception. This can differ heavily \
on different systems and architectures and could be made customisable by the programmer.

### 3.2) How could it be improved?

This behaviour causes the core to "step-over" faulty instructions and continue with the program. \
However, this behaviour should only be used when the programmer knows that a set of instructions \
could be faulty (it should not be used as a general purpose catch-all handler).

The behaviour of "step-over" could be made conditional based on a flag for **"Conditional Step-Over" (CSO)**. \
The CSO bits would act as an indicator to toggle if the interrupt handler should stall (default behaviour) \
or if it should carry on with the rest of the program (similar to the behaviour of a try/catch statement).

Extra care must be taken to return the core to the usable state before the faulty instruction was executed. \
If this cannot be done then the system should stall as per default to avoid corruption and/or data loss.

### Thank you for your attention.
#### the
