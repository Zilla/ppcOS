UART=$(BUILDDIR)/uartl.o
UARTOBJS=$(BUILDDIR)/uart.o

all: $(UART)

$(UART): $(UARTOBJS)
	@echo "Creating archive $(notdir $@)"
	@$(AR) $(ARFLAGS) $(UART) $(UARTOBJS)

$(BUILDDIR)/uart.o: uart.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<
