@ECHO ON

REM No Comment.
SET PATH=%PATH%;%CD%\XCOMPILER-WIN32-TEST\bin
SET FLAGS=-Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles

aarch64-none-elf-gcc %FLAGS% -c BOOT.S -o BOOT.O
aarch64-none-elf-gcc %FLAGS% -c PiX.C -o PiX.O
aarch64-none-elf-gcc %FLAGS% -c iES.C -o iES.O
aarch64-none-elf-ld -nostdlib -nostartfiles BOOT.O iES.O PiX.O -T LINK.LD -o kernel8.elf
aarch64-none-elf-objcopy -O binary kernel8.elf PiX.img
copy PiX.img PiX.iES

PAUSE