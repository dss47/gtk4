CC = gcc
PKG_CONFIG_CFLAGS := $(shell pkg-config --cflags gtk4)
PKG_CONFIG_LIBS := $(shell pkg-config --libs gtk4)
CFLAGS = $(PKG_CONFIG_CFLAGS) -Iinclude -I. -MMD -MP -O2
DEBUG_FLAGS = -g
LIBS = $(PKG_CONFIG_LIBS)
MAKEFLAGS += -j$(shell nproc)

BUILD_DIR = build
TARGET = $(BUILD_DIR)/gtk-app
SRC = main.c src/demo_app.c \
	src/widgets/common.c \
	src/widgets/window.c \
	src/widgets/container.c \
	src/widgets/button.c \
	src/widgets/input.c \
	src/widgets/toggle.c \
	src/widgets/display.c \
	src/widgets/date.c \
	src/widgets/separator.c \
	src/widgets/dialog.c \
	src/widgets/menu.c \
	src/widgets/theme.c

OBJ = $(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))
DEP = $(OBJ:.o=.d)

all: $(TARGET)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIBS)

$(BUILD_DIR)/%.o: %.c
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

debug: CFLAGS += $(DEBUG_FLAGS)
debug: clean all

clean:
	rm -rf $(BUILD_DIR)

run: $(TARGET)
	$(TARGET)

.PHONY: all clean debug run

-include $(DEP)
