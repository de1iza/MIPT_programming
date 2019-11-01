//#ifndef CPU_COMMANDS_H
//#define CPU_COMMANDS_H
//#include "colours_enum.h"

#define VALID_RAM_INDEX(index)                                  \
    if (0 > index || index >= RAM_SIZE) {                       \
        fprintf(stderr, "Invalid RAM index: %d \n", index);     \
        abort();                                                \
    }

#define VALID_VRAM_INDEX(index)                                  \
    if (0 > index || index >= VRAM_SIZE) {                       \
        fprintf(stderr, "Invalid VRAM index: %d \n", index);     \
        abort();                                                 \
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

DEF_CMD(PUSH, PARAM_RAM_IMMED, {
    int index = code[i + 2];
    VALID_RAM_INDEX(index);
    int val = cpu.RAM[index];
    StackPush(&cpu.stack, val);
})

DEF_CMD(POP, PARAM_RAM_IMMED, {
    int val = 0;
    int index = code[i + 2];
    VALID_RAM_INDEX(index);
    StackPop(&cpu.stack, &val);
    cpu.RAM[index] = val;
})

DEF_CMD(PUSH, PARAM_RAM_REG, {
    int reg_ind = code[i + 2];
    int RAM_ind = cpu.registers[reg_ind];
    VALID_RAM_INDEX(RAM_ind);
    int val = cpu.RAM[RAM_ind];
    StackPush(&cpu.stack, val);
})

DEF_CMD(POP, PARAM_RAM_REG, {
    int val = 0;
    int reg_ind = code[i + 2];
    int RAM_ind = cpu.registers[reg_ind];
    VALID_RAM_INDEX(RAM_ind);
    StackPop(&cpu.stack, &val);
    cpu.RAM[RAM_ind] = val;
})

DEF_CMD(PUSH, PARAM_VRAM_IMMED, {
    int index = code[i + 2];
    VALID_VRAM_INDEX(index);
    int val = cpu.VRAM[index];
    StackPush(&cpu.stack, val);
})

DEF_CMD(POP, PARAM_VRAM_IMMED, {
    int val = 0;
    int index = code[i + 2];
    VALID_VRAM_INDEX(index);
    StackPop(&cpu.stack, &val);
    cpu.VRAM[index] = val;
})

#define DEF_COLOUR(name, esc_string) case COLOUR_##name: printf(esc_string); break;

DEF_CMD(DRAW, NO_PARAMS, {

    for (int k = 0; k < VRAM_HEIGHT; k++) {
        for (int j = 0; j < VRAM_WIDTH; j++) {


            int pixel = cpu.VRAM[k * VRAM_WIDTH + j];
            switch(pixel) {
                case 0: printf("\033[40m \033[0m"); break;
                case 1: printf("\033[41m \033[0m"); break;
                case 2: printf("\033[42m \033[0m"); break;
                case 3: printf("\033[43m \033[0m"); break;
                case 4: printf("\033[44m \033[0m"); break;
                case 5: printf("\033[45m \033[0m"); break;
                case 6: printf("\033[46m \033[0m"); break;
                case 7: printf("\033[47m \033[0m"); break;

                default: fprintf(stderr, "Wrong colour code: %d", pixel); break;
            }
        }
        printf("\n");
    }

})

#undef DEF_COLOUR

//#endif
// CPU_COMMANDS_H
