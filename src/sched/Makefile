SCHED=$(BUILDDIR)/schedl.o
SCHEDOBJS=$(BUILDDIR)/scheduler.o

all: $(SCHED)

$(SCHED): $(SCHEDOBJS)
	@echo "Creating archive $(notdir $@)"
	@$(AR) $(ARFLAGS) $(SCHED) $(SCHEDOBJS)

$(BUILDDIR)/scheduler.o: scheduler.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<
