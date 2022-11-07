
PSPINC = 
PSPINC += -I ./include/kernel
PSPINC += -I ./include/user
PSPINC += -I ./include/base
PSPINC += -I ./include/modinfo
PSPINC += -I ./include/display
PSPINC += -I ./include/debug
PSPINC += -I ./include/ctrl
#PSPINC += -I ./include/#

PSPLIB = 
PSPLIB += -L ./lib/kernel -lpspkernel
PSPLIB += -L ./lib/user -lpspuser
PSPLIB += -L ./lib/base
PSPLIB += -L ./lib/modinfo -lpspmodinfo
PSPLIB += -L ./lib/display -lpspdisplay -lpspdisplay_driver
PSPLIB += -L ./lib/debug -lpspdebug -lpspdebugkb -lpspgdb -lpspgdb_kernel -lpspgdb_user
PSPLIB += -L ./lib/ctrl -lpspctrl
#PSPLIB += -L ./lib/#

#PSPINC += -I ./include/gu
#PSPLIB += -L ./lib/gu -lpspgu

#PSPINC += -I ./include/fpu
#PSPLIB += -L ./lib/fpu -lpspfpu

#PSPINC += -I ./include/vfpu
#PSPLIB += -L ./lib/vfpu -lpspvfpu

#PSPINC += -I ./include/mp3
#PSPLIB += -L ./lib/mp3 -lpspmp3

#PSPINC += -I ./include/mpeg
#PSPLIB += -L ./lib/mpeg -lpspjpeg -lpspmpeg -lpspmpegbase -lpspmpegbase_driver

#PSPINC += -I ./include/video
#PSPLIB += -L ./lib/video -lpspvideocodec

PSPLNK = 
PSPLNK = -T link.ld.main
#PSPLNK += -T ./lib/base/linkfile.prx.in

all:
	#export PSPOUT="/mnt/c/users/alastair/documents/PPSSPP/memstick/PSP/GAME/RKN" && \
	export PSPDEV="/usr/local/pspdev" && \
	export PSPBIN="$$PSPDEV/bin" && \
	export PSPLIB="$$PSPDEV/lib" && \
	export PSPSDK="./elf2pbp" && \
	export PSPGCC="$$PSPLIB/gcc/psp/11.2.0" && \
	export PATH="$$PATH:$$PSPBIN:./elf2pbp" && \
	psp-gcc -c main.c -o main.o $(PSPINC) && \
	psp-ld main.o -o kernel.elf $(PSPLNK) $(PSPLIB) && \
	#psp-strip -s kernel.elf && \
	psp-objdump -x kernel.elf && \
	psp-readelf -a kernel.elf && \
	elf2pbp kernel.elf EBOOT.PBP && \
	#cp ./EBOOT.PBP $$PSPOUT && \
	echo "Done!"
clean:
	rm -f ./kernel.elf
	rm -f ./EBOOT.PBP
	rm -f ./main.o
