#ifndef PAGING_H
#define PAGING_H

#include "type.h"

#define PAGE_SIZE 4096
#define PAGE_TABLE_ENTRIES 512

// flags برای page table entries
#define PAGE_PRESENT         (1 << 0)
#define PAGE_WRITABLE        (1 << 1)
#define PAGE_USER            (1 << 2)
#define PAGE_WRITETHROUGH    (1 << 3)
#define PAGE_CACHE_DISABLE   (1 << 4)
#define PAGE_ACCESSED        (1 << 5)
#define PAGE_DIRTY           (1 << 6)
#define PAGE_HUGE            (1 << 7)
#define PAGE_GLOBAL          (1 << 8)

// سطح‌های صفحه‌بندی x86-64
#define PML4_INDEX(vaddr)    (((vaddr) >> 39) & 0x1FF)
#define PDPT_INDEX(vaddr)    (((vaddr) >> 30) & 0x1FF) 
#define PD_INDEX(vaddr)      (((vaddr) >> 21) & 0x1FF)
#define PT_INDEX(vaddr)      (((vaddr) >> 12) & 0x1FF)

// آدرس‌های مهم
#define KERNEL_VIRTUAL_BASE  0xFFFFFFFF80000000
#define KERNEL_PHYSICAL_BASE 0x100000

typedef uint64_t PageTableEntry;

// توابع صفحه‌بندی
void Init_Paging(void);
void Complete_Boot_Paging(void);  // تابع جدید برای تکمیل صفحه‌بندی بوت
void map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags);
void unmap_page(uint64_t virtual_addr);
uint64_t get_physical_address(uint64_t virtual_addr);
void* get_kernel_page_table(void);
void enable_paging(void);

// توابع جدید برای یکپارچه‌سازی
void* get_current_pml4(void);
void copy_boot_mappings(uint64_t* new_pml4);
void setup_kernel_mappings(uint64_t* pml4);

#endif