#pragma once
#include <stdbool.h>

typedef struct page_table {
	int page_number;
	bool presence_bit;
	char* frame_number;
	struct page_table* next;
} table;


table* parse_pages_table(char* filename);
void print_page(table* page);
table get_table(int index, table* virt_array, int pos);
char* convert_int_binary(int frame_digit, int len);