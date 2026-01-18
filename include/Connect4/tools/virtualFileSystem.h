#pragma once
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

bool C4_VFS_Init(const char* filePath);
void C4_VFS_Quit(void);
void* C4_VFS_ReadFile(const char* filename, size_t* outSize);
void C4_VFS_FreeFile(void* buffer);