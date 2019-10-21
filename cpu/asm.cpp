#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "textlib.h"

const int MAX_COMMAND_SIZE = 100;

line* read_commands(const char* filename, int* n_cmds);
int* code_to_buf(line* commands, int n_cmds);
bool dump_code(int* buf, int n_lines, const char* filename);

int main() {
    const char* INPUT_FILE = "code.txt";
    const char* OUTPUT_FILE = "bin_data";

    line* commands = NULL;
    int n_cmds = 0;

    commands = read_commands(INPUT_FILE, &n_cmds);

    assert(commands);
    assert(n_cmds);

    int* buf = code_to_buf(commands, n_cmds);

    assert(buf);

    dump_code(buf, n_cmds, OUTPUT_FILE);

    return 0;
}



line* read_commands(const char* filename, int* n_cmds) { //TODO bool - return error status?
    FILE *fp = open_file(filename, "r");

    assert(fp != NULL);


    char* lines = NULL;

    *n_cmds = read_lines(fp, &lines);

    assert(lines);
    assert(*n_cmds);

    fclose(fp);

    line* index = build_index(lines, *n_cmds);

    return index;
}

int* code_to_buf(line* commands, int n_cmds) {
    int* buf = (int*)calloc(n_cmds,  2 * sizeof(int));

    assert(commands);
    assert(buf);

    char command_name[MAX_COMMAND_SIZE] = "";
    int value = 0;

    #define DEF_CMD(name, num, args, code)          \
        else if (strcmp(command_name, #name) == 0)  \
            buf[2 * i] = num;

    for (int i = 0; i < n_cmds; i++) {

        sscanf(commands[i].p_start, "%s", command_name);

        if (false) ;

        #include "commands.h"

        else {
            fprintf(stderr, "Wrong command: %s", command_name);
        }

        printf("%s\n", command_name);

        if (sscanf(commands[i].p_start, "%*[^0-9]%d", &value) == 1) {
            buf[2 * i + 1] = value;
        }
        else {
            buf[2 * i + 1] = -1;
        }

    }

    for (int i = 0; i < 2* n_cmds; i++) {
        printf("%d ", buf[i]);
    }

    #undef DEF_CMD
    return buf;
}

bool dump_code(int* buf, int n_lines, const char* filename) {
    FILE  *fp = open_file(filename, "wb");
    assert(fp);
    if (fp == NULL) return false;

    fwrite(buf, 2 * sizeof(int), n_lines, fp);

    fclose(fp);
    return true;
}