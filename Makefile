CC       ?= cc
CSTD     := -std=c11
WARN     := -Wall -Wextra -Wpedantic -Wshadow -Wconversion -Wno-sign-conversion
DEFS     := -D_POSIX_C_SOURCE=200809L

override CFLAGS := $(CSTD) $(WARN) $(DEFS) -g -O0 $(CFLAGS) $(EXTRA_CFLAGS)
CPPFLAGS := -Iinclude

BUILD_DIR := build
OBJ_DIR   := $(BUILD_DIR)/obj
BIN       := $(BUILD_DIR)/coquille

SRCS := src/main.c
OBJS := $(patsubst src/%.c,$(OBJ_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all clean

all: $(BIN)

$(BIN): $(OBJS) | $(BUILD_DIR)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $^ $(LDLIBS)

$(OBJ_DIR)/%.o: src/%.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -MMD -MP -c -o $@ $<

$(BUILD_DIR) $(OBJ_DIR):
	mkdir -p $@

-include $(DEPS)

clean:
	rm -rf $(BUILD_DIR)
