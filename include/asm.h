#ifndef ASM_H
#define ASM_H

#include "type.h"

static inline void outb(uint16_t port, uint8_t value) {
    asm volatile ("outb %b0, %w1" : : "a"(value), "Nd"(port));
}

static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    asm volatile ("inb %w1, %b0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void outw(uint16_t port, uint16_t value) {
    asm volatile ("outw %w0, %w1" : : "a"(value), "Nd"(port));
}

static inline uint16_t inw(uint16_t port) {
    uint16_t value;
    asm volatile ("inw %w1, %w0" : "=a"(value) : "Nd"(port));
    return value;
}

static inline void outl(uint16_t port, uint32_t value) {
    asm volatile ("outl %0, %w1" : : "a"(value), "Nd"(port));
}

static inline uint32_t inl(uint16_t port) {
    uint32_t value;
    asm volatile ("inl %w1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

#define hlt() asm volatile ("hlt")
#define sti() asm volatile ("sti")
#define cli() asm volatile ("cli")
#define nop() asm volatile ("nop")

//static inline __attribute__((always_inline)) void hlt() {
//    asm volatile ("hlt" : : : "memory");
//}

#endif