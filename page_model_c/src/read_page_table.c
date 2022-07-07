#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/read_page_table.h"
#pragma warning (disable:4996)

table* newNode(unsigned int pg_num, bool bit, char* frame) {
	table* temp = (table*)malloc(sizeof(table));
	temp->page_number = pg_num;
	temp->presence_bit = bit;
	if (frame != NULL) {
		temp->frame_number = frame;
	}
	else
		temp->frame_number = NULL;
	temp->next = NULL;
	return temp;
}

table* insert(table* page, unsigned int pg_num, bool bit, char* frame) {
	if(page == NULL)
		return newNode(pg_num, bit, frame);
	while (page->next != NULL) {
		page = page->next;
	}
	page->next = newNode(pg_num, bit, frame);
	return page;
}

char* convert_int_binary(int frame_digit, int len) {
	int c, d, t = 0;
	char* p = (char*)malloc(len + 1);
	if (p == NULL)
		exit(EXIT_FAILURE);
	for (c = len - 1; c >= 0; c--) {
		d = frame_digit >> c;
		if (d & 1)
			*(p + t) = 1 + '0';
		else
			*(p + t) = 0 + '0';
		t++;
	}
	*(p + t) = '\0';
	return  p;
}

table record_analyze(char* string_val) {
	table pgTab;
	char* pch;
	pch = strtok(string_val, ";");
	int el = 0;
	pgTab.page_number = atoi(pch);
	pch = strtok(NULL, ";");
	if (atoi(pch))
		pgTab.presence_bit = true;
	else
		pgTab.presence_bit = false;
	pch = strtok(NULL, ";");
	if (pch[0] != '\n') {
		pch = convert_int_binary(atoi(pch), 2);
		pgTab.frame_number = (char*)malloc(2 * (sizeof(char)));
		pgTab.frame_number[0] = pch[0];
		pgTab.frame_number[1] = pch[1];
	}
	else
		pgTab.frame_number = NULL;
	return pgTab;
}

table* parse_pages_table(char* filename) {
	FILE* fp;
	char* ch_string = (char*)malloc(32 * (sizeof(char) + 1));
	table pgTab, * pageArr = NULL;
	bool checkBeg = true;
	fp = fopen(filename, "r");
	if (fp == NULL) {
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}
	while (fgets(ch_string, 32, fp) != NULL) {
		pgTab = record_analyze(ch_string);
		if (checkBeg) {
			pageArr = insert(pageArr, pgTab.page_number, pgTab.presence_bit, pgTab.frame_number);
			checkBeg = false;
		}
		else
			insert(pageArr, pgTab.page_number, pgTab.presence_bit, pgTab.frame_number);
	}
	free(ch_string);
	fclose(fp);
	return pageArr;
}

void print_page(table* page) {
	if (page != NULL) {
		if (page->frame_number != NULL)
			printf("%d %d %c%c\n", page->page_number, page->presence_bit, page->frame_number[0], page->frame_number[1]);
		else
			printf("%d %d\n", page->page_number, page->presence_bit);
		print_page(page->next);
	}
}

table get_table(int index, table* virt_array, int pos) {
	while (virt_array != NULL) {
		if (pos != index) {
			virt_array = virt_array->next;
			++pos;
		}
		else
			return virt_array[0];
	}
	table tab;
	tab.page_number = -1;
	return tab;
}