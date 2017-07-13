#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// I/O management.
#define IN      	"cases/SampleInput/count.txt"
#define LEX_OUT     "io/lex_out.txt"
#define PAR_OUT     "io/par_out.txt"
#define VM_OUT 		"io/vm_out.txt"

// Internal representation.
#define MAX_CODE_LENGTH 32768
#define MAX_IDENTIFIER_LENGTH 12
#define MAX_NUMBER_LENGTH 5
#define TABLE_SIZE 32

typedef struct {
    int op; 	// opcode
    int l;  	// lexicographical level
    int m;		// miscellaneous
} instruction;

/* REFERENCE FOR SYMBOL INDECES *//*
nulsym = 1, identsym = 2, numbersym = 3, plussym = 4, minussym = 5,
multsym = 6, slashsym = 7, oddsym = 8, eqlsym = 9, neqsym = 10, 
lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14, lparentsym = 15,
rparentsym = 16, commasym = 17, semicolonsym = 18, periodsym = 19, 
becomesym = 20, beginsym = 21, endsym = 22, ifsym = 23, thensym = 24,
whilesym = 25, dosym = 26, callsym = 27, constsym = 28, varsym = 29, 
writesym = 31, readsym = 32, procsym = 33;
/**/ 
enum sym {
	nulsym = 1,
	identsym,
	numbersym,
	plussym,
	minussym,
	multsym,
	slashsym,
	oddsym,
	eqlsym,
	neqsym,
	lessym,
	leqsym,
	gtrsym,
	geqsym,
	lparentsym,
	rparentsym,
	commasym,
	semicolonsym,
	periodsym,
	becomesym,
	beginsym,
	endsym,
	ifsym,
	thensym,
	whilesym,
	dosym,
	callsym,
	constsym,
	varsym,
	writesym,
 	readsym,
	procsym
};

enum opcode {
	LIT = 1,
	OPR,
	LOD,
	STO,
	CAL,
	INC,
	JMP,
	JPC,
	SIO
};

enum opr {
	RET,
	NEG,
	ADD,
	SUB,
	MUL,
	DIV,
	ODD,
	MOD,
	EQL,
	NEQ,
	LSS,
	LEQ,
	GTR,
	GEQ
};

void error(int err);

#endif