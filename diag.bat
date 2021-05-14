@echo on
rem [pix.ies]
set PATH=%CD%\xcompiler\bin
aarch64-none-elf-objdump -D pix.elf
rem aarch64-none-elf-objdump -h hello_fft\gpu_fft.o
rem aarch64-none-elf-objdump -h hello_fft\gpu_fft_base.o
rem aarch64-none-elf-objdump -h hello_fft\gpu_fft_shaders.o
rem aarch64-none-elf-objdump -h hello_fft\gpu_fft_twiddles.o
rem aarch64-none-elf-objdump -h hello_fft\gpu_fft_trans.o
pause