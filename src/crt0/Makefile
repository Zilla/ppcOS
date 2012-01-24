CRT0=$(BUILDDIR)/crt0.o
CRT0OBJS=$(BUILDDIR)/start.o $(BUILDDIR)/start-asm.o

all: $(CRT0)

$(CRT0): $(CRT0OBJS)
	@echo "Creating archive $(notdir $@)"
	@$(AR) $(ARFLAGS) $(CRT0) $(CRT0OBJS)

$(BUILDDIR)/start.o: start.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<

$(BUILDDIR)/start-asm.o: start.S
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -mregnames -o $@ $<
