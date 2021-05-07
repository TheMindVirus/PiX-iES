@echo on
rem [pix.ies]
set PATH=%PATH%;%CD%\xcompiler\bin
set COMPILER_FLAGS=-Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
set LINKER_FLAGS= -T link.ld -nostdlib -nostartfiles

aarch64-none-elf-g++ %COMPILER_FLAGS% -c boot.S -o boot.o
aarch64-none-elf-g++ %COMPILER_FLAGS% -c pix.cxx -o pix.o
aarch64-none-elf-g++ %COMPILER_FLAGS% -c ies.cxx -o ies.o
aarch64-none-elf-ld  %LINKER_FLAGS% boot.o ies.o pix.o -o kernel8.elf
aarch64-none-elf-objcopy -O binary kernel8.elf pix.ies
copy /Y .\pix.ies .\boot\pix.ies
pause