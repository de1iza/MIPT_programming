#ifndef CPU_TEXTLIB_H
#define CPU_TEXTLIB_H

struct line{
    char* p_start;
    char* p_end;
};

/*!
    Opens file
    @param filename name of file to open
    @param mode r for read, w for write, etc
    @return fp opened filestream
*/
FILE* open_file(const char* filename, const char* mode);

int read_lines(FILE *fp, char** lines);

/*!
    Returns size of file in symbols
    @param fp filestream
    @return file_size size of file
*/
int get_file_size(FILE *fp);

/*!
    Allocates memory for lines array
    @param file_size size of text file
    @return lines char array
*/
char* init_lines(int file_size);

/*!
    Returns number of strings in array and replaces "\n" with "\0"
    @param lines array of chars
    @param c_count size of lines array
    @return n_lines number of lines
*/
int count_lines(char* lines, int c_count);

/*!
    Fills index array with pointers to strings from lines array
    @param lines array of chars
    @param n_lines number of lines in lines array
    @param index array to fill with pointers
*/
void arrange_ptrs(char* lines, int n_lines, line* index);

/*!
    Allocates memory for index array and fills it with pointers
    @param lines lines array
    @param n_lines number of lines in the text
    @return index array with pointers to lines
*/
line* build_index(char* lines, int n_lines);

#include "textlib.cpp"

#endif //CPU_TEXTLIB_H
