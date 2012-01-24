IRQ=$(BUILDDIR)/irql.o
IRQOBJS=$(BUILDDIR)/exception_handlers.o $(BUILDDIR)/asm_exception_handlers.o $(BUILDDIR)/irq.o

all: $(IRQ)

$(IRQ): $(IRQOBJS)
	@echo "Creating archive $(notdir $@)"
	@$(AR) $(ARFLAGS) $(IRQ) $(IRQOBJS)

$(BUILDDIR)/exception_handlers.o: exception_handlers.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<

$(BUILDDIR)/asm_exception_handlers.o: exception_handlers.S
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -mregnames -o $@ $<

$(BUILDDIR)/irq.o: irq.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<
