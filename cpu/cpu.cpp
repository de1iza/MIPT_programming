#include <stdio.h>
#include <cmath>
#include <stdlib.h>
#include <assert.h>
#include "../stack/stack.h"
#include "textlib.h"
#include "enum.h"
#include "constants.h"
#include "colours_enum.h"

#define DEBUG 1


struct cpu_t {
    elem_t* registers = NULL;
    elem_t* RAM = NULL;
    int* VRAM = NULL;
    stack_t stack = {};
    stack_t calls = {};
};

void cpu_init(cpu_t* cpu, size_t stack_size, size_t calls_stack_size, size_t n_registers, size_t ram_size, size_t vram_size);
int read_buf(const char* filename, int** buf);
bool execute(cpu_t* cpu, int* code, int n_cmds);
void draw_pixel(int pixel);
int double_to_int(double val, int precision);
double int_to_double(int val, int precision);

int main() {
    const char* BIN_FILE = "code.bin";

    const size_t CPU_STACK_SIZE = 10;
    const size_t CPU_CALLS_STACK_SIZE = 10;
    const size_t N_REGISTERS = 4;

    int* buf = NULL;
    int n_cmds = read_buf(BIN_FILE, &buf);
    assert(buf);

    static cpu_t cpu = {};
    cpu_init(&cpu, CPU_STACK_SIZE, CPU_CALLS_STACK_SIZE,
            N_REGISTERS, RAM_SIZE, VRAM_SIZE);

    execute(&cpu, buf, n_cmds);

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

bool execute(cpu_t* cpu, int* code, int n_cmds) {
    const int PRECISION = 1000;

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

void cpu_init(cpu_t* cpu, size_t stack_size, size_t calls_stack_size, size_t n_registers, size_t ram_size, size_t vram_size) {
    cpu->registers = (elem_t*) calloc(n_registers, sizeof(elem_t));
    cpu->RAM = (elem_t*) calloc(ram_size, sizeof(elem_t));
    cpu->VRAM = (int*) calloc(vram_size, sizeof(int));
    STACK_INIT(cpu->stack, stack_size);
    STACK_INIT(cpu->calls, calls_stack_size);
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

double int_to_double(int val, int precision) {
    return (double) val / precision;
}

int double_to_int(double val, int precision) {
    return round(val * precision);
}