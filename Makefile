COMPILE := gcc -std=gnu11 -Wall -Werror
EXEC := client
DFLAGS := -g -DDEBUG

SRCD := src
BLDD := build
BIND := bin
INCD := include

SRCF := $(shell find $(SRCD) -type f -name *.c)
OBJF := $(patsubst $(SRCD)/%,$(BLDD)/%,$(SRCF:.c=.o))

.PHONY: clean all setup debug
.DEFAULT: clean all

all: setup $(EXEC)

debug: COMPILE += $(DFLAGS)
debug: all

clean:
	rm -rf $(BIND) $(BLDD)

setup:
	mkdir -p bin build

$(EXEC): $(OBJF)
	$(COMPILE) -I $(INCD) $^ -o $(BIND)/$@ 

$(BLDD)/%.o: $(SRCD)/%.c
	$(COMPILE) -I $(INCD) -c $< -o $@
