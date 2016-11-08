VPATH   = src include
CFLAGS  =
CC      = g++
LDLIBS  =
LDFLAGS =

all: sim

clean:
	rm -rf example.o sim.o example sim

sim.o: sim.cpp RegisterInfo.h MemoryStore.h EndianHelpers.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS) $(LDLIBS)

example.o: example.cpp RegisterInfo.h MemoryStore.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS) $(LDLIBS)

sim: sim.o UtilityFunctions.o
	$(CC) $(CFLAGS) $^ -o $@

example: example.o UtilityFunctions.o
	$(CC) $(CFLAGS) $^ -o $@
