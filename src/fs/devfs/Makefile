DEVFS=$(BUILDDIR)/devfsl.o
DEVFSOBJS=$(BUILDDIR)/devfs.o

all: $(DEVFS)

$(DEVFS): $(DEVFSOBJS)
	@echo "Creating archive $(notdir $@)"
	@$(AR) $(ARFLAGS) $(DEVFS) $(DEVFSOBJS)

$(BUILDDIR)/devfs.o: devfs.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<
