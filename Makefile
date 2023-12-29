CC = gcc
CFLAGS = -Wall -I src/include
LDFLAGS = -L src/lib
LIBS = -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32

SRC_DIR = src
INCLUDE_DIR = $(SRC_DIR)/include

# Retrieve all .c files recursively in the include directory
SOURCES = $(shell find $(INCLUDE_DIR) -name '*.c')

# Generate object file names from source files
OBJECTS = $(SOURCES:.c=.o)

# Target executable name
TARGET = retro_fps

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(SRC_DIR)/$(TARGET) $(SRC_DIR)/main.c $(OBJECTS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(SRC_DIR)/$(TARGET)
