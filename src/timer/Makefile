TIMER=$(BUILDDIR)/timerl.o
TIMEROBJS=$(BUILDDIR)/fit.o $(BUILDDIR)/tick.o

all: $(TIMER)

$(TIMER): $(TIMEROBJS)
	@echo "Creating archive $(notdir $@)"
	@$(AR) $(ARFLAGS) $(TIMER) $(TIMEROBJS)

$(BUILDDIR)/fit.o: fit.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<

$(BUILDDIR)/tick.o: tick.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<
