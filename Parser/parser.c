/*
int nulsym = 1, identsym = 2, numbersym = 3, plussym = 4,
minussym = 5, multsym = 6, slashsym = 7, oddsym = 8, eqlsym = 9,
neqsym = 10, lessym = 11, leqsym = 12, gtrsym = 13, geqsym = 14,
lparentsym = 15, rparentsym = 16, commasym = 17, semicolonsym = 18,
periodsym = 19, becomesym = 20, beginsym = 21, endsym = 22, ifsym = 23,
thensym = 24, whilesym = 25, dosym = 26, callsym = 27, constsym = 28,
varsym = 29, writesym = 31, readsym = 32;
/**/ 

typedef struct {
    int kind;                              // const = 1, var = 2, proc = 3
    char name[MAX_IDENTIFIER_LENGTH];      // length in compiler.h
    int val;                               // number (ASCII value)
    int level;                             // L level
    int addr;                              // M address
} symbol;

enum kind {
    const_type= 1, var_type, proc_type
};

// Pre-defined functions.
void expression();
int block();

// Global variables.
symbol symbol_table[TABLE_SIZE];

int token, tablePtr = 0;

char *codePtr,
      code[MAX_CODE_LENGTH],
      idName[MAX_IDENTIFIER_LENGTH];

void printTable() {
    for (int i = 0; i < tablePtr; i++) {
        switch(symbol_table[i].kind) {
            case 1:
                printf("const\t%s\t%d\n", symbol_table[i].name, symbol_table[i].val);
                break;
            case 2:
                printf("var\t%s\t\t%d\n", symbol_table[i].name, symbol_table[i].val);
                break;
            case 3:
                printf("proc\t%s\t%d\n", symbol_table[i].name, symbol_table[i].level);
        }
    }
}

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

int find(char *name) {
    for (int i = 0; i < tablePtr; i++)
        if (strcmp(name, symbol_table[i].name))
            return i;
        else
            return -1;
}

void update() {
    ;
}

/*
For constants, you must store kind, name and value.
For variables, you must store kind, name, L and M.
For procedures, you must store kind, name, L and M.
*/
void enter(int kind, char *name, int value, int L) {
    // Check if symbol is already in the table.
    // same name vars and consts ares not permitted.
    if (find(name) < 0)
        return;

    symbol newsym;

    newsym.kind = kind;
    newsym.level = L;
    strcpy(newsym.name, name);

    // Constant initialization
    if (kind == 1) newsym.val = value;

    symbol_table[tablePtr] = newsym;
    tablePtr++;
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

char *getToken(int start) {
    codePtr = (start == 1 ? strtok(code, " ") : strtok(NULL, " "));
    
    if (!isDigit(*codePtr)) {
        strcpy(idName, codePtr);
        getToken(0);
    }
        
    token = atoi(codePtr);
}

void factor() {
    if (token == identsym) getToken(0);

    else if (token == numbersym) {
        getToken(0);
        //printf("%s\n", idName);
        getToken(0); // var contents
    }

    else if (token == lparentsym) {
        getToken(0);
        expression();

        if (token != rparentsym) error(-1);             // left ( has not been closed
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
        if (!isRelation(token)) error(-1);          // missing relation operator in conditional statement
        getToken(0);
        expression();
    }
}

void statement() {
    // Identifier
    if (token == identsym) {
        getToken(0);

        /* CODE GEN STUFF */
        int i = find(idName);
        if (i < 0) error(-1);                       // Undeclared identifier
        if (symbol_table[i].kind != var_type);      // Assigment to constant or procedure is not allowed.
        /**/

        if (token != becomesym) error(-1);          // := missing in statement

        getToken(0);
        expression();
        emit();
    }

    // Procedure call
    else if (token == callsym) {
        getToken(0);
        if (token != identsym) error (-1);          // missing identifier

        getToken(0);
    } 

    // Begin
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

    // If
    else if (token == ifsym) {
        getToken(0);
        condition();

        if (token != thensym) error(-1);            // if condition must be followed by then

        getToken(0);
        statement();
    }

    // While
    else if (token == whilesym) {
        getToken(0);
        condition();

        if (token != dosym) error(-1);              // while condition must be followed by do

        getToken(0);
        statement();
    }

    // Read
    else if (token == readsym) {
        getToken(0);

        // Read identifier.
        if (token != identsym) error(-1);     // missing identifier.

        getToken(0);
        statement();
    }
    
    // Write
    else if (token == writesym) {
        getToken(0);

        // Write identifier.
        if (token != identsym) error(-1);     // missing identifier.

        getToken(0);
        statement();        
    }
}

// Procedure/call
void procDecl(int level) {
    getToken(0);
    if (token != identsym) error(-1);           // missing procedure declaration

    enter(proc_type, idName, 0, level);         // param: procedure, ident

    getToken(0);
    if (token != semicolonsym) error(-1);       // procedure declaration must end with ;

    getToken(0);
    block(level + 1);

    if (token != semicolonsym) error(-1);       // no ; at the end of block
    getToken(0);
}

// Variable declaration
void varDecl(int level) {
    do {
        getToken(0);
        if (token != identsym) error(-1);       // missing identifier

        getToken(0);
        enter(var_type, idName, 0, level);             // param: variable, ident, level            
    } while (token == commasym);                // commasym continues loop.

    if (token != semicolonsym) error(-1);       // declaration must end with ;
    getToken(0);
}

// Constant declaration
void constDecl(int level) {
    do {
        getToken(0);
        if (token != identsym) error(-1);       // missing identifier

        getToken(0);
        if (token != eqlsym) error(-1);         // identifier should be followed by =

        getToken(0);
        if (token != numbersym) error(-1);      // = should be followed by number 

        getToken(0);                            // var contents
        enter(const_type, idName, token, level);       // param: constant, ident, number
        
        getToken(0);
    } while (token == commasym);                // commasym continues loop.

    if (token != semicolonsym) error(-1);       // declaration must end with ;
    getToken(0);
}

// Program block
int block(int level) {    
    // Constant declaration
    if (token == constsym)
        constDecl(level);

    // Variable declaration
    if (token == varsym)
        varDecl(level);
    
    // Call main procedure
    while (token == callsym)
        procDecl(level);
    
    statement();
    return 1;
}

// Start program
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
    printTable();

    return 0;
}