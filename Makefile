#!/usr/bin/make -f

# GCC9.2 is not in /bin on the uni PCs
PATH := /opt/york/cs/net/gcc-arm-9.2.0/bin:$(PATH)
SHELL := /bin/bash
ARCH=arm-none-eabi
CC=$(ARCH)-gcc
OBJCOPY=$(ARCH)-objcopy

# CodeSourcery thumb2 libs
SOURCERY=$(PKG)/sourcery-g++-lite-arm-eabi-2010.09.51-i686-1
GNU_VERSION=4.5.0
THUMB2GNULIB=$(SOURCERY)/$(ARCH)/lib/$(GNU_VERSION)/thumb2
THUMB2GNULIB2=$(SOURCERY)/$(ARCH)/lib/thumb2

# CMSIS Startup files, also the flags passed to the C compiler, and linker
CMSIS=/opt/york/cs/net/lpc1700-cmsis-lite-2011.01.26-i686-1
CMSISINCLUDES=-I$(CMSIS)/include
CMSISFL=$(CMSIS)/lib/core_cm3.o \
		$(CMSIS)/lib/system_LPC17xx.o \
		$(CMSIS)/lib/startup_LPC17xx.o
LDSCRIPT = $(CMSIS)/lib/ldscript_rom_gnu.ld

EXECFILENAME = exec
EXECNAME	   = bin/exec

# Compiler Flags
WARN= -Wall
CFLAGS= -O2 -std=gnu18 $(WARN) \
		-mcpu=cortex-m3 -mthumb -mtune=cortex-m3 -march=armv7-m \
		-D__thumb2__=1 -msoft-float -ggdb -mno-sched-prolog -fno-hosted  \
		-mfix-cortex-m3-ldrd -ffunction-sections -fdata-sections \
		-D__RAM_MODE__=0 $(CMSISINCLUDES) -I../libs -I.
LDFLAGS=$(CMSISFL) -static -mcpu=cortex-m3 -mthumb -mthumb-interwork \
		-Wl,--start-group -L$(THUMB2GNULIB) -L$(THUMB2GNULIB2) \
		-lc -lg -lstdc++ -lsupc++ -lgcc -lm -Wl,--end-group \
		-Xlinker -Map -Xlinker bin/lpc1700.map -Xlinker -T $(LDSCRIPT)
LDFLAGS+=-L$(CMSIS)/lib -lDriversLPC17xxgnu

# Target locations
LIBSC		= libs/*.c
LIBS		= $(shell echo $(LIBSC) | sed s/\\.c/\\.o/g)
TASKSC      = tasks/*.c
TASKS		= $(shell echo $(TASKSC) | sed s/\\.c/\\.o/g)
OBJ			= $(LIBS) $(TASKS) main.o

# Targets
all:	main
	@echo "Build finished"

version:
	$(CC) --version

libs: $(LIBS)

main: $(OBJ) libs
	mkdir -p bin
	/opt/york/cs/net/bin/arm-none-eabi-gcc -o $(EXECNAME) $(OBJ) $(LDFLAGS)
	$(OBJCOPY) -I elf32-little -O binary $(EXECNAME) $(EXECNAME).bin

clean:
	rm -f `find . | grep \~`
	rm -f *.swp *.o */*.o */*/*.o *.log
	rm -f *.d */*.d *.srec */*.a bin/*.map
	rm -f *.elf *.wrn bin/*.bin log *.hex
	rm -f $(EXECNAME)

USER:=$(shell whoami)

install:
	@echo "Copying " $(EXECNAME) "to the MBED file system"
	-rm /media/$(USER)/MBED/$(EXECFILENAME).bin
	cp $(EXECNAME).bin /media/$(USER)/MBED &
	sync
	@echo "Now press the reset button on all MBED file systems"
