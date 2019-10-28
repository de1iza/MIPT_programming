#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "textlib.h"

const int MAX_COMMAND_SIZE = 100;
const int MAX_LABELS_COUNT = 100;
const int MAX_LABEL_SIZE = 100;

struct label {
    char name[MAX_LABEL_SIZE];
    int value;
};

label labels[MAX_LABELS_COUNT] = {};
int n_labels = 0;

line* read_commands(const char* filename, int* n_cmds);
int* code_to_buf(line* commands, int n_lines, int* buf_size);
bool dump_code(int* buf, int n_lines, const char* filename);
bool islabel(char* command);
void make_label_name(char* label);
void arrange_labels(line* commands, int n_cmds);
int get_label_value(char* label_name);

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

int* code_to_buf(line* commands, int n_lines, int* buf_size) {
    int* buf = (int*)calloc(n_lines,  2 * sizeof(int));

    assert(commands);
    assert(buf);
    assert(buf_size);


    char cur_command[MAX_COMMAND_SIZE] = "";

    int value = 0;
    char reg[10] = "";

    #define DEF_CMD(name, num, args, code)          \
        else if (strcmp(command_name, #name) == 0)  \
            buf[2 * buf_cnt] = num;


    int cmd_cnt = 0, buf_cnt = 0, line_cnt = 0;

    arrange_labels(commands, n_lines);

    for (int i = 0; i < n_labels; i++) {
        printf("!! %s %d\n", labels[i].name, labels[i].value);
    }

    printf("N_CMDS %d\n", n_lines);

    //#define DEF_CMD(name, num, args, code)      \
        if (strcmp(command_name, #name) == 0)   \
            code;                               \

    while (line_cnt < n_lines) {
        char command_name[MAX_COMMAND_SIZE] = "";

        strcpy(cur_command, commands[line_cnt].p_start);

        char* pch = strtok(cur_command, " ");

        strcpy(command_name, pch);

        pch = strtok(NULL, " ");
        char* end = NULL;

        if (pch == NULL) {
            // cmd without arg
            if (islabel(command_name)) {
                line_cnt++;
                continue;
            }
            buf[2 * buf_cnt + 1] = -1;
        }
        else {
            // cmd with arg
            int arg = strtol(pch, &end, 10);

            if (pch == end) {
                //cmd with string arg (NOW: only JUMP!)

                //strcat(command_name, " ");
                //strcat(command_name, pch);


                int jmp_val = get_label_value(pch);
                assert(jmp_val);

                buf[2 * buf_cnt + 1] = jmp_val;
            }
            else {
                // cmd with num arg

                buf[2 * buf_cnt + 1] = arg;
            }

        }



        if (false) ;

        #include "commands.h"

        else {
            fprintf(stderr, "Wrong command: %s", command_name);
        }

        line_cnt++; buf_cnt++;

    }

    assert(buf_cnt);
    assert(line_cnt);

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

bool islabel(char* command) {
    assert(strlen(command) > 1);
    return command[strlen(command) - 1] == ':';
}

void make_label_name(char* label) {
    assert(label);
    assert(strlen(label) > 1);

    label[strlen(label) - 1] = '\0';
}

void arrange_labels(line* commands, int n_lines) {
    assert(commands);
    assert(n_lines);

    char cur_command[MAX_COMMAND_SIZE] = "";
    int labels_cnt = 0;
    int cmd_cnt = 0;

    for (int i = 0; i < n_lines; i++) {
        strcpy(cur_command, commands[i].p_start);
        char* command_name = strtok(cur_command, " ");

        if (islabel(command_name)) {
            make_label_name(command_name);
            strcpy(labels[labels_cnt].name, command_name);
            labels[labels_cnt].value = cmd_cnt;
            labels_cnt++;
        }
        else {
            cmd_cnt++;
        }

    }
    n_labels = labels_cnt;

    #undef DEF_CMD
}

int get_label_value(char* label_name) {
    assert(label_name);

    int label_val = 0;
    for (int j = 0; j < n_labels; j++) {
        if (strcmp(labels[j].name, label_name) == 0) {
            return labels[j].value;
        }
    }
    return -1;
}