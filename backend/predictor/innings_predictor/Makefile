# Executable
TARGET = innings_predict

# Source files and object files
SRCS = innings_predict.c innings_instr.c utilities.c \
       innings_game_linker.c decision_tree.c game_dec_tree.c
OBJS = $(SRCS:.c=.o)

# Compiler and flags
CC = gcc
CFLAGS = -g -std=c17 -Wall
LDFLAGS = -lsqlite3

# Clean command
RM = rm -f

# Default target
all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	$(RM) $(TARGET) $(OBJS) *~