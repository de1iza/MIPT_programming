#ifndef CPU_ENUM_H
#define CPU_ENUM_H

#define DEF_CMD(name, arg_type, code) CMD_##name##arg_type,

enum Commands {
    #include "commands.h"
};

#undef DEF_CMD

#endif //CPU_ENUM_H
