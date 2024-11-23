# exe
INNINGS_PREDICT = innings_predict
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

$(INNINGS_PREDICT).o: $(INNINGS_PREDICT).c innings_instr.h utilities.h
	$(CC) $(CFLAGS) -c $(INNINGS_PREDICT).c

$(INSTR).o: $(INSTR).c innings_instr.h
	$(CC) $(CFLAGS) -c $(INSTR).c

$(UTIL).o: $(UTIL).c utilities.h
	$(CC) $(CFLAGS) -c $(UTIL).c

clean:
	$(RM) *~ *.o
