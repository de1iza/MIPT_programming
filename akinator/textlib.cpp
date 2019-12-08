FILE* open_file(const char* filename, const char* mode) {
    FILE* fp = fopen(filename, mode);

    if (fp == NULL) {
        fprintf(stderr, "Can't open file: %s", filename);
        exit(1);
    }

    return fp;
}

int read_lines(FILE *fp, char** lines) {
    assert(fp);

    int n_lines = 0;

    int file_size = get_file_size(fp) + 1;

    assert(file_size > 1);

    *lines = init_lines(file_size);
    assert(lines);

    fread(*lines, sizeof(char), file_size, fp);

    int chars_count = file_size / sizeof(char) - 1;

    n_lines = count_lines(*lines, chars_count);

    assert(n_lines);
    assert(*lines);

    return n_lines;
}

int get_file_size(FILE *fp) {
    assert(fp != NULL);

    fseek(fp, 0, SEEK_END);
    int file_size = ftell(fp);

    assert(file_size);

    fseek(fp, 0, SEEK_SET);

    return file_size;
}

char* init_lines(int file_size) {
    char *lines = (char*) calloc(file_size + 1, sizeof(char));

    assert(lines);

    return lines;
}

int count_lines(char* lines, int c_count) {
    assert(lines);

    int n_lines = 1;

    for (int i = 0; i < c_count; i++) {
        if (lines[i] == '\n') {
            lines[i] = '\0';
            n_lines++;
        }
    }

    assert(n_lines);

    return n_lines;
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

line* build_index(char* lines, int n_lines) {
    line* index = (line*) calloc(n_lines + 1, sizeof(line));
    arrange_ptrs(lines, n_lines, index);
    return index;
}


