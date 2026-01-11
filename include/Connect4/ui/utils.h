#pragma once
#include <stdint.h>

int C4_Max(int a, int b);
int C4_Min(int a, int b);
float C4_FMax(float a, float b);
float C4_FMin(float a, float b);
size_t C4_ULLMax(size_t a, size_t b);
size_t C4_ULLMin(size_t a, size_t b);
char* C4_JoinStrings(const char* strings[], size_t count, const char* separator);