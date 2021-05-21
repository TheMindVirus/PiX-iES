@echo on

rem No Comment.
set PATH=%PATH%;%CD%\compiler\bin
set FLAGS=-mcpu=arm1176jzf-s -std=gnu99 -fpic -ffreestanding -O2 -Wall -Wextra
set LINKS=-T link.ld -O2 -ffreestanding -nostartfiles -lgcc -L %CD%\compiler\lib\gcc\arm-none-eabi\10.2.1\

arm-none-eabi-gcc %FLAGS% -c boot.S -o boot.o
arm-none-eabi-gcc %FLAGS% -c pix.c -o pix.o
arm-none-eabi-gcc %FLAGS% -c ies.c -o ies.o
arm-none-eabi-gcc %LINKS% boot.o ies.o pix.o -o pix.elf
arm-none-eabi-objcopy -O binary pix.elf pix.img
copy pix.img pix.ies

pause