#include "../include/memory.h"
#include "../include/common.h"

// متغیرهای سراسری مدیریت حافظه
static uint8_t* memory_map = NULL;
static uint64_t total_pages = 0;
static uint64_t used_pages = 0;
static uint64_t memory_start = 0x100000;  // از 1MB شروع می‌شود
static uint64_t memory_end = 0x20000000;  // مقدار پیش‌فرض 512MB

// ساختار Memory Map Entry
typedef struct {
    uint32_t base_low;
    uint32_t base_high;
    uint32_t length_low;
    uint32_t length_high;
    uint32_t type;
    uint32_t acpi;
} __attribute__((packed)) MemoryMapEntry;

// =============================================================================
// توابع داخلی
// =============================================================================

// تابع داخلی برای محاسبه اندازه bitmap
static uint64_t calculate_bitmap_size(uint64_t total_pages) {
    return (total_pages + 7) / 8;
}

// تابع ایمن برای خواندن از memory map - حالا memory_end رو به عنوان اشاره‌گر می‌گیره
static int safe_parse_memory_map(uint64_t* memory_end_ptr) {
    // ابتدا بررسی کن که آیا memory map وجود داره
    uint16_t* entry_count_ptr = (uint16_t*)MEMORY_MAP_LOCATION;

    // بررسی اولیه - اگر مقدار غیرمنطقی باشه، استفاده نکن
    if (*entry_count_ptr > 100) {  // بیشتر از 100 entry منطقی نیست
        DEBUGER(LIGHT_RED, "Invalid memory map entry count: %d", *entry_count_ptr);
        return -1;
    }

    uint16_t entry_count = *entry_count_ptr;

    //DEBUGER(LIGHT_CYAN, "Memory map entries: %d", entry_count);

    if (entry_count == 0 || entry_count > 50) {
        DEBUGER(LIGHT_RED, "No valid memory map entries, using fallback");
        return -1;
    }

    MemoryMapEntry* entries = (MemoryMapEntry*)(MEMORY_MAP_LOCATION + 2);
    uint64_t largest_block_size = 0;
    uint64_t largest_block_base = memory_start;
    int usable_entries_found = 0;

    for (int i = 0; i < entry_count; i++) {
        // محاسبه base و length با احتیاط
        uint64_t base = ((uint64_t)entries[i].base_high << 32) | entries[i].base_low;
        uint64_t length = ((uint64_t)entries[i].length_high << 32) | entries[i].length_low;

        // بررسی مقادیر منطقی
        if (base > 0x100000000ULL || length > 0x100000000ULL) {
            //DEBUGER(LIGHT_RED, "Skipping invalid memory entry %d: base=0x%x, length=0x%x", i, base, length);
            continue;
        }

        // نوع 1 = حافظه قابل استفاده
        if (entries[i].type == 1) {
            usable_entries_found++;
            //DEBUGER(LIGHT_CYAN, "Usable Entry %d: Base=0x%x, Length=0x%x MB",  i, base, length / (1024 * 1024));

            // فقط بلوک‌های بعد از 1MB رو در نظر بگیر
            if (base >= memory_start) {
                if (length > largest_block_size) {
                    largest_block_size = length;
                    largest_block_base = base;
                    DEBUGER(LIGHT_GREEN, "New largest block: 0x%x - 0x%x (%d MB)",
                        base, base + length, length / (1024 * 1024));
                }
            }
            else if (base < memory_start && (base + length) > memory_start) {
                // اگر بلوک از قبل از 1MB شروع شده اما بعد از 1MB ادامه داره
                uint64_t usable_base = memory_start;
                uint64_t usable_length = (base + length) - memory_start;

                if (usable_length > largest_block_size) {
                    largest_block_size = usable_length;
                    largest_block_base = usable_base;
                    //DEBUGER(LIGHT_GREEN, "New largest usable block: 0x%x - 0x%x (%d MB)",
                        //usable_base, usable_base + usable_length, usable_length / (1024 * 1024));
                }
            }
        }
        else {
            //DEBUGER(LIGHT_YELLOW, "Non-usable Entry %d: Type=%d, Base=0x%x", i, entries[i].type, base);
        }
    }

    //DEBUGER(LIGHT_CYAN, "Found %d usable memory entries", usable_entries_found);

    // اگر بلوک قابل استفاده پیدا شد
    if (largest_block_size > 0) {
        *memory_end_ptr = largest_block_base + largest_block_size;

        // محدود کردن به 4GB برای ایمنی
        if (*memory_end_ptr > MAX_MEMORY) {
            *memory_end_ptr = MAX_MEMORY;
            DEBUGER(LIGHT_YELLOW, "Limited to 4GB for safety");
        }

        //DEBUGER(LIGHT_GREEN, "Detected memory: 0x%x - 0x%x (%d MB)",
            //memory_start, *memory_end_ptr, (*memory_end_ptr - memory_start) / (1024 * 1024));
        return 0;
    }

    //DEBUGER(LIGHT_RED, "No usable memory blocks found");
    return -1;
}

// علامت گذاری محدوده حافظه به عنوان استفاده شده
static void mark_memory_range_used(uint64_t start, uint64_t end) {
    if (start < memory_start) start = memory_start;
    if (end > memory_end) end = memory_end;
    if (start >= end) return;

    uint64_t start_page = (start - memory_start) / PAGE_SIZE;
    uint64_t end_page = (end - memory_start) / PAGE_SIZE;

    for (uint64_t i = start_page; i < end_page && i < total_pages; i++) {
        uint64_t byte_index = i / 8;
        uint8_t bit_mask = 1 << (i % 8);

        if (memory_map[byte_index] & bit_mask) {
            memory_map[byte_index] &= ~bit_mask;
            used_pages++;
        }
    }
}

// =============================================================================
// توابع عمومی
// =============================================================================

void Init_Memory(void) {
    //DEBUGER(LIGHT_CYAN, "Initializing memory manager...");

    // سعی کن memory map رو بخونی و memory_end رو آپدیت کن
    if (safe_parse_memory_map(&memory_end) != 0) {
        //DEBUGER(LIGHT_YELLOW, "Using default memory: 512MB");
        memory_end = 0x20000000;  // 512MB پیش‌فرض
    }

    // محاسبه تعداد صفحات بر اساس memory_end جدید
    total_pages = (memory_end - memory_start) / PAGE_SIZE;

    //DEBUGER(LIGHT_CYAN, "Memory range: 0x%x - 0x%x", memory_start, memory_end);
    //DEBUGER(LIGHT_CYAN, "Total pages: %d", total_pages);

    // محاسبه اندازه bitmap
    uint64_t map_size_bytes = calculate_bitmap_size(total_pages);
    //DEBUGER(LIGHT_CYAN, "Bitmap size: %d bytes", map_size_bytes);

    // تخصیص bitmap - از حافظه بعد از 0x60000 استفاده می‌کنیم
    memory_map = (uint8_t*)0x60000;

    // مقداردهی اولیه bitmap - همه صفحات آزاد
    for (uint64_t i = 0; i < map_size_bytes; i++) {
        memory_map[i] = 0xFF;
    }

    // علامت گذاری حافظه‌های استفاده شده
    // 1. کرنل (0x10000 تا 0x20000)
    mark_memory_range_used(0x10000, 0x20000);

    // 2. صفحه‌بندی (0x1000 تا 0x5000)
    mark_memory_range_used(0x1000, 0x5000);

    // 3. memory map (0x5000 تا 0x6000)
    mark_memory_range_used(0x5000, 0x6000);

    // 4. bitmap ما (0x60000 به بعد)
    mark_memory_range_used(0x60000, 0x60000 + map_size_bytes);

    // 5. ویدئو مموری (0xB8000)
    mark_memory_range_used(0xB8000, 0xB8000 + 0x8000);

    DEBUGER(LIGHT_RED, "Memory initialized: %d MB, %d free pages",
        (memory_end - memory_start) / (1024 * 1024), total_pages - used_pages);
}

void* get_physical_page(void) {
    if (memory_map == NULL) {
        return NULL;
    }

    uint64_t map_size_bytes = calculate_bitmap_size(total_pages);

    for (uint64_t i = 0; i < total_pages; i++) {
        uint64_t byte_index = i / 8;
        uint8_t bit_mask = 1 << (i % 8);

        if (memory_map[byte_index] & bit_mask) {
            // صفحه آزاد پیدا شد
            memory_map[byte_index] &= ~bit_mask;
            used_pages++;

            uint64_t physical_addr = memory_start + (i * PAGE_SIZE);

            if (physical_addr >= memory_end) {
                // برگرداندن وضعیت
                memory_map[byte_index] |= bit_mask;
                used_pages--;
                return NULL;
            }

            // پاک کردن صفحه
            memset((void*)physical_addr, 0, PAGE_SIZE);

            return (void*)physical_addr;
        }
    }

    return NULL;
}

void free_physical_page(void* page) {
    if (memory_map == NULL) {
        return;
    }

    uint64_t physical_addr = (uint64_t)page;

    // بررسی محدوده معتبر
    if (physical_addr < memory_start || physical_addr >= memory_end) {
        return;
    }

    // بررسی alignment
    if (physical_addr % PAGE_SIZE != 0) {
        return;
    }

    uint64_t page_index = (physical_addr - memory_start) / PAGE_SIZE;

    if (page_index >= total_pages) {
        return;
    }

    uint64_t byte_index = page_index / 8;
    uint8_t bit_mask = 1 << (page_index % 8);

    // اگر قبلاً آزاد نبوده، آزادش کن
    if (!(memory_map[byte_index] & bit_mask)) {
        memory_map[byte_index] |= bit_mask;
        used_pages--;

        // پاک کردن صفحه
        memset(page, 0, PAGE_SIZE);
    }
}

uint64_t find_contiguous_free_pages(uint64_t count) {
    if (memory_map == NULL || count == 0 || count > total_pages) {
        return 0;
    }

    uint64_t consecutive_free = 0;
    uint64_t start_page = 0;

    for (uint64_t i = 0; i < total_pages; i++) {
        uint64_t byte_index = i / 8;
        uint8_t bit_mask = 1 << (i % 8);

        if (memory_map[byte_index] & bit_mask) {
            if (consecutive_free == 0) {
                start_page = i;
            }
            consecutive_free++;

            if (consecutive_free >= count) {
                return start_page;
            }
        }
        else {
            consecutive_free = 0;
        }
    }

    return 0;
}

uint64_t get_total_memory(void) {
    return total_pages * PAGE_SIZE;
}

uint64_t get_free_memory(void) {
    return (total_pages - used_pages) * PAGE_SIZE;
}

uint64_t get_total_physical_memory(void) {
    return total_pages * PAGE_SIZE;
}

void MemoryInfo(void) {
    uint64_t total_bytes = get_total_memory();
    uint64_t free_bytes = get_free_memory();
    uint64_t used_bytes = used_pages * PAGE_SIZE;

    uint64_t total_mb = total_bytes / (1024 * 1024);
    uint64_t free_mb = free_bytes / (1024 * 1024);
    uint64_t used_mb = used_bytes / (1024 * 1024);

    kprintf("\n=== Memory Information ===\n");
    kprintf("Total Memory:  %d MB\n", total_mb);
    kprintf("Used Memory:   %d MB (%d pages)\n", used_mb, used_pages);
    kprintf("Free Memory:   %d MB (%d pages)\n", free_mb, total_pages - used_pages);
    kprintf("Memory Range:  0x%x - 0x%x\n", memory_start, memory_end);
    kprintf("Page Size:     %d bytes\n", PAGE_SIZE);
    kprintf("==========================\n");
}

void memory_debug_dump(void) {
    kprintf("\n=== Memory Debug ===\n");
    kprintf("memory_map: 0x%x\n", memory_map);
    kprintf("memory_start: 0x%x\n", memory_start);
    kprintf("memory_end: 0x%x\n", memory_end);
    kprintf("total_pages: %d\n", total_pages);
    kprintf("used_pages: %d\n", used_pages);
}

void debug_memory_map(void) {
    uint16_t* entry_count_ptr = (uint16_t*)MEMORY_MAP_LOCATION;
    uint16_t entry_count = *entry_count_ptr;

    kprintf("\n=== Memory Map Debug ===\n");
    kprintf("Total entries: %d\n", entry_count);

    MemoryMapEntry* entries = (MemoryMapEntry*)(MEMORY_MAP_LOCATION + 2);

    for (int i = 0; i < entry_count && i < 10; i++) {
        uint64_t base = ((uint64_t)entries[i].base_high << 32) | entries[i].base_low;
        uint64_t length = ((uint64_t)entries[i].length_high << 32) | entries[i].length_low;

        kprintf("Entry %d: Base=0x%x, Length=0x%x, Type=%d\n",
            i, base, length, entries[i].type);
    }
    kprintf("=======================\n");
}
