TIMER=$(BUILDDIR)/timerl.o
TIMEROBJS=$(BUILDDIR)/fit.o

all: $(TIMER)

$(TIMER): $(TIMEROBJS)
	$(AR) $(ARFLAGS) $(TIMER) $(TIMEROBJS)

$(BUILDDIR)/fit.o: fit.c
	$(CC) $(CFLAGS) -o $@ $<

