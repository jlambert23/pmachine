#ifndef COMPILER_H
#define COMPILER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// I/O management.
#define IN      "cases/case2.txt"
#define LEX_OUT     "io/lex_out.txt"
#define PAR_OUT     "io/par_out.txt"
#define VM_OUT 		"io/vm_out.txt"

// Internal representation.
#define MAX_CODE_LENGTH 32768
#define MAX_IDENTIFIER_LENGTH 12
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

// Errors utilized by parser.c and virtualMachine.c
void error(int err) {
    printf ("Error %d: ", err);

    switch (err) {
        case 1:
            printf("':=' missing in statement.\n");
            break;
        case 2:
            printf("'=' must be followed by a number.\n");
            break;
        case 3:
            printf("Identifier must be followed by '='.\n");
            break;
        case 4:
            printf("'const', 'var', 'procedure' must be followed by identifier.\n");
            break;
        case 5:
            printf("Semicolon or comma missing.\n");
            break;
        case 6:
            printf("Procedure declaration must end with a semicolon.\n");
            break;
        case 7:
            printf("Expected 'end' after 'begin'.\n");
            break;
        case 8:
            printf("Incorrect symbol after statement part in block.\n");
            break;
        case 9:
            printf("Period expected.\n");
            break;
        case 10:
            printf("Semicolon between statements missing.\n");
            break;
        case 11:
            printf("Undeclared identifier.\n");
            break;
        case 12:
            printf("Assignment to constant or procedure is not allowed.\n");
            break;
		case 13:
			printf("Failed to execute virtual machine.\n");
			break;
        case 14:
            printf("call must be followed by an identifier.\n");
            break;
        case 16:
            printf("Expected 'then' after 'if' condition.\n");
            break;
        case 17:
            printf("Semicolon or '}' expected.\n");
            break;
        case 18:
            printf("Expected 'do' after 'while' condition.\n");
            break;
        case 20:
            printf("Relational operator expected.\n");
            break;
        case 21:
            printf("Identifier, '(', or number expected.\n");
            break;
        case 22:
            printf("Right parenthesis missing.\n");
            break;
        case 25:
            printf("This number is too large.\n");
            break;
		case 26:
			printf("Unable to read parser input file. See 'LEX_OUT' in 'compiler.h'.\n");
			break;
		case 27:
			printf("Unable to read virtual machine input file. See 'PAR_OUT' in 'compiler.h'.\n");
			break;
        default:
            printf("Undefined error.\n");
            break;
    }
    
    exit(0);
}

#endif