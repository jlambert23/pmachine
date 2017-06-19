#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int kind;           // const = 1, var = 2, proc = 3
    char name[10];      // name up to 11 chars
    int val;            // number (ASCII value)
    int level;          // L level
    int addr;           // M address
} symbol;

int main(int argv, char **argc) {


}
