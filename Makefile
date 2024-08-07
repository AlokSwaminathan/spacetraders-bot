CC := gcc
CFLAGS := -Wall -Wextra -g

SRCDIR := src
BUILDDIR := build
LIBDIR := lib
TESTDIR := tests
BUILDDIR_TEST := $(BUILDDIR)/$(TESTDIR)
BUILDDIR_LIB := $(BUILDDIR)/$(LIBDIR)

SRCS := $(wildcard $(SRCDIR)/*.c)
LIBS := $(wildcard $(LIBDIR)/*.c)
TESTS := $(wildcard $(TESTDIR)/*.c)
TEST_SCRIPTS := $(wildcard $(TESTDIR)/*.py)
SRCOBJS := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))
LIBOBJS := $(patsubst $(LIBDIR)/%.c, $(BUILDDIR_LIB)/%.o, $(LIBS))
TESTOBJS := $(patsubst $(TESTDIR)/%.c, $(BUILDDIR_TEST)/%.o, $(TESTS))
TEST_TARGETS := $(patsubst $(TESTDIR)/%.c, $(BUILDDIR_TEST)/%, $(TESTS))

TARGET := $(BUILDDIR)/spacetrader_bot

$(TARGET) : $(SRCOBJS) $(LIBOBJS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(SRCOBJS) $(LIBOBJS) -o $@

$(BUILDDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -I$(LIBDIR) -c $< -o $@

$(BUILDDIR_LIB)/%.o : $(LIBDIR)/%.c
	@mkdir -p $(BUILDDIR_LIB)
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILDDIR_TEST)/%.o : $(TESTDIR)/%.c
	@mkdir -p $(BUILDDIR_TEST)
	$(CC) $(CFLAGS) -I$(LIBDIR) -c $< -o $@

$(TEST_TARGETS) : $(TESTOBJS) $(LIBOBJS)
	@mkdir -p $(BUILDDIR_TEST)
	$(CC) $(LIBOBJS) $(TESTOBJS) -o $@

.PHONY: all
all: $(TARGET)

.PHONY: test
test: $(TEST_TARGETS)

.PHONY: clean
clean:
	@echo "Deleting build dir"
	@rm -rf $(BUILDDIR)
