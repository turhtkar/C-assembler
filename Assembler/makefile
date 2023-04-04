CC = gcc
CFLAGS = -Wall -ansi -pedantic -Werror -std=c90 -g

ASSEMBLER = Assembler
OBJS = Assembler.o macro.o preAssembler.o phaser.o symbol.o machineCode.o exception.o

$(ASSEMBLER): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(ASSEMBLER)

Assembler.o: Assembler.c Assembler.h macro.h preAssembler.h phaser.h symbol.h machineCode.h exception.h
	$(CC) $(CFLAGS) -c Assembler.c

macro.o: macro.c macro.h exception.h
	$(CC) $(CFLAGS) -c macro.c

preAssembler.o: preAssembler.c preAssembler.h exception.h macro.h
	$(CC) $(CFLAGS) -c preAssembler.c

phaser.o: phaser.c phaser.h symbol.h machineCode.h token.h exception.h
	$(CC) $(CFLAGS) -c phaser.c

Assembler.o: phaser.o

symbol.o: symbol.c symbol.h machineCode.h exception.h
	$(CC) $(CFLAGS) -c symbol.c

machineCode.o: machineCode.c machineCode.h
	$(CC) $(CFLAGS) -c machineCode.c

exception.o: exception.c exception.h
	$(CC) $(CFLAGS) -c exception.c

clean:
	rm -f $(OBJS) $(ASSEMBLER)
