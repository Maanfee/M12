#include "../include/memory.h"
#include "../include/common.h"

static uint8_t* memory_map = NULL;
static uint64_t total_pages = 0;
static uint64_t used_pages = 0;
static uint64_t memory_start = 0x100000;  // از 1MB شروع می‌شود
static uint64_t memory_end = 0x8000000;   // تا 128MB

void Init_Memory(void) {
    // محاسبه صحیح تعداد صفحات
    total_pages = (memory_end - memory_start) / PAGE_SIZE;

    //DEBUGER(LIGHT_CYAN, "Memory range: 0x%x - 0x%x, Pages: %d",  memory_start, memory_end, total_pages);

    // memory map در آدرس ثابت
    memory_map = (uint8_t*)MEMORY_MAP_LOCATION;

    // محاسبه اندازه bitmap
    uint64_t map_size_bytes = (total_pages + 7) / 8;
    //DEBUGER(LIGHT_CYAN, "Memory map size: %d bytes", map_size_bytes);

    // مقداردهی اولیه - همه صفحات آزاد
    for (uint64_t i = 0; i < map_size_bytes; i++) {
        memory_map[i] = 0xFF;  // همه بیت‌ها 1 = همه صفحات آزاد
    }

    // صفحاتی که قبلاً استفاده شده‌اند (kernel) را mark کنیم
    // kernel در 0x10000 تا 0x20000 است (64KB)
    uint64_t kernel_end = 0x20000;
    if (kernel_end > memory_start) {
        uint64_t kernel_pages = (kernel_end - memory_start) / PAGE_SIZE;
        //DEBUGER(LIGHT_CYAN, "Marking %d kernel pages as used", kernel_pages);

        for (uint64_t i = 0; i < kernel_pages; i++) {
            memory_map[i / 8] &= ~(1 << (i % 8));  // صفر کردن بیت = استفاده شده
            used_pages++;
        }
    }

    DEBUGER(LIGHT_RED, "Physical memory initialized");
    //print_memory_info();
}

void* get_physical_page(void) {
    // ابتدا مطمئن شویم memory_map مقداردهی شده
    if (memory_map == NULL) {
        DEBUGER(LIGHT_RED, "Memory not initialized!");
        return NULL;
    }

    for (uint64_t i = 0; i < total_pages; i++) {
        uint64_t byte_index = i / 8;
        uint8_t bit_mask = 1 << (i % 8);

        if (memory_map[byte_index] & bit_mask) {
            // صفحه آزاد پیدا شد
            memory_map[byte_index] &= ~bit_mask;  // mark as used
            used_pages++;

            uint64_t physical_addr = memory_start + (i * PAGE_SIZE);

            // بررسی معتبر بودن آدرس
            if (physical_addr >= memory_end) {
                DEBUGER(LIGHT_RED, "Invalid physical address: 0x%x", physical_addr);
                // برگرداندن وضعیت
                memory_map[byte_index] |= bit_mask;
                used_pages--;
                return NULL;
            }

            //DEBUGER(LIGHT_CYAN, "Allocated page %d -> 0x%x", i, physical_addr);
            return (void*)physical_addr;
        }
    }

    DEBUGER(LIGHT_RED, "Out of physical memory! Used: %d/%d pages", used_pages, total_pages);
    return NULL;
}

void free_physical_page(void* page) {
    if (memory_map == NULL) {
        DEBUGER(LIGHT_RED, "Memory not initialized!");
        return;
    }

    uint64_t physical_addr = (uint64_t)page;

    // بررسی محدوده معتبر
    if (physical_addr < memory_start || physical_addr >= memory_end) {
        DEBUGER(LIGHT_RED, "Invalid page address: 0x%x (range: 0x%x-0x%x)",
            physical_addr, memory_start, memory_end);
        return;
    }

    uint64_t page_index = (physical_addr - memory_start) / PAGE_SIZE;

    if (page_index >= total_pages) {
        DEBUGER(LIGHT_RED, "Page index out of range: %d (max: %d)", page_index, total_pages);
        return;
    }

    uint64_t byte_index = page_index / 8;
    uint8_t bit_mask = 1 << (page_index % 8);

    // اگر قبلاً آزاد نبوده، آزادش کن
    if (!(memory_map[byte_index] & bit_mask)) {
        memory_map[byte_index] |= bit_mask;  // mark as free
        used_pages--;
        DEBUGER(LIGHT_RED, "Freed page %d -> 0x%x", page_index, physical_addr);
    }
    else {
        DEBUGER(LIGHT_RED, "Page already free: 0x%x", physical_addr);
    }
}

uint64_t get_total_memory(void) {
    return total_pages * PAGE_SIZE;
}

uint64_t get_free_memory(void) {
    return (total_pages - used_pages) * PAGE_SIZE;
}

void MemoryInfo(void) {
    uint64_t total_bytes = get_total_memory();  // 133,169,152 bytes
    uint64_t free_bytes = get_free_memory();
    uint64_t used_bytes = used_pages * PAGE_SIZE;

    // محاسبه MB و KB به درستی
    uint64_t total_mb = total_bytes / (1024 * 1024);  // 127 MB
    uint64_t total_kb = total_bytes / 1024;           // 130,048 KB

    uint64_t free_mb = free_bytes / (1024 * 1024);
    uint64_t free_kb = free_bytes / 1024;

    uint64_t used_mb = used_bytes / (1024 * 1024);
    uint64_t used_kb = used_bytes / 1024;

    kprintf("Memory Info: \n");
    kprintf("  Total: %d MB (%d KB, %d bytes) \n", total_mb, total_kb, total_bytes);
    kprintf("  Used:  %d MB (%d KB, %d bytes, %d pages) \n", used_mb, used_kb, used_bytes, used_pages);
    kprintf("  Free:  %d MB (%d KB, %d bytes) \n", free_mb, free_kb, free_bytes);
}

void memory_debug_dump(void) {
    DEBUGER(LIGHT_MAGENTA, "=== MEMORY DEBUG DUMP ===");
    DEBUGER(LIGHT_MAGENTA, "memory_map: 0x%x", memory_map);
    DEBUGER(LIGHT_MAGENTA, "memory_start: 0x%x", memory_start);
    DEBUGER(LIGHT_MAGENTA, "memory_end: 0x%x", memory_end);
    DEBUGER(LIGHT_MAGENTA, "PAGE_SIZE: %d", PAGE_SIZE);
    DEBUGER(LIGHT_MAGENTA, "total_pages: %d", total_pages);
    DEBUGER(LIGHT_MAGENTA, "used_pages: %d", used_pages);

    // dump اولیه bitmap
    DEBUGER(LIGHT_MAGENTA, "First 16 bytes of memory_map:");
    for (int i = 0; i < 16; i++) {
        kprintf("  [0x%x]: 0x%x\n", &memory_map[i], memory_map[i]);
    }
}
