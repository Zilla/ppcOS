CC=powerpc-eabi-gcc
CFLAGS=-c -Wall
LDFLAGS=-T kernel.lcf
DOT=dot
IMGTYPE=-Tpng


all: make-all

docs:
	$(DOT) $(IMGTYPE) -o doc/os.png doc/os.dot

make-all: start.o start-asm.o
	$(CC) $(LDFLAGS) build/start.o build/start-asm.o -o kernel.ppc.elf
	powerpc-eabi-objcopy -O binary kernel.ppc.elf kernel.ppc.bin

start.o:
	$(CC) $(CFLAGS) src/crt0/start.c -o build/start.o

start-asm.o:
	$(CC) $(CFLAGS) src/crt0/start.S -o build/start-asm.o

clean:
	rm -f kernel.ppc.elf
	rm -f kernel.ppc.bin
	rm -rf build/*.o

