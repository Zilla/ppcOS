CC=powerpc-eabi-gcc
CFLAGS=-c -g -Wall $(INCDIRS) -fno-builtin
INCDIRS=-Isrc/ -Inewlib/powerpc-eabi/include -Isrc/include
LDFLAGS=-T src/kernel.lcf -fno-builtin 
LOADERLFFLAGS=-T src/boot.lcf -nostdlib -fno-builtin
DOT=dot
IMGTYPE=-Tpng


all: make-all

docs:
	$(DOT) $(IMGTYPE) -o doc/os.png doc/os.dot

emu:
	./scripts/start_simics.sh

make-all: kernel loader

kernel: start.o start-asm.o mm.o syscall_stubs.o uart.o
	$(CC) $(LDFLAGS) build/syscall_stubs.o build/start.o build/start-asm.o build/mm.o build/uart.o newlib/powerpc-eabi/lib/libc.a newlib/powerpc-eabi/lib/libm.a -o kernel.ppc.elf
	powerpc-eabi-objcopy -O binary kernel.ppc.elf kernel.ppc.bin

loader:	loader.o
	$(CC) $(LOADERLFFLAGS) build/loader.o -o loader.ppc.elf
	powerpc-eabi-objcopy -O binary loader.ppc.elf loader.ppc.bin

start.o:
	$(CC) $(CFLAGS) src/crt0/start.c -o build/start.o

start-asm.o:
	$(CC) $(CFLAGS) -mregnames src/crt0/start.S -o build/start-asm.o

loader.o:
	$(CC) $(CFLAGS) -mregnames src/boot/loader.S -o build/loader.o

mm.o:
	$(CC) $(CFLAGS) src/mm/mm.c -o build/mm.o

uart.o:
	$(CC) $(CFLAGS) src/uart/uart.c -o build/uart.o

syscall_stubs.o:
	$(CC) $(CFLAGS) src/stubs/syscall_stubs.c -o build/syscall_stubs.o

clean:
	rm -f kernel.ppc.elf
	rm -f kernel.ppc.bin
	rm -f loader.ppc.elf
	rm -f loader.ppc.bin
	rm -rf build/*.o

