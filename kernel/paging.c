#include "../include/paging.h"
#include "../include/memory.h"
#include "../include/common.h"

static uint64_t* kernel_pml4 = NULL;

void Init_Paging(void) {
    //DEBUGER(LIGHT_RED, "Initializing paging...");

    // ایجاد PML4 جدید برای kernel
    kernel_pml4 = (uint64_t*)get_physical_page();
    if (!kernel_pml4) {
        DEBUGER(LIGHT_RED, "Failed to allocate PML4!");
        return;
    }

    // پاک کردن PML4
    for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
        kernel_pml4[i] = 0;
    }

    //DEBUGER(LIGHT_CYAN, "PML4 allocated at physical: 0x%x", kernel_pml4);

    // Identity map اولیه 2MB برای kernel (برای booting)
    // این برای کدهای اولیه که هنوز در حالت فیزیکی کار می‌کنند ضروری است
    for (uint64_t i = 0; i < 0x200000; i += 0x200000) {
        map_page(i, i, PAGE_PRESENT | PAGE_WRITABLE | PAGE_GLOBAL);
    }

    // Higher half mapping برای kernel
    // آدرس‌های مجازی kernel به آدرس‌های فیزیکی پایین مپ می‌شوند
    for (uint64_t i = 0; i < 0x200000; i += 0x200000) {
        map_page(KERNEL_VIRTUAL_BASE + i, i, PAGE_PRESENT | PAGE_WRITABLE | PAGE_GLOBAL);
    }

    // فعال کردن صفحه‌بندی
    enable_paging();

    DEBUGER(LIGHT_RED, "Paging initialized successfully");
}

void map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags) {
    uint64_t pml4_index = PML4_INDEX(virtual_addr);
    uint64_t pdpt_index = PDPT_INDEX(virtual_addr);
    uint64_t pd_index = PD_INDEX(virtual_addr);

    // PML4
    if (!(kernel_pml4[pml4_index] & PAGE_PRESENT)) {
        uint64_t* new_pdpt = (uint64_t*)get_physical_page();
        if (!new_pdpt) {
            DEBUGER(LIGHT_RED, "Failed to allocate PDPT!");
            return;
        }

        // پاک کردن PDPT جدید
        for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
            new_pdpt[i] = 0;
        }

        kernel_pml4[pml4_index] = (uint64_t)new_pdpt | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
        //DEBUGER(LIGHT_CYAN, "Created PDPT at 0x%x for PML4[%d]", new_pdpt, pml4_index);
    }

    uint64_t* pdpt = (uint64_t*)(kernel_pml4[pml4_index] & ~0xFFF);

    // PDPT
    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) {
        uint64_t* new_pd = (uint64_t*)get_physical_page();
        if (!new_pd) {
            DEBUGER(LIGHT_RED, "Failed to allocate PD!");
            return;
        }

        // پاک کردن PD جدید
        for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
            new_pd[i] = 0;
        }

        pdpt[pdpt_index] = (uint64_t)new_pd | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
        //DEBUGER(LIGHT_CYAN, "Created PD at 0x%x for PDPT[%d]", new_pd, pdpt_index);
    }

    uint64_t* pd = (uint64_t*)(pdpt[pdpt_index] & ~0xFFF);

    // PD - استفاده از 2MB pages برای سادگی
    // برای mappingهای بزرگ بهتر است از huge pages استفاده کنیم
    pd[pd_index] = physical_addr | flags | PAGE_HUGE;

    //DEBUGER(LIGHT_CYAN, "Mapped VA 0x%x -> PA 0x%x (flags: 0x%x)", virtual_addr, physical_addr, flags);
}

void enable_paging(void) {
    // بارگذاری PML4 در CR3
    asm volatile("mov %0, %%cr3" : : "r"(kernel_pml4));

    // فعال کردن PAE (نیاز برای 64-bit)
    uint64_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 5);  // PAE bit
    asm volatile("mov %0, %%cr4" : : "r"(cr4));

    // تنظیم EFER MSR برای فعال کردن Long Mode
    uint64_t efer;
    asm volatile("rdmsr" : "=a"(efer) : "c"(0xC0000080));
    efer |= (1 << 8);  // LME bit
    asm volatile("wrmsr" : : "c"(0xC0000080), "a"(efer & 0xFFFFFFFF), "d"(efer >> 32));

    // فعال کردن صفحه‌بندی
    uint64_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= (1 << 31);  // PG bit
    asm volatile("mov %0, %%cr0" : : "r"(cr0));

    //DEBUGER(LIGHT_GREEN, "Paging enabled - CR3: 0x%x", kernel_pml4);
}

void* get_kernel_page_table(void) {
    return kernel_pml4;
}

// تابع کمکی برای تست
uint64_t get_physical_address(uint64_t virtual_addr) {
    // این یک پیاده‌سازی ساده است - در واقعیت باید page tables را walk کنیم
    if (virtual_addr >= KERNEL_VIRTUAL_BASE) {
        return virtual_addr - KERNEL_VIRTUAL_BASE;
    }
    return virtual_addr;  // برای identity map
}