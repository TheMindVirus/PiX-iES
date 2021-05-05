.section .text
.global _boot
.global _vector

_boot:
    //Initialisation
    //ldr x0, =0xff800000    //Low Peripheral Mode
    //str wzr, [x0]
    //mov w1, 0x80000000     //Prescaler
    //str w1, [x0, #0x8]
    //mrs x0, S3_1_C11_C0_2  //L2 Cache Latency
    //mov x1, #0x22
    //orr x0, x0, x1
    //msr S3_1_C11_C0_2, x0
    //ldr x0, =54000000      //Internal 54MHz Oscillator
    //msr CNTFRQ_EL0, x0
    //msr CNTVOFF_EL2, xzr
    //mov x0, #0x33ff        //Enable FPU //CAN'T DO THIS AT EL1!!!
    //msr CPTR_EL3, x0
    //mov x0, 0b10110110001  //System Control
    //msr SCR_EL3, x0
    //mov x0, 0b1110011      //Auxiliary Control
    //msr ACTLR_EL3, x0
    //mov x0, 0b1000000      //CPU Extended Control
    //msr S3_1_C15_C2_1, x0

    //bl _gic

    //ldr x0, =0x30c50830    //Configure EL2
    //msr SCTLR_EL2, x0
    //mov x0, #0b1111001001  //Enter EL2
    //msr spsr_el3, x0
    //adr x0, in_el2
    //msr elr_el3, x0
    //eret
    
    //Load Interrupt Vector Table
    //mov     x0, #0x5b1
    //msr     scr_el3, x0  //CAN'T DO THIS AT EL1!!!
    //ldr     x1, =_vector
    //msr     vbar_el1, x1
    //msr     vbar_el2, x1
    //msr     vbar_el3, x1
    //msr     daifclr, 0b1111

    ldr     x1, =_vector
    msr     vbar_el2, x1
    msr     daifclr, 0b1111

in_el2:
    //Processor ID Check
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    //Invalid Core - Infinite Loop
1:  wfe
    b       1b
2:  //Valid Core - Continue

    //Move Stack to the end after all labels
    ldr     x1, =_boot
    mov     sp, x1

    //Clean the BSS Section
    ldr     x1, =__bss_start     //Start address
    ldr     w2, =__bss_size      //Section Size
3:  cbz     w2, 4f               //Quit Loop If Zero
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b               //Loop If Non-Zero

    //Reset Used Variables
    mov x1, #0
    mov x2, #0
    mov x3, #0

    //Branch to main() in C
4:  bl      main
    //main() returned - Infinite Loop
    b       1b

_gic:
    mrs x0, MPIDR_EL1
    ldr x2, =0xff841000
    tst x0, #0x3
    b.eq 2f
    mov w0, #3 //Enable IRQ 0 and 1
    str w0, [x2, #0x0]
2:
    add x1, x2, #0x1000
    mov w0, #0x1e7
    str w0, [x1, #0x0]
    mov w0, #0xff
    str w0, [x1, #0x4]
    add x2, x2, #0x80
    mov x0, #0x20
3:
    subs x0, x0, #4
    str w1, [x2, x0]
    b.ne 3b
    ret

.balign 0x800
_vector:
    //Interrupt Vector Table

    //Current Exception Level and Stack Pointer Zero
    b       interrupt_synch      //Synchronous Exception (SyncEx)
    .balign 0x80
    b       interrupt_request    //Interrupt Request Queue (IRQ)
    .balign 0x80
    b       interrupt_fast       //Fast Interrupt Queue (FIQ)
    .balign 0x80
    b       interrupt_system     //System Error (SError)
    .balign 0x80

    //Current Exception Level and Current Stack Pointer
    b       interrupt_synch      //Synchronous Exception (SyncEx)
    .balign 0x80
    b       interrupt_request    //Interrupt Request Queue (IRQ)
    .balign 0x80
    b       interrupt_fast       //Fast Interrupt Queue (FIQ)
    .balign 0x80
    b       interrupt_system     //System Error (SError)
    .balign 0x80

    //Lower Exception Level (Aarch64)         
    b       interrupt_synch      //Synchronous Exception (SyncEx)
    .balign 0x80
    b       interrupt_request    //Interrupt Request Queue (IRQ)
    .balign 0x80
    b       interrupt_fast       //Fast Interrupt Queue (FIQ)
    .balign 0x80
    b       interrupt_system     //System Error (SError)
    .balign 0x80

    //Lower Exception Level (Aarch32)         
    b       interrupt_synch      //Synchronous Exception (SyncEx)
    .balign 0x80
    b       interrupt_request    //Interrupt Request Queue (IRQ)
    .balign 0x80
    b       interrupt_fast       //Fast Interrupt Queue (FIQ)
    .balign 0x80
    b       interrupt_system     //System Error (SError)
    .balign 0x80