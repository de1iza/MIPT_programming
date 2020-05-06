#include "hack.h"
#include <QDir>
#include <QString>
#include <QDebug>

long long calculate_hash(const char* password, FILE* file, const long address) {
    long long offset = 0x0;
    long hash_byte = 0x0;
    long hash = 0x0;

    for (size_t i = 1; i < strlen(password); ++i) {
        offset = address + password[i] + 1;

        fseek(file, offset, SEEK_SET);
        fread(&hash_byte, 1, 1, file);
        hash += hash_byte;
    }

    hash += 0xaa;

    return hash;
}

void patch(const long byte, int count, const long address, FILE* file) {
    fseek(file, address, SEEK_SET);
    fwrite(&byte, 1, count, file);
}

int hack(const char* new_password, const char* filename) {
    //const char* filename = "C:\\Users\\Deliza\\Desktop\\MIPT_prog\\hack\\hack.com";
    const long table_start = 0x0000007d;
    const long hash_address = 0x0000017d;

    FILE* file = fopen(filename, "r+");

    if (!file) {
        printf("Can't open file");
        return 1;
    }

    //printf("%s x", new_password);

    long new_hash = calculate_hash(new_password, file, table_start);
    patch(new_hash, 2, hash_address, file);

    fclose(file);

    return 0;
}
