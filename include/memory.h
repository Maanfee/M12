#ifndef MEMORY_H
#define MEMORY_H

#include "type.h"

#define PAGE_SIZE 4096
#define MEMORY_MAP_LOCATION 0x5000
#define MAX_MEMORY 0x100000000ULL  // 4G      0x1000000000000ULL  // 256TB

// توابع مدیریت حافظه
void Init_Memory(void);
void* get_physical_page(void);
void free_physical_page(void* page);
uint64_t get_total_memory(void);
uint64_t get_free_memory(void);
uint64_t get_total_physical_memory(void);
uint64_t find_contiguous_free_pages(uint64_t count);

// توابع اطلاعات و دیباگ
void MemoryInfo(void);
void memory_debug_dump(void);
void debug_memory_map(void);

#endif
