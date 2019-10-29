//#ifndef CPU_COMMANDS_H
//#define CPU_COMMANDS_H

#define VALID_RAM_INDEX(index)                                  \
    if (0 > index || index >= RAM_SIZE) {                       \
        fprintf(stderr, "Invalid RAM index: %d \n", index);     \
        abort();                                                \
    }

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

DEF_CMD(PUSHREG, 6, 1, {
    int reg_ind = code[i + 1];
    StackPush(&cpu.stack, cpu.registers[reg_ind]);
})

DEF_CMD(POPREG, 10, 1, {
    int reg_ind = code[i + 1];
    StackPop(&cpu.stack, &cpu.registers[reg_ind]);
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

DEF_CMD(END, 16, 0, {
    end_flag = true;
})

DEF_CMD(JMP, 17, 1, {
    i = 2 * code[i + 1] - 2;
})

DEF_CMD(JA, 18, 1, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    if (a < b)
        i = 2 * code[i + 1] - 2;
})

DEF_CMD(JAE, 19, 1, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    if (a <= b)
        i = 2 * code[i + 1] - 2;
})

DEF_CMD(JB, 20, 1, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    if (a > b)
        i = 2 * code[i + 1] - 2;
})

DEF_CMD(JBE, 21, 1, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    if (a >= b)
        i = 2 * code[i + 1] - 2;
})

DEF_CMD(JE, 22, 1, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    if (a == b)
        i = 2 * code[i + 1] - 2;
})

DEF_CMD(JNE, 23, 1, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    if (a != b)
        i = 2 * code[i + 1] - 2;
})

DEF_CMD(CALL, 24, 1, {
    StackPush(&cpu.calls, i);
    i = 2 * code[i + 1] - 2;
})

DEF_CMD(RET, 25, 0, {
    StackPop(&cpu.calls, &i);
})

DEF_CMD(PUSHRAM, 26, 1, {
    int index = code[i + 1];
    VALID_RAM_INDEX(index);
    int val = cpu.RAM[index];
    StackPush(&cpu.stack, val);
    /*for (int i = 0; i < RAM_SIZE; i++) {
        printf("%d ", cpu.RAM[i]);
    }*/

})

DEF_CMD(POPRAM, 27, 1, {
    int val = 0;
    int index = code[i + 1];
    VALID_RAM_INDEX(index);
    StackPop(&cpu.stack, &val);
    cpu.RAM[index] = val;
    /*for (int i = 0; i < RAM_SIZE; i++) {
        printf("%d ", cpu.RAM[i]);
    }*/
})

//#endif
// CPU_COMMANDS_H
