# exe
INNINGS_PREDICT = innings_predict
INSTR = innings_instr
UTIL = utilities
LINKER = innings_game_linker

INNINGS_OBJECTS = $(INNINGS_PREDICT).o $(INSTR).o $(UTIL).o $(LINKER).o

# cmds
RM = rm -f

# tools
CC = gcc
CFLAGS = -g -std=c17 -Wall
LDFLAGS = -lsqlite3


$(INNINGS_PREDICT): $(INNINGS_OBJECTS)
	$(CC) $(CFLAGS) -o $(INNINGS_PREDICT) $(INNINGS_OBJECTS) $(LDFLAGS)

$(INNINGS_PREDICT).o: $(INNINGS_PREDICT).c innings_instr.h utilities.h innings_game_linker.h
	$(CC) $(CFLAGS) -c $(INNINGS_PREDICT).c

$(INSTR).o: $(INSTR).c innings_instr.h
	$(CC) $(CFLAGS) -c $(INSTR).c

$(UTIL).o: $(UTIL).c utilities.h
	$(CC) $(CFLAGS) -c $(UTIL).c

$(LINKER).o: $(LINKER).c innings_game_linker.h
	$(CC) $(CFLAGS) -c $(LINKER).c

clean:
	$(RM) *~ *.o