#ifndef CPU_CONSTANTS_H
#define CPU_CONSTANTS_H

const int RAM_SIZE = 20;
const int MAX_COMMAND_SIZE = 100;
const int CPU_STACK_SIZE = 10;
const int CPU_CALLS_STACK_SIZE = 10;
const int N_REGISTERS = 4;
const int MAX_LABELS_COUNT = 100;
const int MAX_LABEL_SIZE = 100;

enum Param_t {
    NO_PARAMS,
    PARAM_IMMED,
    PARAM_REG,
    PARAM_RAM_IMMED,
    PARAM_RAM_REG,
    LABEL,
};

#endif //CPU_CONSTANTS_H
