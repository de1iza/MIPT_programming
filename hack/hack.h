#ifndef HACK_H
#define HACK_H

#endif // HACK_H

#include <stdio.h>
#include <string.h>

long long calculate_hash(const char* password, FILE* file, const long address);
void patch(const long byte, const int count, const long address, FILE* file);
int hack(const char* new_password, const char* filename);
