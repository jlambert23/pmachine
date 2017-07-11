#include "compiler.h"
#include "LexicalAnalyzer/lexicalAnalyzer.c"
#include "Parser/parser.c"
#include "VirtualMachine/virtualMachine.c"

char lexin[64];

void printFile(char *file) {
    char ch;
    FILE *ifp = fopen(file, "r");

    while ((ch = getc(ifp)) != EOF)
        printf("%c", ch);
    printf("\n");
    fclose(ifp);
}

// Provide custom input file utilizing '-f' flag.
char *inputFile(int argc, char **argv) {
    for (int i = 1; i < argc && argv[i][0] == '-'; i++)
        if (argv[i][1] == 'f') return argv[i + 1];
    return IN;
}

// Check flags and print respective files.
void directives(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] == '-')
            switch(argv[i][1]) {
                // Print list of lexemes/tokens to the screen.
                case 'l':
                    printf("%cLexeme List:\n", (i > 1 ? '\n' : '\0'));
                    printFile(LEX_OUT);
                    break;
                // Print generated assembly code to the screen.
                case 'a':
                    printf("%cAssembly code:\n", (i > 1 ? '\n' : '\0'));
                    printFile(PAR_OUT);
                    break;
                // Print vm execution trace to the screen.
                case 'v':
                    printf("%cVirtual Machine execution:\n", (i > 1 ? '\n' : '\0'));
                    printFile(VM_OUT);
                    break;
        }
    }
}

int main(int argc, char **argv) {
    strcpy(lexin, inputFile(argc, argv));
    
    lexicalAnalyzer(lexin, LEX_OUT);
    parser(LEX_OUT, PAR_OUT);
    virtualMachine(PAR_OUT);

    directives(argc, argv);
    return 0;
}
