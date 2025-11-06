#ifndef IRQ_H
#define IRQ_H

#include "type.h"

#define IRQ_COUNT 16

// تعریف ساختار InterruptFrame اگر در فایل دیگری تعریف شده
#include "isr.h"

// توابع
void Init_IRQ(void);
void irq_handler(InterruptFrame* frame);
void irq_install_handler(int irq, void (*handler)(InterruptFrame*));
void irq_uninstall_handler(int irq);
void Init_PIC(void);

// تعریف تمام IRQ handlers در اسمبلی
extern void irq0(void);
extern void irq1(void);
extern void irq2(void);
extern void irq3(void);
extern void irq4(void);
extern void irq5(void);
extern void irq6(void);
extern void irq7(void);
extern void irq8(void);
extern void irq9(void);
extern void irq10(void);
extern void irq11(void);
extern void irq12(void);
extern void irq13(void);
extern void irq14(void);
extern void irq15(void);

#endif