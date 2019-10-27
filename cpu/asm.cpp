#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "textlib.h"

const int MAX_COMMAND_SIZE = 100;

line* read_commands(const char* filename, int* n_cmds);
int* code_to_buf(line* commands, int n_cmds, int* buf_size);
bool dump_code(int* buf, int n_lines, const char* filename);

int main() {
    const char* INPUT_FILE = "code.txt";
    const char* OUTPUT_FILE = "bin_data";

    line* commands = NULL;
    int n_cmds = 0;

    commands = read_commands(INPUT_FILE, &n_cmds);

    assert(commands);
    assert(n_cmds);
    int buf_size = 0;

    int* buf = code_to_buf(commands, n_cmds, &buf_size);

    assert(buf);

    dump_code(buf, buf_size, OUTPUT_FILE);

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

int* code_to_buf(line* commands, int n_cmds, int* buf_size) {
    int* buf = (int*)calloc(n_cmds,  2 * sizeof(int));

    assert(commands);
    assert(buf);

    char command_name[MAX_COMMAND_SIZE] = "";
    char cur_command[MAX_COMMAND_SIZE] = "";
    int value = 0;
    char reg[10] = "";

    #define DEF_CMD(name, num, args, code)          \
        else if (strcmp(command_name, #name) == 0)  \
            buf[2 * buf_cnt] = num;

    int cmd_cnt = 0, buf_cnt = 0;
    while (cmd_cnt < n_cmds) {

        strcpy(cur_command, commands[cmd_cnt].p_start);

        printf("%s\n", cur_command);

        char* pch = strtok(cur_command, " ");

        strcpy(command_name, pch);

        pch = strtok(NULL, " ");
        char* end = NULL;

        if (pch == NULL) {
            // cmd without arg
            buf[2 * buf_cnt + 1] = -1;
        }
        else {
            // cmd with arg
            int arg = strtol(pch, &end, 10);

            if (pch == end) {
                //cmd with reg
                strcat(command_name, " ");
                strcat(command_name, pch);

                buf[2 * buf_cnt + 1] = -1;
            }
            else {
                // cmd with num arg
                if (strcmp(command_name, "JMP") == 0) {
                    cmd_cnt = arg - 1;
                    continue;
                }
                buf[2 * buf_cnt + 1] = arg;
            }
        }


        if (false) ;

        #include "commands.h"

        else {
            fprintf(stderr, "Wrong command: %s", command_name);
        }

        cmd_cnt++; buf_cnt++;

    }

    for (int i = 0; i < 2 * buf_cnt; i += 2) {
        printf("%d %d\n", buf[i], buf[i+1]);
    }

    #undef DEF_CMD
    *buf_size = buf_cnt;

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