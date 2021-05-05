GCCPATH = ./XCOMPILER-LINUX86/bin
COMPILE_FLAGS = -Wall -O0 -ffreestanding -nostdinc -nostdlib -nostartfiles
LINK_FLAGS = -T LinkScript.ld -nostdlib -nostartfiles

all:
	$(GCCPATH)/aarch64-none-elf-g++ $(COMPILE_FLAGS) -c BOOT.S -o BOOT.o
	$(GCCPATH)/aarch64-none-elf-g++ $(COMPILE_FLAGS) -c PiX.cpp -o PiX.o
	$(GCCPATH)/aarch64-none-elf-g++ $(COMPILE_FLAGS) -c iES.cpp -o iES.o
	$(GCCPATH)/aarch64-none-elf-ld $(LINK_FLAGS) BOOT.o iES.o PiX.o -o kernel8.elf
	$(GCCPATH)/aarch64-none-elf-objcopy -O binary kernel8.elf PiX.img
	$(GCCPATH)/aarch64-none-elf-objdump -D kernel8.elf
	cp PiX.img PiX.iES
clean:
	rm kernel8.elf *.o *.img *.iES
