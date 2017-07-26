/* Justin Lambert
 * COP 3402: Systems Software
 * 5/28/17
 *
 * --------------------
 *   virtualMachine.c 
 * --------------------
 * This program is a VM simulating a stack machine known as P-machine (PM/0).
 * The program takes an input file  containing instructions under the PM/0 ISA,
 * executes said instructions, and prints the programs operations 
 * in output file in "VM_OUT.txt".
 */

#define MAX_STACK_HEIGHT 2000

const char *opcode[] = {"lit", "opr", "lod", "sto", "cal", "inc", "jmp", "jpc", "sio"};

typedef struct Registers {
    instruction *ir;
    int sp;
    int bp;
    int pc;
} registers;


/* Function credit: Euripides Montagne */
// Find base L levels down.
// mode = 1 for SL
// mode = 2 for DL
int base(int *stack, int l, int base, int mode) {
    int bl = base;
    
    while (l > 0) {
        bl = stack[bl + mode];
        l--;
    }

    return bl;
}

// Print instructions passed to VM to output file.
void print_instructions(FILE **ofp, instruction **code, int n) {
    int i;

    *ofp = fopen(VM_OUT, "w");
    /*
    fprintf(*ofp, "Line\tOP\tL\tM\n");

    for (i = 0; i < n; i++)
        fprintf(*ofp, "%*d\t%s\t%d\t%d\n", 3, i, opcode[code[i]->op - 1], code[i]->l, code[i]->m);

    fprintf(*ofp, "\n");
    */
}

// Print instructions, registers, and stack during execution to output file.
void print_execution(FILE *ofp, int *stack, registers *reg, int call) {
    int i, n;

    fprintf(ofp, "%s\t%d\t%d\t\t%d\t%d\t%d\t", 
        opcode[reg->ir->op - 1], reg->ir->l, reg->ir->m, reg->pc, reg->bp, reg->sp);
    
    // Print stack.
    for (i = 1, n = reg->sp; i <= n; i++) {
        fprintf(ofp, "%d ", stack[i]);

        // Peek at new activation record during CAL instruction.
        if (reg->ir->op == CAL && reg->bp > n)
            n += 4;
        
        if (reg->bp > 1) {
            int arIndex = base(stack, call - 1, reg->bp, 2);

            // Print vertical bars to distinguish activation records.
            if (i == arIndex - 1 && i != n) {
                fprintf(ofp, "| ");
                call--;
            }
            
            // Leaf case for when a procedure is the first AR of the stack.
            else if (arIndex == 1)
                call--;
        }
        
    }
        
    fprintf(ofp, "\n");
}

// Open file at argc[1] or FILE_NAME when no argument is given.
// Return 0 when file is unable to be read, otherwise return 1.
void open_file(FILE **ofp, char *filename, instruction ***code) {
    FILE *ifp;
    char buffer[32];
    int i;

    // Terminate program if file is unable to be read.
    if (!(ifp = fopen(filename, "r"))) error(27);

    // Dynamically allocate memory for instructions
    *code = malloc(MAX_CODE_LENGTH * sizeof(instruction));

    for (i = 0; i < MAX_CODE_LENGTH; i++)
        (*code)[i] = malloc(sizeof(instruction));

    // Load and store instructions into array.
    for (i = 0; fscanf(ifp, "%s", buffer) != EOF; i++) {
        (*code)[i]->op = atoi(buffer);

        fscanf(ifp, "%s", buffer);
        (*code)[i]->l = atoi(buffer);

        fscanf(ifp, "%s", buffer);
        (*code)[i]->m = atoi(buffer);
    }   

    print_instructions(ofp, *code, i);

    fclose(ifp);
}

// Operations called when op = 2 (mostly logic/arithmetic).
// Return 0 when invalid instruction is passed, otherwise return 1.
int opr(int *stack, registers *reg, int *call) {
    switch(reg->ir->m) {
        // RET
        case 0:
            (*call)--;
            reg->sp = reg->bp - 1;
            reg->pc = stack[reg->sp + 4];
            reg->bp = stack[reg->sp + 3];
            return 1;
        // NEG
        case 1:
            stack[reg->sp] = -stack[reg->sp];
            return 1;
        // ADD
        case 2:
            reg->sp--;
            stack[reg->sp] = stack[reg->sp] + stack[reg->sp + 1];
            return 1;
        // SUB
        case 3:
            reg->sp--;
            stack[reg->sp] = stack[reg->sp] - stack[reg->sp + 1];
            return 1;
        // MUL
        case 4:
            reg->sp--;
            stack[reg->sp] = stack[reg->sp] * stack[reg->sp + 1];
            return 1;
        // DIV
        case 5:
            reg->sp--;
            stack[reg->sp] = stack[reg->sp] / stack[reg->sp + 1];
            return 1;
        // ODD
        case 6:
            stack[reg->sp] = stack[reg->sp] % 2;
            return 1;
        // MOD
        case 7:
            reg->sp--;
            stack[reg->sp] = stack[reg->sp] % stack[reg->sp + 1];
            return 1;
        // EQL
        case 8:
            reg->sp--;
            stack[reg->sp] = (stack[reg->sp] == stack[reg->sp + 1]);
            return 1;
        // NEQ
        case 9:
            reg->sp--;
            stack[reg->sp] = (stack[reg->sp] != stack[reg->sp + 1]);
            return 1;
        // LSS
        case 10:
            reg->sp--;
            stack[reg->sp] = (stack[reg->sp] < stack[reg->sp + 1]);
            return 1;
        // LEQ
        case 11:
            reg->sp--;
            stack[reg->sp] = (stack[reg->sp] <= stack[reg->sp + 1]);
            return 1;
        // GTR
        case 12:
            reg->sp--;
            stack[reg->sp] = (stack[reg->sp] > stack[reg->sp + 1]);
            return 1;
        // GEQ
        case 13:
            reg->sp--;
            stack[reg->sp] = (stack[reg->sp] >= stack[reg->sp + 1]);
            return 1;
        // Invalid instruction: return 0.
        default:
            return 0;
        }
}

// Execute instructions and call print_execution on current instruction.
// Return -1 when invalid instruction is passed or 0 when program is terminated;
// otherwise return 1.
int execution_cycle(FILE *ofp, int *stack, registers *reg, int *call) {
    int run = 1;

    switch (reg->ir->op) {
        // LIT
        case LIT:
            reg->sp++;
            stack[reg->sp] = reg->ir->m;
            break;
        // OPR
        case OPR:
            // Return -1 when invalid instruction is passed to opr().
            if (!opr(stack, reg, call))
                run = -1;
            break;
        // LOD
        case LOD:
            reg->sp++;
            stack[reg->sp] = stack[base(stack, reg->ir->l, reg->bp, 1) + reg->ir->m];
            break;
        // STO
        case STO:
            stack[base(stack, reg->ir->l, reg->bp, 1) + reg->ir->m] = stack[reg->sp];
            reg->sp--;
            break;
        // CAL
        case CAL:
            (*call)++;
            stack[reg->sp + 1] = 0;                                    // Function Value (FV)
            stack[reg->sp + 2] = base(stack, reg->ir->l, reg->bp, 1);  // Static Link (SL)
            stack[reg->sp + 3] = reg->bp;                              // Dynamic Link (DL)
            stack[reg->sp + 4] = reg->pc;                              // Return Address (RA)
            reg->bp = reg->sp + 1;
            reg->pc = reg->ir->m;
            break;
        // INC
        case INC:
            reg->sp += reg->ir->m;
            break;
        // JMP
        case JMP:
            reg->pc = reg->ir->m;
            break;
        // JPC
        case JPC:
            if (stack[reg->sp] == 0)
                reg->pc = reg->ir->m;

            reg->sp--;
            break;
        // SIO
        case SIO:
            switch(reg->ir->m) {
                // Print top of stack.
                case 1:
                    printf("%d\n", stack[reg->sp]);
                    reg->sp--;
                    break;
                // Write to top of stack.
                case 2:
                    reg->sp++;
                    scanf("%d", &stack[reg->sp]);
                    break;
                // Halt program.
                case 3:
                    reg->pc = 0;
                    reg->bp = 0;
                    reg->sp = 0;
                    run = 0;
                    break;
            }
            break;
        // Invalid instruction: return -1.
        default:
            run = -1;
            break;
        } 

        print_execution(ofp, stack, reg, *call);
        return run;
}

void cleanup(instruction **code, registers *reg, int *stack, FILE *ofp) {
    int i;
    for (i = 0; i < MAX_CODE_LENGTH; i++)
        free(code[i]);
    free(code);
    free(reg);
    free(stack);    
    fclose(ofp);
}

// Operates virtual machine to simulate PM/0.
// Return 0 if program terminated successfully, else return -1.
int virtualMachine(char *inputFile) {
    FILE *ofp;
    instruction **code;
    int *stack;
    int call = 0;
    int run = 1;    

    open_file(&ofp, inputFile, &code);
    
    // Instantiate registers.
    registers *reg = malloc(sizeof(registers));
    reg->sp = 0;
    reg->bp = 1;
    reg->pc = 0;
    
    // Instantiate data stack.
    stack = calloc(MAX_STACK_HEIGHT, sizeof(int));
    
    // Print preparation info to output file.
    fprintf(ofp, "\t\t\t\t\tpc\tbp\tsp\tstack\n");
    fprintf(ofp, "Initial values \t\t\t\t%d\t%d\t%d\n", reg->sp, reg->bp, reg->pc);

    // Instruction fetch and execution:
    while (run > 0)
    {
        // Fetch Cycle
        fprintf(ofp, "%*d\t", 3, reg->pc);
        reg->ir = code[reg->pc];
        reg->pc++;
        
        // Execution Cycle. Sets run to -1 upon failure.
        run = execution_cycle(ofp, stack, reg, &call);
    }

    cleanup(code, reg, stack, ofp);

    // Program failed to terminate appropriately; notify user.
    if (run == -1) error (13);

    return run;
}