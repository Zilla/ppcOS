CC=powerpc-eabi-gcc
CFLAGS=-c -g -Wall -Werror $(INCDIRS) -fno-builtin -msoft-float -mcpu=440 -fomit-frame-pointer
INCDIRS=-I$(shell pwd)/src/ -I$(shell pwd)/src/include -I$(shell pwd)/newlib/powerpc-eabi/include
LDFLAGS=-T src/kernel.lcf -fno-builtin -msoft-float -mcpu=440
LOADERLFFLAGS=-T src/boot.lcf -nostdlib -fno-builtin -Wall -Werror
DOT=dot
IMGTYPE=-Tpng
BUILDDIR=$(shell pwd)/build
AR=powerpc-eabi-ar
ARFLAGS=rcs
OBJCOPY=powerpc-eabi-objcopy
NEWLIBS=newlib/powerpc-eabi/lib/libc.a newlib/powerpc-eabi/lib/libm.a
DIVIDER="$(BSTART)=================================$(BEND)"
SUBMAKE=$(MAKE) --no-print-directory
BSTART=\033[1m
BEND  =\033[0m


# Build objects
KERNEL=kernel.ppc.bin
KRNELF=kernel.ppc.elf

CRT0   = $(BUILDDIR)/crt0.o
IRQ    = $(BUILDDIR)/irql.o
LOG    = $(BUILDDIR)/logl.o
MM     = $(BUILDDIR)/mml.o
TIMER  = $(BUILDDIR)/timerl.o
UART   = $(BUILDDIR)/uartl.o
PROC   = $(BUILDDIR)/procl.o
SCHED  = $(BUILDDIR)/schedl.o
VFS    = $(BUILDDIR)/vfsl.o
PFS    = $(BUILDDIR)/pfsl.o
DEVFS  = $(BUILDDIR)/devfsl.o


OBJS=$(CRT0) $(IRQ) $(MM) $(TIMER) $(UART) $(LOG) $(PROC) $(SCHED) $(VFS) $(PFS) $(DEVFS)

export CC CFLAGS INCDIRS LDFLAGS BUILDDIR AR ARFLAGS

all: make-all

.PHONY: docs
docs:
	$(DOT) $(IMGTYPE) -o doc/os.png doc/os.dot

.PHONY: emu
emu:
	./scripts/start_simics.sh

make-all: clean $(KERNEL) loader

$(KERNEL): $(OBJS)
	@echo $(DIVIDER)
	@echo "Linking $(KRNELF)"
	@$(CC) $(LDFLAGS) $(NEWLIBS) $(OBJS) -o $(KRNELF)
	@echo "Creating binary $(KERNEL)"
	@$(OBJCOPY) -O binary $(KRNELF) $(KERNEL)

loader:	loader.o
	@echo "Linking boot loader"
	@$(CC) $(LOADERLFFLAGS) build/loader.o -o loader.ppc.elf
	@echo "Creating binary boot loader"
	@$(OBJCOPY) -O binary loader.ppc.elf loader.ppc.bin

loader.o:
	@echo $(DIVIDER)
	@echo "Building boot loader"
	@$(CC) $(CFLAGS) -mregnames src/boot/loader.S -o build/loader.o

$(CRT0):
	@echo $(DIVIDER)
	@echo "Building $(BSTART)$(notdir $@)$(BEND)"
	@$(SUBMAKE) -C src/crt0

$(IRQ):
	@echo $(DIVIDER)
	@echo "Building $(BSTART)$(notdir $@)$(BEND)"
	@$(SUBMAKE) -C src/irq

$(LOG):
	@echo $(DIVIDER)
	@echo "Building $(BSTART)$(notdir $@)$(BEND)"
	@$(SUBMAKE) -C src/log

$(MM):
	@echo $(DIVIDER)
	@echo "Building $(BSTART)$(notdir $@)$(BEND)"
	@$(SUBMAKE) -C src/mm

$(TIMER):
	@echo $(DIVIDER)
	@echo "Building $(BSTART)$(notdir $@)$(BEND)"
	@$(SUBMAKE) -C src/timer

$(UART):
	@echo $(DIVIDER)
	@echo "Building $(BSTART)$(notdir $@)$(BEND)"
	@$(SUBMAKE) -C src/uart;

$(PROC):
	@echo $(DIVIDER)
	@echo "Building $(BSTART)$(notdir $@)$(BEND)"
	@$(SUBMAKE) -C src/proc

$(SCHED):
	@echo $(DIVIDER)
	@echo "Building $(BSTART)$(notdir $@)$(BEND)"
	@$(SUBMAKE) -C src/sched

$(VFS):
	@echo $(DIVIDER)
	@echo "Building $(BSTART)$(notdir $@)$(BEND)"
	@$(SUBMAKE) -C src/vfs

$(PFS):
	@echo $(DIVIDER)
	@echo "Building $(BSTART)$(notdir $@)$(BEND)"
	@$(SUBMAKE) -C src/fs/pfs

$(DEVFS):
	@echo $(DIVIDER)
	@echo "Building $(BSTART)$(notdir $@)$(BEND)"
	@$(SUBMAKE) -C src/fs/devfs

.PHONY: clean
clean:
	@echo $(DIVIDER)
	@echo "Removing build files"
	@echo $(DIVIDER)
	@rm -f kernel.ppc.elf
	@rm -f kernel.ppc.bin
	@rm -f loader.ppc.elf
	@rm -f loader.ppc.bin
	@rm -rf build/*.o

