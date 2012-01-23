PROC=$(BUILDDIR)/procl.o
PROCOBJS=$(BUILDDIR)/process.o $(BUILDDIR)/test_proc.o

all: $(PROC)

$(PROC): $(PROCOBJS)
	$(AR) $(ARFLAGS) $(PROC) $(PROCOBJS)

$(BUILDDIR)/process.o: process.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILDDIR)/test_proc.o: test_proc.c
	$(CC) $(CFLAGS) -o $@ $<
