VFS=$(BUILDDIR)/vfsl.o
VFSOBJS=$(BUILDDIR)/vfs.o

all: $(VFS)

$(VFS): $(VFSOBJS)
	@echo "Creating archive $(notdir $@)"
	@$(AR) $(ARFLAGS) $(VFS) $(VFSOBJS)

$(BUILDDIR)/vfs.o: vfs.c
	@echo "Compiling $<"
	@$(CC) $(CFLAGS) -o $@ $<
