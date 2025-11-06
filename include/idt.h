#ifndef IDT_H
#define IDT_H

#include "type.h"

#define IDT_ENTRIES 256

// ساختار صحیح یک entry در IDT برای x86-64
typedef struct {
    uint16_t offset_low;    // آفست ۰-۱۵
    uint16_t selector;      // segment selector  
    uint8_t  ist;          // Interrupt Stack Table
    uint8_t  type_attr;    // نوع و ویژگی‌ها
    uint16_t offset_mid;    // آفست ۱۶-۳۱
    uint32_t offset_high;   // آفست ۳۲-۶۳
    uint32_t zero;          // صفر
} __attribute__((packed)) IDTEntry;

// ساختار اشاره‌گر IDT
typedef struct {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) IDTPointer;

// انواع interrupt
#define IDT_TYPE_INTERRUPT  0x8E  // 64-bit Interrupt Gate
#define IDT_TYPE_TRAP       0x8F  // 64-bit Trap Gate

// توابع
void Init_IDT(void);
void idt_set_entry(uint8_t index, uint64_t offset, uint16_t selector, uint8_t type_attr);

#endif