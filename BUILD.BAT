@echo on
rem No Comment.
set PATH=%CD%\xcompiler\bin

aarch64-none-elf-gcc -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles -c boot.S -o boot.o
aarch64-none-elf-ld -nostdlib -nostartfiles boot.o -o pix.elf -T link.ld
aarch64-none-elf-objcopy -O binary pix.elf pix.ies -v
aarch64-none-elf-objdump -h pix.elf
pause