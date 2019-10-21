//#ifndef CPU_COMMANDS_H
//#define CPU_COMMANDS_H

DEF_CMD(PUSH, 1, {
    StackPush(&cpu, code[i + 1]);
})

DEF_CMD(ADD, 2, {
    int a = 0;
    int b = 0;
    StackPop(&cpu, &a);
    StackPop(&cpu, &b);
    StackPush(&cpu, a + b);
})

DEF_CMD(SUB, 3, {
    int a = 0;
    int b = 0;
    StackPop(&cpu, &a);
    StackPop(&cpu, &b);
    StackPush(&cpu, a - b);
})

DEF_CMD(MUL, 4, {
    int a = 0;
    int b = 0;
    StackPop(&cpu, &a);
    StackPop(&cpu, &b);
    StackPush(&cpu, a * b);
})

DEF_CMD(DIV, 5, {
    int a = 0;
    int b = 0;
    StackPop(&cpu, &a);
    StackPop(&cpu, &b);
    StackPush(&cpu, a / b);
})

//#endif
// CPU_COMMANDS_H
