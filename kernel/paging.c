#include "../include/paging.h"
#include "../include/memory.h"
#include "../include/common.h"

static uint64_t* kernel_pml4 = NULL;

// تابع جدید: دریافت PML4 فعلی
void* get_current_pml4(void) {
    uint64_t cr3;
    asm volatile("mov %%cr3, %0" : "=r"(cr3));
    return (void*)(cr3 & ~0xFFF);  // پاک کردن flags
}

// تابع جدید: کپی mapping های ضروری از PML4 بوت
void copy_boot_mappings(uint64_t* new_pml4) {
    uint64_t* boot_pml4 = get_current_pml4();

    // کپی اولین entry از PML4 بوت (identity mapping پایه)
    if (boot_pml4[0] & PAGE_PRESENT) {
        new_pml4[0] = boot_pml4[0];
        //DEBUGER(LIGHT_CYAN, "Copied boot identity mapping");
    }

    // کپی mapping های دیگر اگر وجود دارند
    for (int i = 1; i < PAGE_TABLE_ENTRIES; i++) {
        if (boot_pml4[i] & PAGE_PRESENT) {
            new_pml4[i] = boot_pml4[i];
            //DEBUGER(LIGHT_CYAN, "Copied boot PML4[%d]", i);
        }
    }
}

// تابع جدید: تنظیم mapping های کرنل
void setup_kernel_mappings(uint64_t* pml4) {
    // Higher half mapping برای کرنل (512GB)
    uint64_t kernel_pml4_index = PML4_INDEX(KERNEL_VIRTUAL_BASE);

    if (!(pml4[kernel_pml4_index] & PAGE_PRESENT)) {
        uint64_t* kernel_pdpt = (uint64_t*)get_physical_page();
        if (!kernel_pdpt) {
            DEBUGER(LIGHT_RED, "Failed to allocate kernel PDPT!");
            return;
        }

        // پاک کردن PDPT جدید
        for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
            kernel_pdpt[i] = 0;
        }

        pml4[kernel_pml4_index] = (uint64_t)kernel_pdpt | PAGE_PRESENT | PAGE_WRITABLE;
        //DEBUGER(LIGHT_CYAN, "Created kernel PDPT at PML4[%d]", kernel_pml4_index);
    }

    uint64_t* pdpt = (uint64_t*)(pml4[kernel_pml4_index] & ~0xFFF);

    // Identity map کرنل فیزیکی به آدرس مجازی بالایی
    for (uint64_t i = 0; i < 0x200000; i += 0x200000) {
        uint64_t virtual_addr = KERNEL_VIRTUAL_BASE + i;
        uint64_t pdpt_index = PDPT_INDEX(virtual_addr);
        uint64_t pd_index = PD_INDEX(virtual_addr);

        if (!(pdpt[pdpt_index] & PAGE_PRESENT)) {
            uint64_t* pd = (uint64_t*)get_physical_page();
            if (!pd) {
                DEBUGER(LIGHT_RED, "Failed to allocate PD!");
                return;
            }

            for (int j = 0; j < PAGE_TABLE_ENTRIES; j++) {
                pd[j] = 0;
            }

            pdpt[pdpt_index] = (uint64_t)pd | PAGE_PRESENT | PAGE_WRITABLE;
        }

        uint64_t* pd = (uint64_t*)(pdpt[pdpt_index] & ~0xFFF);
        pd[pd_index] = i | PAGE_PRESENT | PAGE_WRITABLE | PAGE_HUGE | PAGE_GLOBAL;

        //DEBUGER(LIGHT_GREEN, "Mapped kernel: VA 0x%x -> PA 0x%x", virtual_addr, i);
    }
}

// تابع اصلی صفحه‌بندی - اصلاح شده
void Init_Paging(void) {
    //DEBUGER(LIGHT_CYAN, "Initializing kernel paging...");

    // دریافت PML4 فعلی (ساخته شده توسط بوت‌لودر)
    uint64_t* current_pml4 = get_current_pml4();
    //DEBUGER(LIGHT_CYAN, "Current PML4: 0x%x", current_pml4);

    // اگر PML4 بوت‌لودر در محدوده کرنل است، از آن استفاده کن
    if ((uint64_t)current_pml4 >= 0x100000 && (uint64_t)current_pml4 < 0x200000) {
        kernel_pml4 = current_pml4;
        //DEBUGER(LIGHT_GREEN, "Using bootloader PML4 at 0x%x", kernel_pml4);
    }
    else {
        // در غیر این صورت PML4 جدید بساز
        kernel_pml4 = (uint64_t*)get_physical_page();
        if (!kernel_pml4) {
            DEBUGER(LIGHT_RED, "Failed to allocate PML4!");
            return;
        }

        // پاک کردن PML4 جدید
        for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
            kernel_pml4[i] = 0;
        }

        // کپی mapping های بوت
        copy_boot_mappings(kernel_pml4);
        //DEBUGER(LIGHT_CYAN, "Created new PML4 at 0x%x", kernel_pml4);
    }

    // اضافه کردن mapping های کرنل
    setup_kernel_mappings(kernel_pml4);

    // اگر PML4 جدید ساخته شدیم، به آن سوئیچ کن
    if (kernel_pml4 != current_pml4) {
        asm volatile("mov %0, %%cr3" : : "r"(kernel_pml4));
        //DEBUGER(LIGHT_GREEN, "Switched to kernel PML4");
    }

    DEBUGER(LIGHT_RED, "Paging initialization completed successfully");
}

// بقیه توابع بدون تغییر...
void map_page(uint64_t virtual_addr, uint64_t physical_addr, uint64_t flags) {
    if (!kernel_pml4) return;

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

        for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
            new_pdpt[i] = 0;
        }

        kernel_pml4[pml4_index] = (uint64_t)new_pdpt | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    uint64_t* pdpt = (uint64_t*)(kernel_pml4[pml4_index] & ~0xFFF);

    // PDPT
    if (!(pdpt[pdpt_index] & PAGE_PRESENT)) {
        uint64_t* new_pd = (uint64_t*)get_physical_page();
        if (!new_pd) {
            DEBUGER(LIGHT_RED, "Failed to allocate PD!");
            return;
        }

        for (int i = 0; i < PAGE_TABLE_ENTRIES; i++) {
            new_pd[i] = 0;
        }

        pdpt[pdpt_index] = (uint64_t)new_pd | PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    }

    uint64_t* pd = (uint64_t*)(pdpt[pdpt_index] & ~0xFFF);

    // PD - استفاده از 2MB pages
    pd[pd_index] = physical_addr | flags | PAGE_HUGE;

    //DEBUGER(LIGHT_CYAN, "Mapped VA 0x%x -> PA 0x%x", virtual_addr, physical_addr);
}

// بقیه توابع بدون تغییر...
void enable_paging(void) {
    // اطمینان از فعال بودن PAE
    uint64_t cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= (1 << 5);  // PAE bit
    asm volatile("mov %0, %%cr4" : : "r"(cr4));

    // اطمینان از فعال بودن Long Mode
    uint64_t efer;
    asm volatile("rdmsr" : "=a"(efer) : "c"(0xC0000080));
    efer |= (1 << 8);  // LME bit
    asm volatile("wrmsr" : : "c"(0xC0000080), "a"(efer & 0xFFFFFFFF), "d"(efer >> 32));

    // اطمینان از فعال بودن صفحه‌بندی
    uint64_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= (1 << 31);  // PG bit
    asm volatile("mov %0, %%cr0" : : "r"(cr0));

    DEBUGER(LIGHT_GREEN, "Paging verified and enabled");
}

void* get_kernel_page_table(void) {
    return kernel_pml4;
}

uint64_t get_physical_address(uint64_t virtual_addr) {
    if (virtual_addr >= KERNEL_VIRTUAL_BASE) {
        return virtual_addr - KERNEL_VIRTUAL_BASE;
    }
    return virtual_addr;
}
