#include <stdio.h>
#include <stdlib.h>

#include "LexicalAnalyzer/lexicalAnalyzer.c"
#include "Parser/parser.c"
//#include "pm0vm.c"

#define LEX_IN      "Parser/Sample Input/count.txt"
#define LEX_OUT     "Parser/in.txt"

#define PAR_OUT     "Parser/out.txt"

typedef struct {
    int kind;           // const = 1, var = 2, proc = 3
    char name[10];      // name up to 11 chars
    int val;            // number (ASCII value)
    int level;          // L level
    int addr;           // M address
} symbol;

typedef enum { false, true } bool;

typedef struct { bool l, a, v; } flag;

/*
int createParserInput() {
    FILE *ifPar, *ofLex;
    
    char buffer[32];

    if (!(ofLex = fopen(LEX_OUT, "r"))) {
        printf("Error: %s cannot be opened. Address defined in LEX_OUT.\n", LEX_OUT);
        return 0;
    }

    // Sanitize LEX_OUT for parser.    
    while (fscanf(ofLex, "%s", buffer) != EOF) {
        if (buffer == "Symbolic" || buffer == "Lexeme" || buffer == "List")
            continue;

        fprintf(ifPar, "%s ", buffer);
    }

    fclose(ofLex);
    fclose(ifPar);
}
*/

int main(int argc, char **argv) {
    flag dir;
    
    // Check and execute compiler directives.
    int ptr;
    for (ptr = 1; ptr < argc && argv[ptr][0] == '-'; ptr++) {
        switch(argv[ptr][1]) {
            // Print list of lexemes/tokens to the screen.
            case 'l':
                dir.l = true;
                break;
            // Print generated assembly code to the screen.
            case 'a':
                dir.a = true;
                break;
            // Print vm execution trace to the screen.
            case 'v':
                dir.v = true;
                break;
        }
    }

    lexicalAnalyzer(LEX_IN, LEX_OUT);

    //createParserInput();
    parser(LEX_OUT, PAR_OUT);

    return 0;
}
