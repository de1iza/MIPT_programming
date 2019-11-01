//#ifndef CPU_COMMANDS_H
//#define CPU_COMMANDS_H

#define VALID_RAM_INDEX(index)                                  \
    if (0 > index || index >= RAM_SIZE) {                       \
        fprintf(stderr, "Invalid RAM index: %d \n", index);     \
        abort();                                                \
    }

DEF_CMD(PUSH, PARAM_IMMED, {
    StackPush(&cpu.stack, code[i + 2]);
})

DEF_CMD(ADD, NO_PARAMS, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    StackPush(&cpu.stack, a + b);
})

DEF_CMD(SUB, NO_PARAMS, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    StackPush(&cpu.stack, a - b);
})

DEF_CMD(MUL, NO_PARAMS, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    StackPush(&cpu.stack, a * b);
})

DEF_CMD(DIV, NO_PARAMS, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    StackPush(&cpu.stack, a / b);
})

DEF_CMD(PUSH, PARAM_REG, {
    int reg_ind = code[i + 2];
    StackPush(&cpu.stack, cpu.registers[reg_ind]);
})

DEF_CMD(POP, PARAM_REG, {
    int reg_ind = code[i + 2];
    StackPop(&cpu.stack, &cpu.registers[reg_ind]);
})

DEF_CMD(IN, NO_PARAMS, {
    int value = 0;
    printf("Enter integer value: ");
    scanf("%d", &value);
    StackPush(&cpu.stack, value);
})

DEF_CMD(OUT, NO_PARAMS, {
    int value = 0;
    StackPop(&cpu.stack, &value);
    printf("Value from stack: %d\n", value);
})

DEF_CMD(END, NO_PARAMS, {
    end_flag = true;
})

DEF_CMD(JMP, LABEL, {
    i = 3 * (code[i + 2] - 1);
})

DEF_CMD(JA, LABEL, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    if (a < b)
        i = 3 * (code[i + 2] - 1);
})

DEF_CMD(JAE, LABEL, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    if (a <= b)
        i = 3 * (code[i + 2] - 1);
})

DEF_CMD(JB, LABEL, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    if (a > b)
        i = 3 * (code[i + 2] - 1);
})

DEF_CMD(JBE, LABEL, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    if (a >= b)
        i = 3 * (code[i + 2] - 1);
})

DEF_CMD(JE, LABEL, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    if (a == b)
        i = 3 * (code[i + 2] - 1);
})

DEF_CMD(JNE, LABEL, {
    int a = 0;
    int b = 0;
    StackPop(&cpu.stack, &a);
    StackPop(&cpu.stack, &b);
    if (a != b)
        i = 3 * (code[i + 2] - 1);
})

DEF_CMD(CALL, LABEL, {
    StackPush(&cpu.calls, i);
    i = 3 * (code[i + 2] - 1);
})

DEF_CMD(RET, NO_PARAMS, {
    StackPop(&cpu.calls, &i);
})

DEF_CMD(PUSHRAM, 1, {
    int index = code[i + 2];
    VALID_RAM_INDEX(index);
    int val = cpu.RAM[index];
    StackPush(&cpu.stack, val);
    /*for (int i = 0; i < RAM_SIZE; i++) {
        printf("%d ", cpu.RAM[i]);
    }*/

})

DEF_CMD(POPRAM, 1, {
    int val = 0;
    int index = code[i + 2];
    VALID_RAM_INDEX(index);
    StackPop(&cpu.stack, &val);
    cpu.RAM[index] = val;
    /*for (int i = 0; i < RAM_SIZE; i++) {
        printf("%d ", cpu.RAM[i]);
    }*/
})

//#endif
// CPU_COMMANDS_H
