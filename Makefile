CC := gcc
CFLAGS := -Wall -Wextra -g

SRCDIR := src
BUILDDIR := build
LIBDIR := lib
TESTDIR := test

SRCS := $(wildcard $(SRCDIR)/*.c)
LIBS := $(wildcard $(LIBDIR)/*.c)
SRCOBJS := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))
LIBOBJS := $(patsubst $(LIBDIR)/%.c, $(BUILDDIR)/lib/%.o, $(LIBS))

TARGET := $(BUILDDIR)/spacetrader_bot

.PHONY: all
all: $(TARGET)

$(TARGET) : $(SRCOBJS) $(LIBOBJS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(SRCOBJS) $(LIBOBJS) -o $@

$(BUILDDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I$(LIBDIR) -c $< -o $@

$(BUILDDIR)/lib/%.o : $(LIBDIR)/%.c
	@mkdir -p $(BUILDDIR)/lib
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "Deleting build dir"
	@rm -rf $(BUILDDIR)
