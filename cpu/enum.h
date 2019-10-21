#ifndef CPU_ENUM_H
#define CPU_ENUM_H

#define DEF_CMD(name, num, code) CMD_##name = num,

enum Commands{
    #include "commands.h"
};

#undef DEF_CMD

#endif //CPU_ENUM_H
