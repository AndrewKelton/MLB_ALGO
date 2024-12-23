# Main Executable
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
innings_predict: innings_predict.o $(OBJS)
# all: $(TARGET)

# Link the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Pipeline Executable
PIP_TARGET = pipeline_main

PIP_SRCS = pipeline_main.c utilities.c
PIP_OBJS = $(SRCS:.c=.o)

# Default Pipeline
pipeline_main: pipeline_main.o $(PIP_OBJS)

$(PIP_TARGET): $(PIP_OBJS)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile source files to object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	$(RM) $(TARGET) $(PIP_TARGET) $(OBJS) *~
