CC=powerpc-eabi-gcc
CFLAGS=-c -g -Wall $(INCDIRS) -fno-builtin -msoft-float -mcpu=440
INCDIRS=-I$(shell pwd)/src/ -I$(shell pwd)/newlib/powerpc-eabi/include -I$(shell pwd)/src/include
LDFLAGS=-T src/kernel.lcf -fno-builtin -msoft-float -mcpu=440
LOADERLFFLAGS=-T src/boot.lcf -nostdlib -fno-builtin
DOT=dot
IMGTYPE=-Tpng
BUILDDIR=$(shell pwd)/build
AR=powerpc-eabi-ar
ARFLAGS=rcs
OBJCOPY=powerpc-eabi-objcopy
NEWLIBS=newlib/powerpc-eabi/lib/libc.a newlib/powerpc-eabi/lib/libm.a

# Build objects
KERNEL=kernel.ppc.bin
KRNELF=kernel.ppc.elf

CRT0  = $(BUILDDIR)/crt0.o
IRQ   = $(BUILDDIR)/irql.o
LOG   = $(BUILDDIR)/logl.o
MM    = $(BUILDDIR)/mml.o
STUB  = $(BUILDDIR)/syscal_stubs.o
TIMER = $(BUILDDIR)/timerl.o
UART  = $(BUILDDIR)/uartl.o
PROC  = $(BUILDDIR)/procl.o

OBJS=$(CRT0) $(IRQ) $(MM) $(STUB) $(TIMER) $(UART) $(LOG) $(PROC)

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
	$(CC) $(LDFLAGS) $(OBJS) $(NEWLIBS) -o $(KRNELF)
	$(OBJCOPY) -O binary $(KRNELF) $(KERNEL)

loader:	loader.o
	$(CC) $(LOADERLFFLAGS) build/loader.o -o loader.ppc.elf
	$(OBJCOPY) -O binary loader.ppc.elf loader.ppc.bin

loader.o:
	$(CC) $(CFLAGS) -mregnames src/boot/loader.S -o build/loader.o

$(CRT0):
	(cd src/crt0; make)

$(IRQ):
	(cd src/irq; make)

$(LOG):
	(cd src/log; make)

$(MM):
	(cd src/mm; make)

# For some reason, newlib gives link errors if we build syscall_stubs
# the same way as the other C-files.
$(STUB):
	$(CC) $(CFLAGS) -o $(STUB) src/stubs/syscall_stubs.c

$(TIMER):
	(cd src/timer; make)

$(UART):
	(cd src/uart; make)

$(PROC):
	(cd src/proc; make)


.PHONY: clean
clean:
	rm -f kernel.ppc.elf
	rm -f kernel.ppc.bin
	rm -f loader.ppc.elf
	rm -f loader.ppc.bin
	rm -rf build/*.o

