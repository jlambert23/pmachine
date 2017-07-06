#include <stdio.h>
#include <stdlib.h>
//#include "../LexicalAnalyzer/lexicalAnalyzer.c"

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
/**/

void expression();

char *codePtr, code[MAX_CODE_LENGTH];

int token;

int isRelation(int ch) {
    return (ch == eqlsym || ch == neqsym || ch == lessym ||
            ch == leqsym || ch == gtrsym || ch == geqsym);
}

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
    switch (err) {
        case 9:
            printf("Error 9: Period expected.\n");
            break;
        default:
            printf("miscellaneous error.\n");
    }
        
}

void getToken(int start) {
    codePtr = (start == 1 ? strtok(code, " ") : strtok(NULL, " "));
    
    if (!isDigit(*codePtr))
        getToken(0);

    token = atoi(codePtr);
}

void enter() {
    return;
}

void factor() {
    if (token == identsym) getToken(0);

    else if (token == numbersym) {
        getToken(0);
        //getToken(0); // var contents
        printf("%d\n", token);
    }

    else if (token == lparentsym) {
        getToken(0);
        expression();

        if (token != rparentsym) error(-1);        // left ( has not been closed
        getToken(0);
    }

    else error(-1);                                     // identifier ( or number expected
    
}

void term() {
    factor();
    while (token == multsym || token == slashsym) {
        getToken(0);
        factor();
    }
}

void expression() {
    if (token == plussym || token == minussym) getToken(0);
    term();
    while (token == plussym || token == minussym) {
        getToken(0);
        term();
    }
}

void condition() {
    if (token == oddsym) {
        getToken(0);
        expression();
    }
    else {
        expression();
        if (!isRelation(token)) error(-1);          // relation operator missing in conditional statement
        getToken(0);
        expression();
    }
}

void statement() {
    // Identifier.
    if (token == identsym) {
        getToken(0);
        if (token != becomesym) error(-1);          // := missing in statement

        getToken(0);
        expression();
    }

    // Procedure call.
    else if (token == callsym) {
        getToken(0);
        if (token != identsym) error (-1);          // missing identifier
        getToken(0);
    } 

    // Begin.
    else if (token == beginsym) {
        getToken(0);
        statement();

        while (token == semicolonsym) {
            getToken(0);
            statement();
        }

        if (token != endsym) error(-1);             // begin must be closed with end
        getToken(0);
    }

    // If.
    else if (token == ifsym) {
        getToken(0);
        condition();

        if (token != thensym) error(-1);            // if condition must be followed by then

        getToken(0);
        statement();
    }

    // While.
    else if (token != whilesym) {
        getToken(0);
        condition();

        if (token != dosym) error(-1);              // while condition must be followed by do

        getToken(0);
        statement();
    }
}

int block(int level) {
    int declaring;
    
    // Constant declaration.
    if (token == constsym) {
        declaring = 1;
                
        while (declaring) {
            getToken(0);
            if (token != identsym) error(-1);       // missing identifier

            getToken(0);
            if (token != eqlsym) error(-1);         // identifier should be followed by =

            getToken(0);
            if (token != numbersym) error(-1);      // = should be followed by number 

            enter(); // param: constant, ident, number
            getToken(0); // var contents

            getToken(0);

            // commasym continues loop.
            if (token != commasym) declaring = 0;
        }

        if (token != semicolonsym) error(-1);       // declaration must end with ;
        getToken(0);
    }

    // Variable declaration.
    if (token == varsym) {
        declaring = 1;

        while (declaring) {
            getToken(0);
            if (token != identsym) error(-1);       // missing identifier

            getToken(0);

            enter(); // param: variable, ident, level
            
            // commasym continues loop.
            if (token != commasym) declaring = 0;
        }

        if (token != semicolonsym) error(-1);       // declaration must end with ;
        getToken(0);
    }

    
    // Call procedure.
    while (token == callsym) {
        getToken(0);
        if (token != identsym) error(-1);           // missing procedure declaration

        enter(); // param: procedure, ident

        getToken(0);
        if (token != semicolonsym) error(-1);       // procedure declaration must end with ;

        getToken(0);
        block(level + 1);

        if (token != semicolonsym) error(-1);       // no ; at the end of block
        getToken(0);
    }
    
    statement();
    return 1;
}

int program() {
    getToken(1);
    int level = 0;

    if (!block(level))
        return -1;
    
    if (token != periodsym) {
        error(9);                                   // No period at end of file.
        return -1;
    }
    
    return 1;
}


int parser(char *inputFile, char *outputFile)
{
    openFile(inputFile);
    program();
    

    return 0;
}