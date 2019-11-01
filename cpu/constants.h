#ifndef CPU_CONSTANTS_H
#define CPU_CONSTANTS_H

const int RAM_SIZE = 20;

enum Param_t {
    NO_PARAMS,
    PARAM_IMMED,
    PARAM_REG,
    PARAM_RAM_IMMED,
    PARAM_RAM_REG,
    LABEL,
};

#endif //CPU_CONSTANTS_H
