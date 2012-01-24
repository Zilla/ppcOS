PROC=$(BUILDDIR)/procl.o
PROCOBJS=$(BUILDDIR)/process.o $(BUILDDIR)/test_proc.o

all: $(PROC)

$(PROC): $(PROCOBJS)
	@echo "Creating archive $(notdir $@)"
	@$(AR) $(ARFLAGS) $(PROC) $(PROCOBJS)

$(BUILDDIR)/process.o: process.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<

$(BUILDDIR)/test_proc.o: test_proc.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<
