#include "compiler.h"
#include "LexicalAnalyzer/lexicalAnalyzer.c"
#include "Parser/parser.c"
#include "Parser/codeGen.c"

typedef struct { bool l, a, v; } flag;

int main(int argc, char **argv) {
    flag dir;
    
    // Check and flag compiler directives.
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

    char lexin[64];
    strcpy(lexin, argc > 1 ? argv[1] : LEX_IN);
    
    lexicalAnalyzer(lexin, LEX_OUT);
    parser(LEX_OUT, PAR_OUT);

    return 0;
}
