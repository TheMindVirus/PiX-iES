.section ".text.boot"
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

    //Move Stack to After the End of _start
    ldr     x1, =_start
    mov     sp, x1

    //Clean the BSS Section
    ldr     x1, =__bss_start     //Start address
    ldr     w2, =__bss_size      //Section Size
3:  cbz     w2, 4f               //Quit Loop If Zero
    str     xzr, [x1], #8
    sub     w2, w2, #1
    cbnz    w2, 3b               //Loop If Non-Zero

    //Branch to main() in C
4:  bl      main
    //main() returned - Infinite Loop
    b       1b