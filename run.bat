@echo on
set PATH=%PATH%;%CD%\xemulator
qemu-system-arm -m 1G -M raspi2 -serial stdio ^
-global bcm2835-fb.pixo=5 ^
-global bcm2835-fb.xres=1024 ^
-global bcm2835-fb.yres=768 ^
-kernel myos.elf
pause