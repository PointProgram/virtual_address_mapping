#include "../include/analyze_alter.h"
#include <stdio.h>

#pragma warning (disable:4996)

#define PAGE_TABLE_PATH "../data/page_table.csv" 
#define CODE_PATH "../data/program_data.dat"


int main() {
	//reading the csv file with table pages
	table* pgTable = parse_pages_table(PAGE_TABLE_PATH);
	struct program_elems hex_ar;
	//display content of csv file with table pages
	printf("Table page: \n");
	print_page(pgTable);
	//read content of the binary file with input data
	//test 1
	hex_ar = read_bin_file(CODE_PATH);
	//test 2
	//hex_ar = read_bin_file("program_data1.dat");
	//test 3
	//hex_ar = read_bin_file("program_data2.dat");
	//display result of input data analysis
	printf("Program analysis: \n");
	program_analyze(hex_ar.hex, hex_ar.size, pgTable);
}
