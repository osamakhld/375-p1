VPATH   = src include
CFLAGS  = 
CC      = g++
LDLIBS  =
LDFLAGS =

all: example

clean:
	rm -rf example.o example

example: example.o UtilityFunctions.o
	$(CC) $(CFLAGS) $^ -o $@

example.o: example.cpp RegisterInfo.h MemoryStore.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS) $(LDLIBS)
