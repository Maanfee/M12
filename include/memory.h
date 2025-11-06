#ifndef MEMORY_H
#define MEMORY_H

#include "type.h"

#define PAGE_SIZE 4096
#define MEMORY_MAP_LOCATION 0x5000  // آدرس ثابت برای memory map

// توابع مدیریت حافظه فیزیکی
void Init_Memory(void);
void* get_physical_page(void);
void free_physical_page(void* page);
uint64_t get_total_memory(void);
uint64_t get_free_memory(void);
void MemoryInfo(void);

#endif