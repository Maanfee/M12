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

