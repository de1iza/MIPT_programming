#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "../stack/stack.h"
#include "textlib.h"
#include "enum.h"
#include "constants.h"

#define DEBUG 1


struct cpu_t {
    elem_t registers[N_REGISTERS] = {0};
    int RAM[RAM_SIZE] = {0};
    int VRAM[VRAM_SIZE] = {0};
    stack_t stack = {};
    stack_t calls = {};
};

void cpu_init(cpu_t* cpu);
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

    int n_cmds = file_size / sizeof(int) / 3;

    fread(*buf, sizeof(int), 3 * n_cmds, fp);

    for (int i = 0; i < 3 * n_cmds; i++)
        printf("%d ", (*buf)[i]);

    return n_cmds;
}

bool execute(int* code, int n_cmds) {
    static cpu_t cpu = {};
    cpu_init(&cpu);


    #define DEF_CMD(name, arg_type, code) case CMD_##name##arg_type: code; break;

    bool end_flag = false;

    int i = 0;
    while (i < 3 * n_cmds) {
        printf("%d\n", code[i]);
        switch(code[i]) {
            #include "commands.h"

            default: fprintf(stderr, "Wrong command code: %d", code[i]); return false; break;
        }
        if (end_flag) break;
        i += 3;
    }

    #undef DEF_CMD

    for (int i = 0; i < 20; i++)
        printf("%d ", cpu.RAM[i]);

    return true;
}

void cpu_init(cpu_t* cpu) {
    STACK_INIT(cpu->stack, CPU_STACK_SIZE);
    STACK_INIT(cpu->calls, CPU_CALLS_STACK_SIZE);
}

