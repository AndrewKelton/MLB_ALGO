
# exe
INNINGS_PREDICT = INNINGS_PREDICT
INSTR = innings_instr
UTIL = utilities

INNINGS_OBJECTS = $(INNINGS_PREDICT).o $(INSTR).o $(UTIL).o

# cmds
RM = rm -f

# tools
CC = gcc
CFLAGS = -g -std=c17 -Wall

$(INNINGS_PREDICT): $(INNINGS_OBJECTS) 
	$(CC) $(CFLAGS) -o $(INNINGS_PREDICT) $(INNINGS_OBJECTS)

$(INNINGS_PREDICT).o: $(INNINGS_PREDICT).c innings_instr.h utilities.h \
	$(CC) $(CFLAGS) -c $<

clean:
	$(RM) *~ *.o 

# rule for compiling individual .c files
%.o: %.c %.h
	$(CC) $(CFLAGS) -c $<