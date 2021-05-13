@echo on
rem [pix.ies]
set PATH=%PATH%;%CD%\xcompiler\bin
rem aarch64-none-elf-objdump -D pix.elf
rem aarch64-none-elf-objdump -D hello_fft\gpu_fft.o
rem aarch64-none-elf-objdump -D hello_fft\gpu_fft_base.o
rem aarch64-none-elf-objdump -D hello_fft\gpu_fft_shaders.o
aarch64-none-elf-objdump -D hello_fft\gpu_fft_twiddles.o
pause