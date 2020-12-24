CC = gcc
BUILD_PATH = build
EXEC = $(BUILD_PATH)/ucam
SRC  = $(wildcard src/*.c)
OBJ = $(SRC:src/%.c=$(BUILD_PATH)/%.o)

INCLUDES = -Iinc

CFLAGS = -W -Wall -g
LDFLAGS = -L/usr/lib -L$(BUILD_PATH)

all: $(BUILD_PATH) $(EXEC)

$(BUILD_PATH):
	mkdir -p $(BUILD_PATH)

DEPS = $(wildcard include/*.h)

$(BUILD_PATH)/%.o: src/%.c $(DEPS)
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

$(EXEC): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)

strip:
	strip $(EXEC)

clean:
	-@(rm -f $(EXEC) $(OBJ))
