# PiX-iES
rpi4-osdev working example of Raspberry Pi 4 Framebuffer being controlled by ARM64 core (first boot of PiX GPU)
https://github.com/isometimes/rpi4-osdev

The Pi4 boots to Exception Level 2 (EL2) instead of EL3 like on Pi3's before it. \
This has made previous bootcode next to useless and needs to be reworked in order to \
get interrupts properly working. The Interrupt/Exception Vector Table has also changed \
in structure entirely in the transition from ARM to Aarch64/32.

More Details: https://developer.arm.com/documentation/100933/0100/AArch64-exception-vector-table

![alt text](https://github.com/TheMindVirus/PiX-iES/blob/asm-interrupt-dev/interrupts.png)
