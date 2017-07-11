typedef struct {
    int kind;                              // const = 1, var = 2, proc = 3
    char name[MAX_IDENTIFIER_LENGTH];      // length in compiler.h
    int val;                               // number (ASCII value)
    int level;                             // L level
    int addr;                              // M address
} symbol;

enum kind {
    const_type = 1, 
    var_type, 
    proc_type
};

// Pre-defined functions.
int parser(char *inputFile, char *outputFile);
void error(int err);

/* Top down parsing functions */
void program();
void block(int level);
void constDecl();
void varDecl();
void procDecl();
void statement();
void condition();
void expression();
void term();
void factor();
void number();
