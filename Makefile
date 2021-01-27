CFILES = $(wildcard *.C)
OFILES = $(CFILES:.C=.O)
GCCFLAGS = -Wall -O2 -ffreestanding -nostdinc -nostdlib -nostartfiles
GCCPATH = ./XCOMPILER-LINUX86/bin

all: clean kernel8.img

boot.O: boot.S
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c BOOT.S -o BOOT.O

%.O: %.C
	$(GCCPATH)/aarch64-none-elf-gcc $(GCCFLAGS) -c $< -o $@

kernel8.img: boot.O $(OFILES)
	$(GCCPATH)/aarch64-none-elf-ld -nostdlib -nostartfiles BOOT.O $(OFILES) -T LINK.LD -o kernel8.elf
	$(GCCPATH)/aarch64-none-elf-objcopy -O binary kernel8.elf PiX.img
	/bin/cp PiX.img PiX.iES

clean:
	/bin/rm kernel8.elf *.O *.img *.iES > /dev/null 2> /dev/null || true
