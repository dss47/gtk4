CC = gcc
CFLAGS = $(shell pkg-config --cflags gtk4) -Iinclude -MMD -MP -O2
LIBS = $(shell pkg-config --libs gtk4) -lsqlite3

BUILD_DIR = build
SRC_DIR = src
TARGET = $(BUILD_DIR)/nippon-app

# On cherche spécifiquement dans le dossier src/
SRC = $(wildcard $(SRC_DIR)/*.c)
# On transforme src/fichier.c en build/fichier.o
OBJ = $(SRC:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEP = $(OBJ:.o=.d)

all: $(TARGET)

$(TARGET): $(OBJ)
	@mkdir -p $(BUILD_DIR)
	$(CC) -o $@ $(OBJ) $(LIBS)

# Règle pour compiler les fichiers de src/ vers build/
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR)

run: all
	./$(TARGET)

.PHONY: all clean run
-include $(DEP)