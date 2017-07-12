#include "parser.h"

// Global variables.
int token, addr = 0, tablePtr = 0, cx = 0;
char *codePtr, lexeme[MAX_CODE_LENGTH], idName[MAX_IDENTIFIER_LENGTH];

symbol symbol_table[TABLE_SIZE];
instruction code[MAX_CODE_LENGTH];

/* ------------------------------------*/
/*           INPUT / OUTPUT            */
/* ------------------------------------*/

int openFile(char *inputFile) {
    FILE *ifp;
    
    if (!(ifp = fopen(inputFile, "r"))) error(26);

    char buffer[MAX_IDENTIFIER_LENGTH];
    while (fscanf(ifp, "%s", buffer) != EOF) {
        strcat(buffer, " ");
        strcat(lexeme, buffer);
    }

    fclose(ifp);
    return 1;
}

void printTable() {
    for (int i = 0; i < tablePtr; i++) {
        switch(symbol_table[i].kind) {
            case 1:
                printf("const\t%s\t%d\n", symbol_table[i].name, symbol_table[i].val);
                break;
            case 2:
                printf("var\t%s\t\t%d\n", symbol_table[i].name, symbol_table[i].addr);
                break;
            case 3:
                printf("proc\t%s\t%d\n", symbol_table[i].name, symbol_table[i].level);
                break;
        }
    }
}

void printCode() {
    FILE *ofp;
    ofp = fopen(PAR_OUT, "w+");

    for (int i = 0; i < cx; i++)
        fprintf(ofp, "%d %d %d\n", code[i].op, code[i].l, code[i].m);

    fclose(ofp);
}

/* ---------------------------------------*/
/*           PARSER / CODE GEN            */
/* ---------------------------------------*/

// Get next token in file. Skips identifiers and stores them into codePtr.
void getToken(int start) {
    codePtr = (start == 1 ? strtok(lexeme, " ") : strtok(NULL, " "));
    if (!codePtr) return; // EOF

    if (!isDigit(*codePtr)) {
        strcpy(idName, codePtr);
        getToken(0);
    }
        
    token = atoi(codePtr);
}

// Return the index if a symbol is found in the table, else return -1.
int find(char *name) {
    for (int i = 0; i < tablePtr; i++)
        if (!strcmp(name, symbol_table[i].name))
            return i;

    return -1;
}

// Generate code for virtual machine.
void emit(int op, int l, int m) {
    if (cx > MAX_CODE_LENGTH) error(25);

    code[cx].op = op;
    code[cx].l = l;
    code[cx].m = m;
    cx++;
}

/*  Insert symbol into symbol_table.
 *  Vars and consts are not permitted to have the same name.
 *  
 *  Constants: store kind, name and value.
 *  Variables: store kind, name, L and M.
 *  Procedures: store kind, name, L and M.
 */
void enter(int kind, char *name, int value, int L, int M) {
    // Check if symbol is already in the table.
    if (find(name) >= 0) return;

    symbol newsym;

    newsym.kind = kind;
    newsym.level = L;
    newsym.addr = M;
    strcpy(newsym.name, name);

    // Constant initialization
    if (kind == 1) newsym.val = value;
    else addr++;

    symbol_table[tablePtr] = newsym;
    tablePtr++;
}

/* Program starts here. Function is called by compiler.c */
void parser(char *inputFile, char *outputFile)
{
    openFile(inputFile);
    program();
    printTable();
    printCode();
}

/* ------------------------------------------------*/
/*           TOP DOWN PARSING FUNCTIONS            */
/* ------------------------------------------------*/

void program() {
    getToken(1);
    int level = 0;

    block(level);    
    if (token != periodsym) error(9);

    emit(SIO, 0, 3);
}

// Program block
void block(int level) {    
    // Constant declaration
    if (token == constsym)
        constDecl(level);
        
    // Variable declaration
    if (token == varsym)
        varDecl(level);

    /* Lexical analyzer does not generate procsym. *//*
    // Procedure declaration
    while (token == procsym)
        procDecl(level);
    /**/

    statement();
}

// Constant declaration
void constDecl(int level) {
    do {
        getToken(0);
        if (token != identsym) error(4);

        getToken(0);
        if (token != eqlsym) error(3);

        getToken(0);
        if (token != numbersym) error(2);

        getToken(0); // value

        // param: constant, ident, value
        enter(const_type, idName, token, level, addr);
        
        getToken(0);
    } while (token == commasym);

    if (token != semicolonsym) error(5);
    getToken(0);
}

// Variable declaration
void varDecl(int level) {
    do {
        getToken(0);
        if (token != identsym) error(4);

        getToken(0);

        // param: variable, ident, level
        enter(var_type, idName, 0, level, addr);
    } while (token == commasym);

    if (token != semicolonsym) error(5);

    getToken(0);
}

// Procedure declaration
void procDecl(int level) {
    getToken(0);
    if (token != identsym) error(4);

    // param: procedure, ident
    enter(proc_type, idName, 0, level, addr);

    getToken(0);
    if (token != semicolonsym) error(6);

    getToken(0);
    block(level + 1);

    if (token != semicolonsym) error(17);

    getToken(0);
}

void statement() {
    // Identifier
    if (token == identsym) {
        getToken(0);

        /* CODE GEN STUFF */
        int i = find(idName);
        if (i < 0) error(11);                               // Undeclared identifier
        if (symbol_table[i].kind != var_type) error(12);    // Assigment to constant or procedure is not allowed.

        if (token != becomesym) error(1);
        
        getToken(0);
        expression();
        emit(STO, symbol_table[i].level, symbol_table[i].addr);
    }

    // Procedure call
    else if (token == callsym) {
        getToken(0);
        if (token != identsym) error (14);

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
        
        if (token != endsym) error(7);
        getToken(0);
    }

    // If
    else if (token == ifsym) {
        int ctemp;

        getToken(0);
        condition();
        if (token != thensym) error(16);
        getToken(0);

        /* CODE GEN STUFF */
        ctemp = cx;
        emit(JPC, 0, 0);
        
        statement();
        code[ctemp].m = cx;
    }

    // While
    else if (token == whilesym) {
        int cx1 = cx;
        int cx2;

        getToken(0);
        condition();
        
        /* CODE GEN STUFF */
        cx2 = cx;
        emit(JPC, 0, 0);

        if (token != dosym) error(18);
        getToken(0);
        statement();

        /* CODE GEN STUFF*/
        emit(JMP, 0, cx1);
        code[cx2].m = cx;
    }

    // Read
    else if (token == readsym) {
        getToken(0);

        // Read identifier.
        if (token != identsym) error(4);
        getToken(0);

        /* CODE GEN STUFF */
        int i = find(idName);
        if (i < 0) error(11);                               // Undeclared identifier
        if (symbol_table[i].kind != var_type) error(12);    // Assigment to constant or procedure is not allowed.

        emit(SIO, 0, 2);
    }
    
    // Write
    else if (token == writesym) {
        getToken(0);

        // Write identifier.
        if (token != identsym) error(4);
        getToken(0);

        /* CODE GEN STUFF */
        int i = find(idName);
        if (i < 0) error(11);                               // Undeclared identifier
        if (symbol_table[i].kind != var_type) error(12);    // Assigment to constant or procedure is not allowed.
        printf("%s\n", symbol_table[i].name);
        emit(LOD, symbol_table[i].level, symbol_table[i].addr);
        emit(SIO, 0, 1);
    }
}

void condition() {
    if (token == oddsym) {
        getToken(0);
        expression();
    }
    else {
        expression();

        if (!isRelation(token)) error(20);
        getToken(0);

        expression();
    }
}

void expression() {
    int addop = token;

    if (token == plussym || token == minussym) getToken(0);
    term();

    /* CODE GEN STUFF */
    if (addop == minussym)
        emit(OPR, 0, NEG);

    while (token == plussym || token == minussym) {
        addop = token;
        getToken(0);
        term();

        /* CODE GEN STUFF */
        if (addop == plussym)
            emit(OPR, 0, ADD);
        else
            emit(OPR, 0, SUB);
    }
}

void term() {
    int mulop;
    
    factor();

    while (token == multsym || token == slashsym) {
        mulop = token;
        getToken(0);
        factor();

        /* CODE GEN STUFF */
        if (mulop == multsym)
            emit(OPR, 0, MUL);
        else
            emit(OPR, 0, DIV);
    }
}

void factor() {
    if (token == identsym) {
        /* CODE GEN STUFF */
        int i = find(idName);
        if (i < 0) error(11);                               // Undeclared identifier
        if (symbol_table[i].kind == var_type)
            emit(LOD, symbol_table[i].level, symbol_table[i].addr);
        else if (symbol_table[i].kind == const_type)
            emit(LIT, 0, symbol_table[i].val);
        /* END CODE GEN STUFF */
    
        getToken(0);
    } 

    else if (token == numbersym) {
        getToken(0);
        emit(LIT, 0, token);
        getToken(0);
    }

    else if (token == lparentsym) {
        getToken(0);
        expression();

        if (token != rparentsym) error(22);
        getToken(0);
    }

    else error(21);
}