@echo on
rem [pix.ies]
set PATH=%CD%\xcompiler\bin
aarch64-none-elf-objdump -D pix.elf
pause