#pragma once

#include <linux/types.h>

int display_init(void);
void display_write_char(char ch);
void display_gotoxy(size_t x, size_t y);
void display_deinit(void);