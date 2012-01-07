CC=powerpc-eabi-gcc
CFLAGS=-c -Wall
LDFLAGS=-T src/kernel.lcf -nostdlib
LOADERLFFLAGS=-T src/boot.lcf -nostdlib
DOT=dot
IMGTYPE=-Tpng


all: make-all

docs:
	$(DOT) $(IMGTYPE) -o doc/os.png doc/os.dot

emu:
	./scripts/start_simics.sh

make-all: kernel loader

kernel: start.o start-asm.o
	$(CC) $(LDFLAGS) build/start.o build/start-asm.o -o kernel.ppc.elf
	powerpc-eabi-objcopy -O binary kernel.ppc.elf kernel.ppc.bin

loader:	loader.o
	$(CC) $(LOADERLFFLAGS) build/loader.o -o loader.ppc.elf
	powerpc-eabi-objcopy -O binary loader.ppc.elf loader.ppc.bin

start.o:
	$(CC) $(CFLAGS) src/crt0/start.c -o build/start.o

start-asm.o:
	$(CC) $(CFLAGS) src/crt0/start.S -o build/start-asm.o

loader.o:
	$(CC) $(CFLAGS) src/boot/loader.S -o build/loader.o

clean:
	rm -f kernel.ppc.elf
	rm -f kernel.ppc.bin
	rm -f loader.ppc.elf
	rm -f loader.ppc.bin
	rm -rf build/*.o

