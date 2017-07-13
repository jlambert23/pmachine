#include "parser.h"

// Global variables.
int token;
int addr = 0;
int tablePtr = 0;
int codeIndex = 0;

char *codePtr;
char lexeme[MAX_CODE_LENGTH];
char idName[MAX_IDENTIFIER_LENGTH];

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

    for (int i = 0; i < codeIndex; i++)
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
int find() {
    for (int i = 0; i < tablePtr; i++)
        if (!strcmp(idName, symbol_table[i].name))
            return i;

    return -1;
}

// Generate code for virtual machine.
void emit(int op, int l, int m) {
    if (codeIndex > MAX_CODE_LENGTH) error(25);

    code[codeIndex].op = op;
    code[codeIndex].l = l;
    code[codeIndex].m = m;
    codeIndex++;
}

/*  Insert symbol into symbol_table.
 *  Vars and consts are not permitted to have the same name.
 *  
 *  Constants: store kind, name and value.
 *  Variables: store kind, name, L and M.
 *  Procedures: store kind, name, L and M.
 */
void enter(int kind, int value, int L, int M) {
    // Check if symbol is already in the table.
    if (find(idName) >= 0) return;

    symbol newsym;

    newsym.kind = kind;
    newsym.level = L;
    newsym.addr = M;
    strcpy(newsym.name, idName);

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
    //printTable();
    printCode();
    printf("No errors. Program is syntatically correct.\n");
}

// Errors utilized by parser.c and virtualMachine.c
void error(int err) {
    printf("Token: %d\n", token);
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

    emit(INC, 0, addr);
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
        
        enter(const_type, token, level, 0);
        
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

        enter(var_type, 0, level, addr);
    } while (token == commasym);

    if (token != semicolonsym) error(5);

    getToken(0);
}

// Procedure declaration
void procDecl(int level) {
    getToken(0);
    if (token != identsym) error(4);

    enter(proc_type, 0, level, 0);

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
        int symIndex;

        getToken(0);

        /* CODE GEN STUFF */
        if ((symIndex = find()) < 0) error(11);
        if (symbol_table[symIndex].kind != var_type) error(12);

        if (token != becomesym) error(1);
        
        getToken(0);
        expression();
        emit(STO, symbol_table[symIndex].level, symbol_table[symIndex].addr);
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
        ctemp = codeIndex;
        emit(JPC, 0, 0);
        
        statement();
        code[ctemp].m = codeIndex;
    }

    // While
    else if (token == whilesym) {
        int cx1, cx2;
        cx1 = codeIndex;

        getToken(0);
        condition();
        
        /* CODE GEN STUFF */
        cx2 = codeIndex;
        emit(JPC, 0, 0);

        if (token != dosym) error(18);
        getToken(0);
        statement();

        /* CODE GEN STUFF*/
        emit(JMP, 0, cx1);
        code[cx2].m = codeIndex;
    }

    // Read
    else if (token == readsym) {
        int symIndex;

        getToken(0);

        // Read identifier.
        if (token != identsym) error(4);
        getToken(0);

        /* CODE GEN STUFF */
        if ((symIndex = find()) < 0) error(11);
        if (symbol_table[symIndex].kind != var_type) error(12);

        emit(SIO, 0, 2);
        emit(STO, symbol_table[symIndex].level, symbol_table[symIndex].addr);
    }
    
    // Write
    else if (token == writesym) {
        int symIndex;
        getToken(0);

        // Write identifier.
        if (token != identsym) error(4);
        getToken(0);

        /* CODE GEN STUFF */
        if ((symIndex = find()) < 0) error(11);
        if (symbol_table[symIndex].kind != var_type) error(12);
        
        emit(LOD, symbol_table[symIndex].level, symbol_table[symIndex].addr);
        emit(SIO, 0, 1);
    }
}

void condition() {
    if (token == oddsym) {
        getToken(0);
        expression();
        emit(OPR, 0, ODD);
    }
    else {
        expression();

        int relation = token;

        getToken(0);
        expression();

        switch (relation) {
            case eqlsym:
                emit(OPR, 0, EQL);
                break;
            case neqsym:
                emit(OPR, 0, NEQ);
                break;
            case lessym:
                emit(OPR, 0, LSS);
                break;
            case leqsym:
                emit(OPR, 0, LEQ);
                break;
            case gtrsym:
                emit(OPR, 0, GTR);
                break;
            case geqsym:
                emit(OPR, 0, GEQ);
                break;
            default:
                error(20); // Token is not a relation symbol.
        }
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
        int symIndex;

        getToken(0);

        /* CODE GEN STUFF */
        if ((symIndex = find()) < 0) error(11);

        // Load variable to stack.
        if (symbol_table[symIndex].kind == var_type)
            emit(LOD, symbol_table[symIndex].level, symbol_table[symIndex].addr);

        // Emit constant to stack
        else if (symbol_table[symIndex].kind == const_type)
            emit(LIT, 0, symbol_table[symIndex].val);
        /* END CODE GEN STUFF */
    
        //getToken(0);
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