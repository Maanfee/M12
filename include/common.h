#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "asm.h"
#include "type.h"

int strlen(const char* str);
void kprint(const char* str);
void kprintf(const char* format, ...);
void* memcpy(void* dest, const void* src, uint32_t n);
void* memset(void* dest, int c, uint32_t n);
void* memmove(void* dest, const void* src, uint32_t n);
int strcmp(const char* s1, const char* s2);
char* strcpy(char* dest, const char* src);
char* strncpy(char* dest, const char* src, uint32_t n);
char* strchr(const char* str, int c);
char *strtok(char* str, const char* delim);
char *strcat(char *dest, const char *src);
char* strrchr(const char* str, int c);
int memcmp(const void* s1, const void* s2, uint32_t n);

#endif