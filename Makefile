VPATH   = src include
CFLAGS  = -g
CC      = g++
LDLIBS  =
LDFLAGS =

all: sim single.bin

clean:
	rm -rf example.o sim.o example sim fib.elf fib.bin single.elf single.bin

sim.o: sim.cpp RegisterInfo.h MemoryStore.h EndianHelpers.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS) $(LDLIBS)

example.o: example.cpp RegisterInfo.h MemoryStore.h
	$(CC) $(CFLAGS) -c $< -o $@ $(LDFLAGS) $(LDLIBS)

sim: sim.o UtilityFunctions.o
	$(CC) $(CFLAGS) $^ -o $@

example: example.o UtilityFunctions.o
	$(CC) $(CFLAGS) $^ -o $@

fib.bin: fib_example.asm
	mips-linux-gnu-as -march=mips32 fib_example.asm -o fib.elf
	mips-linux-gnu-objcopy fib.elf -j .text -O binary fib.bin

single.bin: single.asm
	mips-linux-gnu-as -march=mips32 single.asm -o single.elf
	mips-linux-gnu-objcopy single.elf -j .text -O binary single.bin
	sim single.bin
	more reg_state.out
