#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "enum.h"
#include "textlib.h"
#include "constants.h"

void buf_to_code(const char* filename, int* buf, int n_cmds);
int read_binary(const char* filename, int** buf);

int main() {
    const char* CODE_FILE = "new_code.txt";
    const char* BIN_FILE = "bin_data";

    int* buf = NULL;

    int n_cmds = read_binary(BIN_FILE, &buf);

    buf_to_code(CODE_FILE, buf, n_cmds);

    return 0;
}

int read_binary(const char* filename, int** buf) {
    assert(buf);

    FILE* fp = open_file(filename, "rb");

    size_t file_size = get_file_size(fp) ;

    *buf = (int*)calloc(file_size, 1);
    assert(*buf);

    int n_cmds = file_size / sizeof(int) / 2;

    fread(*buf, sizeof(int), 2 * n_cmds, fp);

    for (int i = 0; i < 2 * n_cmds; i++)
        printf("%d ", (*buf)[i]);

    fclose(fp);

    return n_cmds;
}

void buf_to_code(const char* filename, int* buf, int n_cmds) {
    assert(buf);

    FILE* fp = open_file(filename, "w");

    #define DEF_CMD(name, num, args, code) {                \
        case CMD_##name:                                    \
            fprintf(fp, "%s ", #name);                      \
            if (args) fprintf(fp, "%d", buf[2 * i + 1]);    \
            fprintf(fp, "\n");                              \
            break;                                          \
    }

    for (int i = 0; i < n_cmds; i++) {
        switch (buf[2 * i]) {
            #include "commands.h"
        }
    }

    #undef DEF_CMD
    fclose(fp);
}