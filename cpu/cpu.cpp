#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../stack/stack.h"
#include "textlib.h"
#include "enum.h"

#define DEBUG 1

const int CPU_STACK_SIZE = 10;

int read_buf(const char* filename, int** buf);
bool execute(int* code, int n_cmds);

int main() {
    const char* CODE_FILE = "bin_data";

    int* buf = NULL;
    int n_cmds = read_buf(CODE_FILE, &buf);
    assert(buf);

    execute(buf, n_cmds);

    return 0;
}

int read_buf(const char* filename, int** buf) {
    FILE* fp = open_file(filename, "rb");

    size_t file_size = get_file_size(fp) ;

    *buf = (int*)calloc(file_size, 1);

    int n_cmds = file_size / sizeof(int) / 2;

    fread(*buf, sizeof(int), 2 * n_cmds, fp);

    for (int i = 0; i < 2 * n_cmds; i++)
        printf("%d ", (*buf)[i]);

    return n_cmds;
}

bool execute(int* code, int n_cmds) {
    static stack_t cpu = {};
    STACK_INIT(cpu, CPU_STACK_SIZE);

    #define DEF_CMD(name, num, args, code)   \
        case CMD_##name: code; break;

    for (int i = 0; i < 2 * n_cmds; i += 2) {
        printf("CODE[I] %d\n", code[i]);
        switch(code[i]) {
            #include "commands.h"

            default: fprintf(stderr, "Wrong command code: %d", code[i]); return false; break;
        }
        //STACK_ASSERT(&cpu);
    }

    #undef DEF_CMD

    StackDelete(&cpu);
    return true;
}