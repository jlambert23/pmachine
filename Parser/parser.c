#include <stdio.h>
#include <stdlib.h>
#include "lexicalAnalyzer.c"

#define MAX_SYMBOL_LENGTH 13
#define MAX_CODE_LENGTH 32768
#define TABLE_SIZE 32

/*
int nulsym = 1, identsym = 2, numbersym = 3, plussym = 4,
minussym = 5, multsym = 6, slashsym = 7, oddsym = 8, eqlsym = 9,
neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14,
lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
periodsym = 19, becomesym = 20, beginsym = 21, endsym = 22, ifsym = 23,
thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
varsym = 29, writesym = 31, readsym = 32;
*/

char *codePtr, code[MAX_CODE_LENGTH];

int token;

void openFile(char *inputFile) {
    FILE * ifp;
    char ch;
    int i = 0, j = 0;

    ifp = fopen(inputFile, "r");

    char buffer[MAX_IDENTIFIER_LENGTH];
    while (fscanf(ifp, "%s", buffer) != EOF) {
        strcat(buffer, " ");
        strcat(code, buffer);
    }

    fclose(ifp);
}

void error(int err) {
    switch (err)
        case 9:
            printf("Error 9: Period expected.");
        default:
            break;
        
}

void getToken(int start) { codePtr = (start == 1 ? strtok(code, " ") : strtok(NULL, " ")); }

int var_decl() {
    int declaring = 1;

    while (declaring) {
        getToken();

    }
}

int const_decl() {
    int declaring = 1;

    while (declaring) {
        getToken();
        if (!isIdentifier(token)) error(-1);    // missing identifier

        getToken();
        if (token != eqlsym) error(-1);         // = should be followed by a number

        getToken();
        if (!isDigit(token)) error(-1);         // should be a number

        getToken();
        if (token != commasym) declaring = 0;   // End declarations.
    }

    if (token != semicolonsym) error(-1);
    getToken();
}

int block() {
    switch(token) {
        // constant
        case 28:
            const_decl();
        // variable
        case 29:
            var_decl();
        // case procedure
        //    proc_decl();
        default:
            break;
    }

    statement();
}

int program() {
    getToken(1);

    if (!block())
        return -1;

    if (token != periodsym) {
        error(9);
        return -1;
    )

    return 0;
}


int parser(char *inputFile, char *outputFile)
{
    openFile(inputFile);
    program();
    

    return 0;
}