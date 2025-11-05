#include <stdarg.h>
#include "../include/vga.h"

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_ADDRESS 0xB8000

static volatile uint16_t* const VGA_MEMORY = (volatile uint16_t*)VGA_ADDRESS;
uint8_t color = 0x0F;

void rmline();

void scroll() {
	// کپی خطوط به بالا
	for (int y = 1; y < VGA_HEIGHT; y++) {
		for (int x = 0; x < VGA_WIDTH; x++) {
			VGA_MEMORY[(y - 1) * VGA_WIDTH + x] = VGA_MEMORY[y * VGA_WIDTH + x];
		}
	}

	// پاک کردن خط آخر به درستی
	for (int x = 0; x < VGA_WIDTH; x++) {
		VGA_MEMORY[(VGA_HEIGHT - 1) * VGA_WIDTH + x] = ' ' | (color << 8);
	}
}

void kprint(const char* str) {
	if (strcmp(str, "\033[2k") == 0) {
		rmline();
		return;
	}

	// بهینه‌سازی: حذف فراخوانی strlen اضافی
	while (*str) {
		VGA_Putc(*str++);
	}
}

void kprint_num(int num) {
	char buf[16];
	int i = 0;
	int is_negative = 0;

	// حالت ویژه برای کوچکترین عدد
	if (num == -2147483648) {
		kprint("-2147483648");
		return;
	}

	if (num < 0) {
		is_negative = 1;
		num = -num;
	}

	// حالت صفر
	if (num == 0) {
		buf[i++] = '0';
	}
	else {
		while (num > 0 && i < 15) {
			buf[i++] = '0' + (num % 10);
			num /= 10;
		}
	}

	if (is_negative) {
		buf[i++] = '-';
	}

	buf[i] = '\0';

	// معکوس کردن رشته
	for (int j = 0, k = i - 1; j < k; j++, k--) {
		char temp = buf[j];
		buf[j] = buf[k];
		buf[k] = temp;
	}

	kprint(buf);
}

void kprint_unsigned(unsigned int num) {
	char buf[16];
	int i = 0;

	if (num == 0) {
		buf[i++] = '0';
	}
	else {
		while (num > 0 && i < 15) {
			buf[i++] = '0' + (num % 10);
			num /= 10;
		}
	}

	buf[i] = '\0';

	for (int j = 0, k = i - 1; j < k; j++, k--) {
		char temp = buf[j];
		buf[j] = buf[k];
		buf[k] = temp;
	}

	kprint(buf);
}

void kprintf(const char* format, ...) {
	va_list args;
	va_start(args, format);

	for (const char* p = format; *p != '\0'; p++) {
		if (*p == '%') {
			p++;
			switch (*p) {
			case 'd': {
				int num = va_arg(args, int);
				kprint_num(num);
				break;
			}
			case 's': {
				const char* str = va_arg(args, const char*);
				kprint(str);
				break;
			}
			case 'c': {
				char c = va_arg(args, int);
				VGA_Putc(c);
				break;
			}
			case 'x': {
				int num = va_arg(args, int);
				char buf[16];
				int i = 0;
				int is_negative = 0;

				if (num < 0) {
					is_negative = 1;
					num = -num;
				}

				do {
					int digit = num % 16;
					buf[i++] = digit < 10 ? '0' + digit : 'A' + digit - 10;
					num /= 16;
				} while (num > 0 && i < 15);

				if (is_negative) {
					buf[i++] = '-';
				}

				buf[i] = '\0';

				for (int j = 0, k = i - 1; j < k; j++, k--) {
					char temp = buf[j];
					buf[j] = buf[k];
					buf[k] = temp;
				}

				kprint(buf);
				break;
			}
			case 'f': {
				double num = va_arg(args, double);
				int int_part = (int)num;
				double fractional_part = num - int_part;
				kprint_num(int_part);
				VGA_Putc('.');
				for (int i = 0; i < 6; i++) {
					fractional_part *= 10;
					int digit = (int)fractional_part;
					kprint_num(digit);
					fractional_part -= digit;
				}
				break;
			}
			case 'p': {
				void* ptr = va_arg(args, void*);
				kprint("0x");
				uint64_t num = (uint64_t)ptr;  // استفاده از uint64_t به جای int
				char buf[16];
				int i = 0;

				if (num == 0) {
					buf[i++] = '0';
				}
				else {
					while (num > 0 && i < 15) {
						int digit = num % 16;
						buf[i++] = digit < 10 ? '0' + digit : 'A' + digit - 10;
						num /= 16;
					}
				}

				buf[i] = '\0';

				for (int j = 0, k = i - 1; j < k; j++, k--) {
					char temp = buf[j];
					buf[j] = buf[k];
					buf[k] = temp;
				}

				kprint(buf);
				break;
			}
			case 'u': {
				unsigned int num = va_arg(args, unsigned int);
				kprint_unsigned(num); 
				break;
			}
			default:
				VGA_Putc('%');
				VGA_Putc(*p);
				break;
			}
		}
		else {
			VGA_Putc(*p);
		}
	}

	va_end(args);
}

void kprintcolor(const char* str, Color new) {
	Color temp = color;
	SetColor(new);
	kprint(str);
	SetColor(temp);
}

void rmline() {
	int x, y;
	VGA_GetCursor(&x, &y);
	VGA_SetCursor(0, y);
	for (int i = 0; i < VGA_WIDTH; i++) {
		VGA_Putc(' ');
	}
	VGA_SetCursor(0, y);
}

// ****************************************

void SetColor(Color new_color) {
	color = new_color;
}

void VGA_Clear()
{
	//volatile uint16_t* vga = (volatile uint16_t*)0xB8000;
	//vga[100] = 'O' | ((RED | (GREEN << 4)) << 8);
	//vga[101] = 'S' | ((RED | (GREEN << 4)) << 8);
	//vga[1 * 80 + 10] = 'K' | ((RED | (GREEN << 4)) << 8);		// سطر 1 ستون 10	
	//uint16_t ClearValue = ' ' | ((BLACK | (BLACK << 4)) << 8);
	uint16_t ClearValue = ' ' | (0x0F << 8);

	for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
	{
		VGA_MEMORY[i] = ClearValue;
	}
}

void VGA_Putc(char c) {
	uint16_t position = VGA_GetCursorPosition();

	// بررسی overflow - یک بار کافی است
	if (position >= VGA_WIDTH * VGA_HEIGHT) {
		scroll();
		position = VGA_GetCursorPosition();
	}

	if (c == '\n') {
		int row = position / VGA_WIDTH;
		if (row >= VGA_HEIGHT - 1) {
			scroll();
			row = VGA_HEIGHT - 2;
		}
		VGA_SetCursor(0, row + 1);
		return;
	}

	if (c == '\t') {
		// تب ۴ فضای خالی ایجاد می‌کند (نه ۵ تا)
		for (int i = 0; i < 4; i++) {
			VGA_Putc(' ');
		}
		return;
	}

	if (c == '\b') {
		if (position > 0) {
			position--;
			VGA_SetCursor(position % VGA_WIDTH, position / VGA_WIDTH);
			VGA_MEMORY[position] = (uint16_t)' ' | (color << 8);
		}
		return;
	}

	// نوشتن کاراکتر عادی
	VGA_MEMORY[position] = (uint16_t)c | (color << 8);

	// حرکت کرسر
	position++;
	VGA_SetCursor(position % VGA_WIDTH, position / VGA_WIDTH);

	// بررسی اسکرول فقط اگر نیاز باشد
	if (position >= VGA_WIDTH * VGA_HEIGHT) {
		scroll();
	}
}

// **************** Cursor ****************    

// تعریف پورت‌های VGA
#define VGA_CTRL_REG 0x3D4
#define VGA_DATA_REG 0x3D5

// رجیسترهای کنترل کرسر
#define CURSOR_START_REG 0x0A
#define CURSOR_END_REG   0x0B
#define CURSOR_HIGH_REG  0x0E
#define CURSOR_LOW_REG   0x0F

void VGA_GetCursor(int* x, int* y) {
	uint16_t position = 0;

	// خواندن کم بایت موقعیت
	outb(VGA_CTRL_REG, CURSOR_LOW_REG);
	position |= inb(VGA_DATA_REG);

	// خواندن زیاد بایت موقعیت
	outb(VGA_CTRL_REG, CURSOR_HIGH_REG);
	position |= ((uint16_t)inb(VGA_DATA_REG)) << 8;

	// تبدیل به مختصات x,y
	*x = position % VGA_WIDTH;
	*y = position / VGA_WIDTH;
}

void VGA_SetCursor(int x, int y) {
	// بررسی مرزها برای جلوگیری از خطا
	if (x < 0)
		x = 0;
	if (x >= VGA_WIDTH)
		x = VGA_WIDTH - 1;
	if (y < 0)
		y = 0;
	if (y >= VGA_HEIGHT)
		y = VGA_HEIGHT - 1;

	// محاسبه موقعیت خطی
	uint16_t position = (y * VGA_WIDTH) + x;

	// ارسال کم بایت موقعیت
	outb(VGA_CTRL_REG, CURSOR_LOW_REG);
	outb(VGA_DATA_REG, (uint8_t)(position & 0xFF));

	// ارسال زیاد بایت موقعیت
	outb(VGA_CTRL_REG, CURSOR_HIGH_REG);
	outb(VGA_DATA_REG, (uint8_t)((position >> 8) & 0xFF));
}

uint16_t VGA_GetCursorPosition() {
	uint16_t position = 0;
	outb(0x3D4, 0x0F);
	position |= inb(0x3D5);
	outb(0x3D4, 0x0E);
	position |= ((uint16_t)inb(0x3D5)) << 8;
	return position;
}

void VGA_SetCursorStyle(uint8_t start_scanline, uint8_t end_scanline) {
	// تنظیم شروع اسکن لاین برای کرسر
	outb(VGA_CTRL_REG, CURSOR_START_REG);
	outb(VGA_DATA_REG, start_scanline);

	// تنظیم پایان اسکن لاین برای کرسر  
	outb(VGA_CTRL_REG, CURSOR_END_REG);
	outb(VGA_DATA_REG, end_scanline);
}

void VGA_EnableCursor() {
	VGA_SetCursorStyle(14, 15);

	// اطمینان از فعال بودن کرسر
	outb(VGA_CTRL_REG, CURSOR_START_REG);
	uint8_t start = inb(VGA_DATA_REG);
	if (start & 0x20) {  // اگر بیت 5 تنظیم باشد (کرسر مخفی)
		outb(VGA_DATA_REG, start & ~0x20);  // پاک کردن بیت 5
	}
}

void VGA_DisableCursor() {
	// مخفی کردن کرسر با تنظیم بیت 5
	outb(VGA_CTRL_REG, CURSOR_START_REG);
	outb(VGA_DATA_REG, 0x20);  // بیت 5 = 1 کرسر را مخفی می‌کند
}

// ****************************************

void Init_VGA()
{
	VGA_Clear();
	SetColor(LIGHT_GREEN);
	VGA_EnableCursor();
	VGA_SetCursor(0, 0);
}
