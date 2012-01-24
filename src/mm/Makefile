MM=$(BUILDDIR)/mml.o
MMOBJS=$(BUILDDIR)/mm.o

all: $(MM)

$(MM): $(MMOBJS)
	@echo "Creating archive $(notdir $@)"
	@$(AR) $(ARFLAGS) $(MM) $(MMOBJS)

$(BUILDDIR)/mm.o: mm.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<
