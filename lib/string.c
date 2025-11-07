#include "../include/common.h"

int32_t strlen(const char* str) {
	const char* ptr = str;
	while (*ptr != '\0') {
		ptr++;
	}
	return (int32_t)(ptr - str);
}

// number = *ptr || &number = ptr

int32_t strcmp(const char* s1, const char* s2) {
	while (*s1 && (*s1 == *s2)) {
		s1++;
		s2++;
	}
	return *(const uint8_t*)s1 - *(const uint8_t*)s2;
}

void* memset(void* dest, int c, uint32_t n) {
	uint8_t* ptr = (uint8_t*)dest;
	for (uint32_t i = 0; i < n; i++) {
		ptr[i] = (uint8_t)c;
	}
	return dest;
}
