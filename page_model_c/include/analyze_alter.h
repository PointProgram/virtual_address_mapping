#pragma once
#include "read_page_table.h"

struct program_elems {
	int* hex;
	int size;
};

struct program_elems read_bin_file(char* filename);
void program_analyze(int* hex, int hex_size, table* pg_table);