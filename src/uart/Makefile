UART=$(BUILDDIR)/uartl.o
UARTOBJS=$(BUILDDIR)/uart.o

all: $(UART)

$(UART): $(UARTOBJS)
	$(AR) $(ARFLAGS) $(UART) $(UARTOBJS)

$(BUILDDIR)/uart.o: uart.c
	$(CC) $(CFLAGS) -o $@ $<
