PFS=$(BUILDDIR)/pfsl.o
PFSOBJS=$(BUILDDIR)/pfs.o

all: $(PFS)

$(PFS): $(PFSOBJS)
	@echo "Creating archive $(notdir $@)"
	@$(AR) $(ARFLAGS) $(PFS) $(PFSOBJS)

$(BUILDDIR)/pfs.o: pfs.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<
