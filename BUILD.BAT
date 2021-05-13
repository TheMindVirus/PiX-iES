@echo on
rem No Comment.
set PATH=%CD%\xcompiler\bin
set FLAGS=-mcpu=cortex-a53 -std=gnu99 -fpic -O2 -ffreestanding -Wall -Wextra -nostartfiles -mstrict-align
set LINKS=-O2 -ffreestanding -nostartfiles -T link.ld -L %CD%\compiler\lib\gcc\aarch64-none-elf\10.2.1\ -lm

aarch64-none-elf-gcc -c hello_fft\gpu_fft.c -o hello_fft\gpu_fft.o %FLAGS%
aarch64-none-elf-gcc -c hello_fft\gpu_fft_base.c -o hello_fft\gpu_fft_base.o %FLAGS%
aarch64-none-elf-gcc -c hello_fft\gpu_fft_twiddles.c -o hello_fft\gpu_fft_twiddles.o %FLAGS%
aarch64-none-elf-gcc -c hello_fft\gpu_fft_shaders.c -o hello_fft\gpu_fft_shaders.o %FLAGS%
rem aarch64-none-elf-gcc -c hello_fft\gpu_fft_trans.c -o hello_fft\gpu_fft_trans.o %FLAGS%

aarch64-none-elf-gcc -c boot.S -o boot.o %FLAGS%
aarch64-none-elf-gcc -c pix.c -o pix.o %FLAGS%
aarch64-none-elf-gcc -c ies.c -o ies.o %FLAGS%

aarch64-none-elf-gcc ^
hello_fft\gpu_fft.o ^
hello_fft\gpu_fft_base.o ^
hello_fft\gpu_fft_twiddles.o ^
hello_fft\gpu_fft_shaders.o ^
boot.o pix.o ies.o ^
-o pix.elf %LINKS%

aarch64-none-elf-objcopy -O binary pix.elf pix.ies
pause

rem hello_fft\gpu_fft_trans.o ^

