|====================================================================|
|    					   PM/0 COMPILER						     |
|====================================================================|

  Written by Justin Lambert for Systems Software COP3402 Summer '17.

======================================================================
                               About
======================================================================
This program simulates a compiler for the language PL/0. It parses
the syntax of an input file with PL/0 code and generates assembly
code that is interpreted by a virtual machine.

The compiler will generate errors and will fail to compile if the
given code is not syntatically correct. Otherwise, the compiler
prints a confirmation that the code is syntatically correct.

Note that the compiler is not fully representative of PL/0 as there
are several functions (e.g. procedures) that are not implemented in
its current form.


======================================================================
							  Contents
======================================================================
	- compiler.c
	- lexicalAnalyzer.c
	- parser.c
	- virtualMachine.c
	- compiler.h
	- parser.h
	- Cases (sample code)
	- I/O files
		- lex_out.txt
		- par_out.txt
		- var_out.txt


======================================================================
						   How to execute
======================================================================
	compile:			gcc -std=c99 compiler.c
	execution:			./a.out <optional flags>

	flags:
	----------------------------------------------------------------
		-f <filename> :	Takes a specified input file. *

		-l			  :	Prints list of lexemes/tokens.

		-a			  : Prints generated assembly code.

		-v			  : Prints virtual machine execution trace.


		* Default input file found at constant 'IN' in compiler.h 
	----------------------------------------------------------------

======================================================================
								 EOF
======================================================================

