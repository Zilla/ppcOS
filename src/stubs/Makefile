STUB=$(BUILDDIR)/stubl.o
STUBOBJS=$(BUILDDIR)/syscall_stubs.o

all: $(STUB)

$(STUB): $(STUBOBJS)
	$(AR) $(ARFLAGS) $(STUB) $(STUBOBJS)

$(BUILDDIR)/syscall_stubs.o: syscall_stubs.c
	$(CC) $(CFLAGS) -o $@ $<
