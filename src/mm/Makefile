MM=$(BUILDDIR)/mml.o
MMOBJS=$(BUILDDIR)/mm.o

all: $(MM)

$(MM): $(MMOBJS)
	$(AR) $(ARFLAGS) $(MM) $(MMOBJS)

$(BUILDDIR)/mm.o: mm.c
	$(CC) $(CFLAGS) -o $@ $<
