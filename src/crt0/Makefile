CRT0=$(BUILDDIR)/crt0.o
CRT0OBJS=$(BUILDDIR)/start.o $(BUILDDIR)/start-asm.o

all: $(CRT0)

$(CRT0): $(CRT0OBJS)
	$(AR) $(ARFLAGS) $(CRT0) $(CRT0OBJS)

$(BUILDDIR)/start.o: start.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILDDIR)/start-asm.o: start.S
	$(CC) $(CFLAGS) -mregnames -o $@ $<
