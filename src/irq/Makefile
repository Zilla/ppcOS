IRQ=$(BUILDDIR)/irql.o
IRQOBJS=$(BUILDDIR)/exception_handlers.o $(BUILDDIR)/asm_exception_handlers.o $(BUILDDIR)/irq.o

all: $(IRQ)

$(IRQ): $(IRQOBJS)
	$(AR) $(ARFLAGS) $(IRQ) $(IRQOBJS)

$(BUILDDIR)/exception_handlers.o: exception_handlers.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILDDIR)/asm_exception_handlers.o: exception_handlers.S
	$(CC) $(CFLAGS) -mregnames -o $@ $<

$(BUILDDIR)/irq.o: irq.c
	$(CC) $(CFLAGS) -o $@ $<
