LOG=$(BUILDDIR)/logl.o
LOGOBJS=$(BUILDDIR)/log.o

all: $(LOG)

$(LOG): $(LOGOBJS)
	@echo "Creating archive $(notdir $@)"
	@$(AR) $(ARFLAGS) $(LOG) $(LOGOBJS)

$(BUILDDIR)/log.o: log.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<
