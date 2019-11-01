#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "textlib.h"
#include "constants.h"
#include "enum.h"




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
void tolower_str(char* str);
bool isreg(char* arg);
int get_reg_code(char* reg);
bool is_RAM_index(char* arg, int* index);
bool is_RAM_register(char* arg, int* reg_code);

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



line* read_commands(const char* filename, int* n_cmds) { 
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
    int* buf = (int*)calloc(n_lines,  3 * sizeof(int));

    assert(commands);
    assert(buf);
    assert(buf_size);


    char cur_command[MAX_COMMAND_SIZE] = "";

    int value = 0;
    char reg[10] = "";

    #define DEF_CMD(name, arg_type, code)                                           \
        else if (strcmp(command_name, #name) == 0 && arg_type == param) {           \
            buf[3 * buf_cnt] = CMD_##name##arg_type;                                \
            buf[3 * buf_cnt + 1] = arg_type;                                        \
        }

    int cmd_cnt = 0, buf_cnt = 0, line_cnt = 0;

    arrange_labels(commands, n_lines);

    for (int i = 0; i < n_labels; i++) {
        printf("!! %s %d\n", labels[i].name, labels[i].value);
    }

    Param_t param = NO_PARAMS;

    while (line_cnt < n_lines) {
        if (strlen(commands[line_cnt].p_start) == 0) {
            line_cnt++;
            continue;
        }
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
            param = NO_PARAMS;
            buf[3 * buf_cnt + 2] = -1;
        }
        else {
            // cmd with arg
            int arg = strtol(pch, &end, 10);

            if (pch == end) {
                //cmd with string arg (jump, push/pop to reg or to ram)

                int jmp_val = 0;
                if ((jmp_val = get_label_value(pch)) > -1) {
                    param = LABEL;
                    buf[3 * buf_cnt + 2] = jmp_val;
                }
                else {
                    tolower_str(pch);
                    int index = -1, reg_code = -1;
                    if (isreg(pch)) {

                        param = PARAM_REG;
                        buf[3 * buf_cnt + 2] = get_reg_code(pch);
                    }
                    else if (is_RAM_index(pch, &index)) {
                        assert(index > -1);

                        param = PARAM_RAM_IMMED;
                        buf[3 * buf_cnt + 2] = index;
                    }
                    else if (is_RAM_register(pch, &reg_code)) {
                        assert(reg_code > -1);

                        param = PARAM_RAM_REG;
                        buf[3 * buf_cnt + 2] = reg_code;
                    }
                    else {
                        fprintf(stderr, "Unknown label: %s\n", pch);
                        abort();
                    }

                }

            }
            else {
                // cmd with num arg

                param = PARAM_IMMED;
                buf[3 * buf_cnt + 2] = arg;
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

    for (int i = 0; i < 3 * buf_cnt; i += 3) {
        printf("%d %d %d\n", buf[i], buf[i+1], buf[i+2]);
    }

    #undef DEF_CMD
    *buf_size = buf_cnt;

    return buf;
}

bool dump_code(int* buf, int n_lines, const char* filename) {
    FILE  *fp = open_file(filename, "wb");
    assert(fp);
    if (fp == NULL) return false;

    fwrite(buf, 3 * sizeof(int), n_lines, fp);


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
        if (strlen(commands[i].p_start) == 0) continue;
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

void tolower_str(char* str) {
    assert(str);

    for ( ; *str; ++str) *str = tolower(*str);;
}

bool isreg(char* arg) {
    assert(arg);

    return (strlen(arg) == 2 && arg[1] == 'x' && 0 <= (arg[0] - 'a') && (arg[0] - 'a') <= 3);
}

int get_reg_code(char* reg) {
    assert(reg);

    return reg[0] - 'a';
}

bool is_RAM_index(char* arg, int* index) {
    assert(arg);
    assert(index);

    if (arg[0] != '[' || arg[strlen(arg) - 1] != ']') return false;

    char* copy = NULL;
    copy = strdup(arg);

    copy[strlen(copy) - 1] = '\0';
    copy++;

    char* end = NULL;
    int num = strtol(copy, &end, 10);
    if (end == copy) return false;

    *index = num;

    //free(copy);
    return true;
}

bool is_RAM_register(char* arg, int* reg_code) {
    assert(arg);
    assert(reg_code);

    if (strlen(arg) != 4) return false;
    if (arg[0] != '[' || arg[3] != ']'|| arg[2] != 'x') return false;
    if (0 > arg[1] - 'a' || arg[1] - 'a' > 3) return false;


    *reg_code = arg[1] - 'a';
    return true;
}