#ifndef CPU_ENUM_H
#define CPU_ENUM_H

#define MAKE_CMD_NAME(name)            \
    char* arg = strtok(name, " ");     \
    char* cur_cmd = NULL;              \
    strcpy(cur_cmd, arg);              \
    arg = strtok(NULL, " ");           \
    if (arg != NULL) {                 \
        strcat(cur_cmd, "_");          \
        strcat(cur_cmd, arg);          \
    }                                  \
    strcpy(name, cur_cmd);


#define DEF_CMD(name, num, args, code) CMD_##name = num,

enum Commands {
    #include "commands.h"
};

#undef DEF_CMD

#endif //CPU_ENUM_H
