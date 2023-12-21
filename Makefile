CC = gcc
CFLAGS = -Wall -I src/include
LDFLAGS = -L src/lib -lmingw32 -lSDL2main -lSDL2 -lopengl32 -lglu32
SRC_DIR = src
OBJ_DIR = obj

SRCS = $(wildcard $(SRC_DIR)/include/*.c) $(SRC_DIR)/main.c
OBJS = $(patsubst $(SRC_DIR)/%, $(OBJ_DIR)/%, $(SRCS:.c=.o))

TARGET = $(SRC_DIR)/retro_fps

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJ_DIR)/*.o $(TARGET)
