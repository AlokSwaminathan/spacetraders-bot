CC := gcc
CFLAGS := -Wall -Wextra -Werror

SRCDIR := src
BUILDDIR := build

SRCS := $(wildcard $(SRCDIR)/*.c)
OBJS := $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SRCS))

TARGET := $(BUILDDIR)/spacetrader_bot

.PHONY: all
all: $(TARGET)

$(TARGET) : $(OBJS)
	@mkdir -p $(BUILDDIR)
	$(CC) $(OBJS) -o $@

$(BUILDDIR)/%.o : $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR)
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	@echo "Deleting build dir"
	@rm -rf $(BUILDDIR)
