/*
 *  TODO: Unique variables accross activation records:
 *      - Each independent procedure should have member 
 *        variables that won't conflict or be accessible.
 *      - Unique variable names only occur when member is accessible.
 *      
 *
 *  TODO: Implement return values. (Consult recitation notes.)
 *      - Should a variable accept a return value from a function?
 *          e.g.: x := call thumbs;
 */

#include "parser.h"

// Global variables.
int token;
int level = 0;
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
        switch(getKind(i)) {
            case 1:
                printf("const\t%s\tvalue: %d\n", getName(i), getVal(i));
                break;
            case 2:
                printf("var\t%s\t\taddr: %d\n", getName(i), getAddr(i));
                break;
            case 3:
                printf("proc\t%s\taddr: %d\n", getName(i), getAddr(i));
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
/*               GETTERS                  */
/* ---------------------------------------*/

char *getName(int index) { return symbol_table[index].name; }
int getKind(int index) { return symbol_table[index].kind; }
int getAddr(int index) { return symbol_table[index].addr; }
int getLevel(int index) { return symbol_table[index].level; }
int getVal(int index) { return symbol_table[index].val; }

/* ---------------------------------------*/
/*           PARSER / CODE GEN            */
/* ---------------------------------------*/

/* Program starts here. Function is called by compiler.c */
void parser(char *inputFile, char *outputFile)
{
    openFile(inputFile);
    program();
    printCode();
    printf("No errors. Program is syntatically correct.\n");
}

// Get next token in file. Skips identifiers and stores them into codePtr.
void getToken() {
    codePtr = (!codePtr ? strtok(lexeme, " ") : strtok(NULL, " "));
    if (!codePtr) return; // EOF

    if (!isDigit(*codePtr)) {
        strcpy(idName, codePtr);
        getToken();
    }
        
    token = atoi(codePtr);
}

// Return the index if a symbol is found in the table, else return -1.
int findId() {
    for (int i = 0; i < tablePtr; i++)
        if (!strcmp(idName, getName(i)))
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
    if (findId() >= 0) error(8);

    symbol newsym;

    newsym.kind = kind;
    newsym.level = L;
    newsym.addr = M;
    strcpy(newsym.name, idName);

    // Constant initialization
    if (kind == 1) newsym.val = value;
    if (kind == 2) addr++;

    symbol_table[tablePtr] = newsym;
    tablePtr++;
}

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
            printf("Variable '%s' already exists.\n", idName);
            break;
        case 9:
            printf("Period expected.\n");
            break;
        case 10:
            printf("Call must be followed by a procedure identifier\n");
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
    getToken();

    block();
    if (token != periodsym) error(9);

    emit(SIO, 0, 3);
}

// Program block
void block() { 
    int tmpIndex = codeIndex;
    level++;

    emit(JMP, 0, 0);

    // Constant declaration
    if (token == constsym)
        constDecl();
        
    // Variable declaration
    if (token == varsym)
        varDecl();

    // Procedure declaration
    while (token == procsym)
        procDecl();
    
    // If codeIndex has not moved, increment to avoid infinite loop.
    code[tmpIndex].m = codeIndex == tmpIndex ? codeIndex + 1 : codeIndex;

    emit(INC, 0, addr);

    statement();
    level--;
}

// Constant declaration
void constDecl() {
    do {
        getToken();
        if (token != identsym) error(4);

        getToken();
        if (token != eqlsym) error(3);

        getToken();
        if (token != numbersym) error(2);

        getToken(); // value
        
        enter(const_type, token, level, 0);
        
        getToken();
    } while (token == commasym);

    if (token != semicolonsym) error(5);
    getToken();
}

// Variable declaration
void varDecl() {
    do {
        getToken();
        if (token != identsym) error(4);

        getToken();

        enter(var_type, 0, level, addr);
    } while (token == commasym);

    if (token != semicolonsym) error(5);

    getToken();
}

// Procedure declaration
void procDecl() {
    getToken();
    if (token != identsym) error(4);

    getToken();

    enter(proc_type, 0, level, codeIndex);
    
    if (token != semicolonsym) error(6);    
    getToken();

    // Define procedure and emit member instructions.
    int tmpAddr = addr;
    addr = 4;
    block();

    if (token != semicolonsym) error(6);
    
    // Return from call.
    addr = tmpAddr;
    emit(OPR, 0, 0);

    getToken();
    
}

void statement() {
    // Identifier
    if (token == identsym) {
        int symIndex;

        getToken();

        // Search for identifier in symbol table.
        if ((symIndex = findId()) < 0) error(11);
        if (getKind(symIndex) != var_type) error(12);

        if (token != becomesym) error(1);
        
        getToken();
        expression();
        emit(STO, 0, getAddr(symIndex));
    }

    // Call
    else if (token == callsym) {
        int symIndex;

        getToken();
        if (token != identsym) error (14);
        getToken();

        // Search for identifier in symbol table.
        if ((symIndex = findId()) < 0) error(11);
        if (getKind(symIndex) != proc_type) error(10);

        emit(CAL, getLevel(symIndex), getAddr(symIndex));
    } 

    // Begin
    else if (token == beginsym) {
        getToken();
        statement();
        
        while (token == semicolonsym) {
            getToken();
            statement();
        }
        
        if (token != endsym) error(7);
        getToken();
    }

    // If
    else if (token == ifsym) {
        int skip_if, skip_else;

        getToken();
        condition();

        if (token != thensym) error(16);
        getToken();

        // Save address of JPC.
        skip_if = codeIndex;
        emit(JPC, 0, 0);
        
        statement();

        // Save address of JMP.
        skip_else = codeIndex;
        emit(JMP, 0, 0);

        // Update JPC address to skip if statement.
        code[skip_if].m = codeIndex;

        getToken();

        // Else
        if (token == elsesym) {
            getToken();
            statement();
        }

        // Update JMP address to skip else statement.
        code[skip_else].m = codeIndex;
    }

    // While
    else if (token == whilesym) {
        int loop_while, exit_while;

        // Save address of beginning of while.
        loop_while = codeIndex;

        getToken();
        condition();
        
        // Save address of JPC.
        exit_while = codeIndex;
        emit(JPC, 0, 0);

        if (token != dosym) error(18);

        getToken();
        statement();

        // Loop to beginning of while.
        emit(JMP, 0, loop_while);

        // Update JPC address to exit while loop.
        code[exit_while].m = codeIndex;
    }

    // Read
    else if (token == readsym) {
        int symIndex;

        getToken();

        if (token != identsym) error(4);
        getToken();

        // Search for identifier in symbol table.
        if ((symIndex = findId()) < 0) error(11);
        if (getKind(symIndex) != var_type) error(12);

        emit(SIO, 0, 2);
        emit(STO, 0, getAddr(symIndex));
    }
    
    // Write
    else if (token == writesym) {
        getToken();
        expression();
        emit(SIO, 0, 1);
    }
}

void condition() {
    if (token == oddsym) {
        getToken();
        expression();
        emit(OPR, 0, ODD);
    }
    else {
        expression();

        int relation = token;

        getToken();
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
                error(20);
        }
    }
}

void expression() {
    int addop = token;

    if (token == plussym || token == minussym) getToken();
    term();

    if (addop == minussym)
        emit(OPR, 0, NEG);

    while (token == plussym || token == minussym) {
        addop = token;
        getToken();
        term();

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
        getToken();
        factor();

        if (mulop == multsym)
            emit(OPR, 0, MUL);
        else
            emit(OPR, 0, DIV);
    }
}

void factor() {
    if (token == identsym) {
        int symIndex;
        getToken();

        // Search for identifier in symbol table.
        if ((symIndex = findId()) < 0) error(11);

        // Load variable to stack.
        if (getKind(symIndex) == var_type)
            emit(LOD, 0, getAddr(symIndex));

        // Emit constant to stack.
        else if (getKind(symIndex) == const_type)
            emit(LIT, 0, getVal(symIndex));
    } 

    // Emit number to stack.
    else if (token == numbersym) {
        getToken();
        emit(LIT, 0, token);
        getToken();
    }

    else if (token == lparentsym) {
        getToken();
        expression();

        if (token != rparentsym) error(22);
        getToken();
    }

    else error(21);
}