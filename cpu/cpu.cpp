#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <assert.h>
#include "../stack/stack.h"
#include "textlib.h"
#include "enum.h"
#include "constants.h"
#include "colours_enum.h"

#define DEBUG 1


struct cpu_t {
    elem_t registers[N_REGISTERS] = {0};
    elem_t RAM[RAM_SIZE] = {0};
    int VRAM[VRAM_SIZE] = {0};
    stack_t stack = {};
    stack_t calls = {};
};

void cpu_init(cpu_t* cpu);
int read_buf(const char* filename, int** buf);
bool execute(int* code, int n_cmds);
void draw_pixel(int pixel);
int double_to_int(double val);
double int_to_double(int val);

int main() {
    int* buf = NULL;
    int n_cmds = read_buf(BIN_FILE, &buf);
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

    return n_cmds;
}

bool execute(int* code, int n_cmds) {
    static cpu_t cpu = {};
    cpu_init(&cpu);


    #define DEF_CMD(name, arg_type, code) case CMD_##name##arg_type: code; break;

    bool end_flag = false;

    int i = 0;
    while (i < 3 * n_cmds) {
        switch(code[i]) {
            #include "commands.h"

            default: fprintf(stderr, "Wrong command code: %d", code[i]); return false; break;
        }
        if (end_flag) break;
        i += 3;
    }

    #undef DEF_CMD

    return true;
}

void cpu_init(cpu_t* cpu) {
    STACK_INIT(cpu->stack, CPU_STACK_SIZE);
    STACK_INIT(cpu->calls, CPU_CALLS_STACK_SIZE);
}

#define DEF_COLOUR(name, esc_string) case COLOUR_##name: printf(esc_string); break;

void draw_pixel(int pixel) {
    switch (pixel) {
        #include "colours.h"

        default:
            fprintf(stderr, "Wrong colour code: %d", pixel);
            break;
    }

}

#undef DEF_COLOUR

double int_to_double(int val) {
    return (double) val / 1000;
}

int double_to_int(double val) {
    return (int) (val * PRECISION);
}