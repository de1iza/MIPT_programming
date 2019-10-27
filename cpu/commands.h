//#ifndef CPU_COMMANDS_H
//#define CPU_COMMANDS_H

DEF_CMD(PUSH, 1, 1, {
    StackPush(&cpu.stack, code[i + 1]);
})

DEF_CMD(ADD, 2, 0, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    StackPush(&cpu.stack, a + b);
})

DEF_CMD(SUB, 3, 0, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    StackPush(&cpu.stack, a - b);
})

DEF_CMD(MUL, 4, 0, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    StackPush(&cpu.stack, a * b);
})

DEF_CMD(DIV, 5, 0, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    StackPush(&cpu.stack, a / b);
})

DEF_CMD(PUSH_AX, 6, 0, {
    StackPush(&cpu.stack, cpu.registers[0]);
})

DEF_CMD(PUSH_BX, 7, 0, {
    StackPush(&cpu.stack, cpu.registers[1]);
})

DEF_CMD(PUSH_CX, 8, 0, {
    StackPush(&cpu.stack, cpu.registers[2]);
})

DEF_CMD(PUSH_DX, 9, 0, {
    StackPush(&cpu.stack, cpu.registers[3]);
})

DEF_CMD(POP_AX, 10, 0, {
    StackPop(&cpu.stack, &cpu.registers[0]);
})

DEF_CMD(POP_BX, 11, 0, {
    StackPop(&cpu.stack, &cpu.registers[1]);
})

DEF_CMD(POP_CX, 12, 0, {
    StackPop(&cpu.stack, &cpu.registers[2]);
})

DEF_CMD(POP_DX, 13, 0, {
    StackPop(&cpu.stack, &cpu.registers[3]);
})

DEF_CMD(IN, 14, 0, {
    int value = 0;
    printf("Enter integer value: ");
    scanf("%d", &value);
    StackPush(&cpu.stack, value);
})

DEF_CMD(OUT, 15, 0, {
    int value = 0;
    StackPop(&cpu.stack, &value);
    printf("Value from stack: %d\n", value);
})

DEF_CMD(JMP, 16, 1, {
    
})

//#endif
// CPU_COMMANDS_H
