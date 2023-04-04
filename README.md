# C-assembler
a 2 steps assembler in c (works with linux c90 standard and makefile)
======================================================================
this project takes an assembly file with a given limation of the assembly instructions that it can assemble, 
valid instructions inside the assembly file are as listed below
-mov
-cmp
-add
-sub
-not
-clr
-lea
-inc
-dec
-jmp
-bne
-red
-prn
-jsr
-rts
-stop

although more instructions can be easily added by adding them to the opMap table, in the pharser.c file.
the assembly can also take .data .string .entry and .extern lines
this project also preassemble any macros as well.

in order to run this project you need to call .\assembler [name of assembly file] [name of assembly file] [name of assembly file]....

this project was made soley by me as a final univirsty project.
