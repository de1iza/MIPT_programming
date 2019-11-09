#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "enum.h"
#include "textlib.h"
#include "constants.h"

void buf_to_code(const char* filename, int* buf, int n_cmds);
int read_binary(const char* filename, int** buf);
char get_reg_name(int reg_code);
double int_to_double(int val);

int main() {
    int* buf = NULL;

    int n_cmds = read_binary(BIN_FILE, &buf);

    buf_to_code(OUTPUT_FILE, buf, n_cmds);

    return 0;
}

int read_binary(const char* filename, int** buf) {
    assert(buf);

    FILE* fp = open_file(filename, "rb");

    size_t file_size = get_file_size(fp) ;

    *buf = (int*)calloc(file_size, 1);
    assert(*buf);

    int n_cmds = file_size / sizeof(int) / 3;

    fread(*buf, sizeof(int), 3 * n_cmds, fp);

    for (int i = 0; i < 3 * n_cmds; i++)
        printf("%d ", (*buf)[i]);

    fclose(fp);

    return n_cmds;
}

char get_reg_name(int reg_code) {
    return reg_code + 'A';
}

void buf_to_code(const char* filename, int* buf, int n_cmds) {
    assert(buf);

    FILE* fp = open_file(filename, "w");

    #define DEF_CMD(name, arg_type, code) {                                  \
        case CMD_##name##arg_type:                                           \
            fprintf(fp, "%s ", #name);                                       \
            if (arg_type == PARAM_REG)                                       \
                fprintf(fp, "%cX", get_reg_name(buf[3 * i + 2]));            \
            else if (arg_type == PARAM_RAM_IMMED)                            \
                fprintf(fp, "[%d]", buf[3 * i + 2]);                         \
            else if (arg_type == PARAM_RAM_REG)                              \
                fprintf(fp, "[%cX]", get_reg_name(buf[3 * i + 2]));          \
            else if (arg_type == PARAM_VRAM_IMMED)                           \
                fprintf(fp, "{%d}", buf[3 * i + 2]);                         \
            else if (arg_type == PARAM_VRAM_REG)                             \
                fprintf(fp, "{%cX}", get_reg_name(buf[3 * i + 2]));          \
            else if (arg_type)                                               \
                fprintf(fp, "%g", int_to_double(buf[3 * i + 2]));            \
            fprintf(fp, "\n");                                               \
            break;                                                           \
    }

    for (int i = 0; i < n_cmds; i++) {
        switch (buf[3 * i]) {
            #include "commands.h"
        }
    }

    #undef DEF_CMD
    fclose(fp);
}

double int_to_double(int val) {
    return (double) val / 1000;
}