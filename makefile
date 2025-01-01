
#compiler
CC = gcc

CFLAGS = -Wall `sdl-config --cflags`
LFLAGS = `sdl2-config --libs` 

SRC = main.c cpu.c

OBJ = $(SRC:.c=.o)

TARGET = chip8_em

all: $(TARGET)

# Linking executable with extra flags
$(TARGET): $(OBJ)
	@echo "Linking $@ with object files: $^"
    #@ prefixed to call will supress output
	@$(CC) $(OBJ) -o $@ $(LFLAGS)
	@echo "Executable created: $@"

# Compile source files to object files
%.o: %.c
	@echo "Compiling $< to $@"
	@$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove object files and executable
clean:
	@echo "Cleaning up..."
	@rm -f $(OBJ) $(TARGET) *.ppm
	@echo "Cleanup complete."

# Phony targets
.PHONY: all clean
