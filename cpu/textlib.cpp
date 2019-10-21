FILE* open_file(const char* filename, const char* mode) {
    FILE* fp = fopen(filename, mode);

    if (fp == NULL) {
        fprintf(stderr, "Can't open file");
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

    int chars_count = file_size / sizeof(char) - 1; // TODO!!!!

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

    int n_lines = 0;

    for (int i = 0; i < c_count; i++) {
        if (lines[i] == '\n') {
            lines[i] = '\0';
            n_lines++;
        }
    }

    assert(n_lines);

    return n_lines;
}

