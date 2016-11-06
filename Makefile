VPATH   = src include
CFLAGS  =
CC      = g++
LDLIBS  =
LDFLAGS =

all: sim

clean:
	rm -rf example.o example

sim: sim.o UtilityFunctions.o
	$(CC) $(CFLAGS) $^ UtilityFunctions.o -o $@

example: example.o UtilityFunctions.o
	$(CC) $(CFLAGS) $^ UtilityFunctions.o -o $@

sim.o: sim.cpp RegisterInfo.h MemoryStore.h EndianHelpers.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS) $(LDLIBS)

example.o: example.cpp RegisterInfo.h MemoryStore.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS) $(LDLIBS)
