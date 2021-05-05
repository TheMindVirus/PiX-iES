.section .text
.global _start

_start:
    //Processor ID Check
    mrs     x1, mpidr_el1
    and     x1, x1, #3
    cbz     x1, 2f
    //Invalid Core - Infinite Loop
1:  wfe
    b       1b
2:  //Valid Core - Continue

    //Move Stack to the end after all labels
    ldr     x1, =_start
    mov     sp, x1

    //Clean the BSS Section
    ldr     x1, =__bss_start     //Start address
    ldr     w2, =__bss_size      //Section Size
3:  cbz     w2, 4f               //Quit Loop If Zero
    str     xzr, [x1], #0
    add     x1, x1, #16
    sub     w2, w2, #1
    cbnz    w2, 3b               //Loop If Non-Zero

    //Reset Used Registers
    sub     x1, x1, x1
    sub     w2, w2, w2

    //Branch to main() in C
4:  bl      main
    //main() returned - Infinite Loop
    b       1b