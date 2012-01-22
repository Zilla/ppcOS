LOG=$(BUILDDIR)/logl.o
LOGOBJS=$(BUILDDIR)/log.o

all: $(LOG)

$(LOG): $(LOGOBJS)
	$(AR) $(ARFLAGS) $(LOG) $(LOGOBJS)

$(BUILDDIR)/log.o: log.c
	$(CC) $(CFLAGS) -o $@ $<
