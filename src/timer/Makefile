TIMER=$(BUILDDIR)/timerl.o
TIMEROBJS=$(BUILDDIR)/fit.o $(BUILDDIR)/tick.o

all: $(TIMER)

$(TIMER): $(TIMEROBJS)
	$(AR) $(ARFLAGS) $(TIMER) $(TIMEROBJS)

$(BUILDDIR)/fit.o: fit.c
	$(CC) $(CFLAGS) -o $@ $<

$(BUILDDIR)/tick.o: tick.c
	$(CC) $(CFLAGS) -o $@ $<
