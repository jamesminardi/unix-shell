
# This make file, upon "make all", builds object files for all source files in the
# "src" directory (and sub directories), and flattens them into the build directory.

FLAGS := -Wextra -Wall -Wshadow -Werror -g #-O3 

SRCDIR := src
BLDDIR := build

SOURCES := $(wildcard $(SRCDIR)/*.c $(SRCDIR)/**/*.c)
HEADERS := $(wildcard $(SRCDIR)/*.h $(SRCDIR)/**/*.h)

# Tree directories of all object files ( i.e. <filename>.o subdir/<filename2>.o )
TARGETS := $(patsubst $(SRCDIR)/%.c, %.o, $(SOURCES))

# Final directory for object files (i.e. build/<filename>.o build/<filename2>.o)
OBJS := $(patsubst %.c, $(BLDDIR)/%.o, $(notdir $(SOURCES)))

.PHONY : all clean

print-%  : ; @echo $* = $($*)


all : shell

# Target "shell" uses object files in tree format as targets.
# Builds shell by placing in blddir, as well as flattening the object files into the blddir.
shell : $(TARGETS)
	cc -o $(BLDDIR)/$@ $(OBJS)


# Flatten targets to the blddir directory.
$(TARGETS) : %.o : $(SRCDIR)/%.c
	cc -c -o $(BLDDIR)/$(notdir $@) $^ $(FLAGS)


clean :
	rm -rf $(OBJS) $(BLDDIR)/shell $(BLDDIR)/*



