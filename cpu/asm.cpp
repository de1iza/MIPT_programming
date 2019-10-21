#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "textlib.h"



line* read_commands(const char* filename, int* n_cmds);
int* process_code(line* commands, int n_cmds);
bool dump_code(int* buf, int n_lines, const char* filename);

int main() {
    const char* INPUT_FILE = "input.txt";
    const char* OUTPUT_FILE = "output";

    line* commands = NULL;
    int n_cmds = 0;

    commands = read_commands(INPUT_FILE, &n_cmds);

    assert(commands);
    assert(n_cmds);

    int* buf = process_code(commands, n_cmds);

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

line* build_index(char* lines, int n_lines) {
    line* index = (line*) calloc(n_lines + 1, sizeof(line));
    arrange_ptrs(lines, n_lines, index);
    return index;
}

void arrange_ptrs(char* lines, int n_lines, line* index) {
    int i = 1, line_cnt = 0;

    assert(lines);
    assert(n_lines);
    assert(index);

    index->p_start = lines;

    do {
        if (lines[i - 1] == '\0'){
            assert(i - 2);

            index->p_end = lines + i - 2;
            index++;
            index->p_start = lines + i;
            line_cnt++;
        }
        i++;
    } while(line_cnt < n_lines);

    assert(line_cnt);
}


int* process_code(line* commands, int n_cmds) {
    int* buf = (int*)calloc(n_cmds,  2 * sizeof(int));

    assert(buf);

    char command_name[100] = "";
    int value = 0;

    #define DEF_CMD(name, num)                      \
        else if (strcmp(command_name, #name) == 0)  \
            buf[2 * i] = num;

    for (int i = 0; i < n_cmds; i++) {

        sscanf(commands[i].p_start, "%s", command_name);

        if (false) ;

        #include "commands.h"

        else {
            fprintf(stderr, "Wrong command: %s", command_name);
        }

        if (sscanf(commands[i].p_start, "%*[^0-9]%d", &value)) {
            printf("VALUE %d\n", value);
            buf[2 * i + 1] = value;
        }

        printf("%s\n", command_name);
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

    fwrite(buf, 2 * sizeof(int), n_lines, fp);

    fclose(fp);

}