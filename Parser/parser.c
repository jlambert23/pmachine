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
    switch (err)
        case 9:
            printf("Error 9: Period expected.\n");
        
}

void getToken(int start) {
     codePtr = (start == 1 ? strtok(code, " ") : strtok(NULL, " "));
     //printf("%s\n", codePtr);
     if (isDigit(*codePtr))
        token = atoi(codePtr);
}

void factor() {
    if (token == identsym) getToken(0);

    // TODO: better # finder
    else if (isDigit(token)) getToken(0);

    else if (token != rparentsym) error(-1);
    getToken(0);
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
        if (!isRelation(token)) error(-1);
        getToken(0);
        expression();
    }
}

void statement() {
    // Identifier.
    if (token == identsym) {
        getToken(0);
        if (token != becomesym) error(-1);

        getToken(0);
        expression();
    }

    // Procedure call.
    else if (token == callsym) {
        getToken(0);
        if (token != identsym) error (-1);
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

        if (token != endsym) error(-1);
        getToken(0);
    }

    // If.
    else if (token == ifsym) {
        getToken(0);
        condition();

        if (token != thensym) error(-1);         // Need then

        getToken(0);
        statement();
    }

    // While.
    else if (token != whilesym) {
        getToken(0);
        condition();

        if (token != dosym) error(-1);

        getToken(0);
        statement();
    }
}

int block() {
    int declaring;
    
    // Constant declaration.
    if (token == constsym) {
        declaring = 1;
            
        while (declaring) {
            getToken(0);
            if (token != identsym) error(-1);       // missing identifier

            getToken(0);
            if (token != eqlsym) error(-1);         // = should be followed by a number

            // TODO: use better # check
            getToken(0);
            if (!isDigit(token)) error(-1);         // should be a number 

            getToken(0);
            if (token != commasym) declaring = 0;   // , continues loop
        }

        if (token != semicolonsym) error(-1);       // missing ;
        getToken(0);
    }

    // Variable declaration.
    if (token == varsym) {
        declaring = 1;

        while (declaring) {
            getToken(0);
            if (token != identsym) error(-1);       // missing identifier

            getToken(0);
            if (token != commasym) declaring = 0;   // , continues loop
        }

        if (token != semicolonsym) error(-1);       // missing ;
        getToken(0);
    }

    // Call procedure.
    while (token = callsym) {
        getToken(0);
        if (token != identsym) error(-1);       // missing identifier

        getToken(0);
        if (token != semicolonsym)              // missing ;

        getToken(0);
        block();                                // call procedure

        if (token != semicolonsym) error(-1);   // missing ;
        getToken(0);
    }

    statement();
    return 1;
}

int program() {
    getToken(1);
    
    if (!block())
        return -1;

    if (token != periodsym) {
        error(9);
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