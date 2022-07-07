#include "../include/analyze_alter.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#pragma warning (disable:4996)

enum warn_type {out_page, not_bit, not_reg, not_known};

char* log_out(enum warn_type value) {
	if(value == out_page)
		return "ERROR: failed to convert, out of virtual pages: address ";
	else if (value == not_bit)
		return "ERROR: failed to convert, the page is currently not in the memory: adress ";
	else if (value == not_reg)
		return "ERROR: register code format is not compatible: ";
	else if (value == not_known)
		return "ERROR: could not recognize element ";
}

struct program_elems read_bin_file(char* filename) {
	const int BufferSize = 256;
	int i, * hex_array, size = 1;
	FILE* ptr;
	struct program_elems hx;
	unsigned char* buffer2 = (unsigned char*)malloc(BufferSize * (sizeof(unsigned char)));
	hex_array = (int*)malloc(sizeof(int));
	ptr = fopen(filename, "rb");
	const int fileSize = fread(buffer2, sizeof(unsigned char), BufferSize, ptr);
	for (i = 0; i < (fileSize / sizeof(unsigned char)); i++) {
		hex_array[i] = (int)buffer2[i];
		hex_array = realloc(hex_array, (++size) * sizeof(int));
	}
	hx.hex = hex_array;
	hx.size = size - 1;
	free(buffer2);
	fclose(ptr);
	return hx;
}

//function for forming a strings
char* add_char(char* str, char s) {
	size_t len;
	if (str == NULL)
		len = 0;
	else
		len = strlen(str);
	char* str2 = malloc(len + 1 + 1);
	if (str != NULL)
		strcpy(str2, str);
	str2[len] = s;
	str2[len + 1] = '\0';
	return str2;
}

bool reg_check(char* str, char s_char, char* el) {
	if ((str[0] == s_char) && ((str[1] >= '0' && str[1] <= '9') || (str[1] >= 'A' && str[1] <= 'F'))) {
		char s[3] = { 0 };
		s[0] = str[1];
		int num = strtol(s, NULL, 16);
		sprintf(s, "%d", num);
		el[0] = 'R';
		for (int i = 0; i < strlen(s); i++) {
			el[i + 1] = s[i];
			if (i == strlen(s) - 1) {
				el[i + 2] = '\0';
			}
		}
		return true;
	}
	return false;
}

char* concat(char* s1, const char* s2) {
	const size_t len1 = strlen(s1);
	const size_t len2 = strlen(s2);
	char* result = malloc(len1 + len2 + 1); // +1 for the null-terminator
	memcpy(result, s1, len1);
	memcpy(result + len1, s2, len2 + 1); // +1 to copy the null-terminator
	return result;
}

char* printf_compound(int val, char* attach, char* def_s) {
	sprintf(attach, "%.2X", val);
	def_s = concat(def_s, attach);
	return def_s;
}

char* MMU(char* virtual, table* pg_table, bool* if_error) {
	table page_el;
	unsigned int num = strtol(virtual, NULL, 16);
	char* virt_char = convert_int_binary(num, 32), * buf, * phys_buf;
	buf = (char*)malloc(32 * sizeof(char));
	phys_buf = (char*)malloc(32 * sizeof(char));
	buf[0] = '\0';
	for (int i = 0; i < 20; i++)
		buf = add_char(buf, virt_char[i]);
	page_el = get_table(strtol(buf, NULL, 2), pg_table, 0);
	phys_buf[0] = '\0';
	if (page_el.page_number != -1) {
		if (page_el.presence_bit != true) {
			free(buf);
			char* ret = (char*)malloc(128 * sizeof(char));
			ret[0] = '\0';
			sprintf(ret, "%s [0x%s], ", log_out(not_bit), virtual);
			*if_error = false;
			return ret;
		}
		phys_buf = convert_int_binary(strtol(page_el.frame_number, NULL, 2), 2);
	}
	else {
		free(buf);
		char* ret = (char*)malloc(128 * sizeof(char));
		ret[0] = '\0';
		sprintf(ret, "%s [0x%s]", log_out(not_bit), virtual);
		*if_error = false;
		return ret;
	}
	buf[0] = '\0';
	for (int i = 20; i < 32; i++) {
		buf = add_char(buf, virt_char[i]);
	}
	phys_buf = concat(phys_buf, buf);
	num = strtol(phys_buf, NULL, 2);
	buf[0] = '\0';
	sprintf(buf, "%.8X", num);
	*if_error = true;
	return buf;
}

char* adress_s(char* virt_addr, char* physick, bool if_true) {
	if (if_true)
		virt_addr = concat(virt_addr, physick);
	else 
		virt_addr = physick;
	return virt_addr;
}

char* adress_check(int* key, int* arraym, char* str, char* elem, char* descr, table* pg_table, bool* if_true) {
	for (int k = 0; k < 4; k++) {
		(*key)++;
		elem = printf_compound(arraym[(*key)], str, elem);
		if (k == 3) {
			char* phys = MMU(elem, pg_table, &(*if_true));
			return adress_s(descr, phys, *if_true);
		}
	}
}
void print_row(int bg, int end, int* arr_m) {
	for (int i = bg; i <= end; i++) {
		printf("%.2X ", arr_m[i]);
	}
}

void find_operand(int* arraym, int* key, table* pg_table) {
	char str[3], * descr, error[64];
	char* elem = (char*)malloc(16 * sizeof(char));
	descr = (char*)malloc(128 * sizeof(char));
	descr[0] = '\0';
	bool if_true = true;
	int beg_key = (*key);
	switch (arraym[(*key)])
	{
	case 0x1A:
		descr = concat(descr, "MOV ");
		(*key)++;
		sprintf(str, "%.2X", arraym[(*key)]);
		if (!reg_check(str, '0', elem)) {
			descr = concat("", printf_compound(arraym[(*key)], str, log_out(not_reg)));
			break;
		}
		descr = concat(descr, elem);
		descr = concat(descr, ", ");
		(*key)++;
		sprintf(str, "%.2X", arraym[(*key)]);
		if (!reg_check(str, '0', elem)) {
			descr = concat("", printf_compound(arraym[(*key)], str, log_out(not_reg)));
			break;
		}
		descr = concat(descr, elem);
		break;
	case 0x1B:
		descr = concat(descr, "MOV ");
		(*key)++;
		sprintf(str, "%.2X", arraym[(*key)]);
		if (!reg_check(str, '0', elem)) {
			descr = concat("", printf_compound(arraym[(*key)], str, log_out(not_reg)));
			break;
		}
		descr = concat(descr, elem);
		descr = concat(descr, ", [0x");
		elem[0] = '\0';
		descr = adress_check(&(*key), arraym, str, elem, descr, pg_table, &if_true);
		if (if_true)
			descr = concat(descr, "]");
		break;
	case 0x20:
		descr = concat(descr, "MUL ");
		(*key)++;
		sprintf(str, "%.2X", arraym[(*key)]);
		printf("%.2X ", arraym[(*key)]);
		if (!reg_check(str, '0', elem)) {
			descr = concat("", printf_compound(arraym[(*key)], str, log_out(not_reg)));
			break;
		}
		descr = concat(descr, elem);
		descr = concat(descr, ", ");
		(*key)++;
		sprintf(str, "%.2X", arraym[(*key)]);
		if (!reg_check(str, '0', elem)) {
			descr = concat("", printf_compound(arraym[(*key)], str, log_out(not_reg)));
			break;
		}
		descr = concat(descr, elem);
		break;
	case 0x22:
		descr = concat(descr, "MUL ");
		(*key)++;
		sprintf(str, "%.2X", arraym[(*key)]);
		if (!reg_check(str, '0', elem)) {
			descr = concat("", printf_compound(arraym[(*key)], str, log_out(not_reg)));
			break;
		}
		descr = concat(descr, elem);
		descr = concat(descr, ", [0x");
		elem[0] = '\0';
		descr = adress_check(&(*key), arraym, str, elem, descr, pg_table, &if_true);
		if (if_true)
			descr = concat(descr, "]");
		break;
	case 0x94:
		descr = concat(descr, "JG ");
		(*key)++;
		elem = convert_int_binary(arraym[(*key)], 8);
		if (elem[0] == '1') {
			for (int i = 0; i < strlen(elem) - 1; i++) {
				elem[i] = elem[i + 1];
				if (i == (strlen(elem) - 2))
					elem[i + 1] = '\0';
			}
			int num = strtol(elem, NULL, 2);
			sprintf(elem, "%d", -num);
			descr = concat(descr, elem);
		}
		else {
			sprintf(str, "%d", arraym[(*key)]);
			descr = concat(descr, str);
		}
		break;
	case 0x95:
		descr = concat(descr, "JG [0x");
		elem[0] = '\0';
		descr = adress_check(&(*key), arraym, str, elem, descr, pg_table, &if_true);
		if (if_true)
			descr = concat(descr, "]");
		break;
	case 0x80:
		descr = concat(descr, "CMP ");
		(*key)++;
		sprintf(str, "%.2X", arraym[(*key)]);
		if (!reg_check(str, '0', elem)) {
			descr = concat("", printf_compound(arraym[(*key)], str, log_out(not_reg)));
			break;
		}
		descr = concat(descr, elem);
		descr = concat(descr, ", ");
		(*key)++;
		sprintf(str, "%.2X", arraym[(*key)]);
		if (!reg_check(str, '0', elem)) {
			descr = concat("", printf_compound(arraym[(*key)], str, log_out(not_reg)));
			break;
		}
		descr = concat(descr, elem);
		break;
	case 0x1C:
		descr = concat(descr, "MOV ");
		(*key)++;
		sprintf(str, "%.2X", arraym[(*key)]);
		if (reg_check(str, '0', elem)) {
			descr = concat(descr, elem);
			descr = concat(descr, ", ");
			(*key)++;
			sprintf(str, "%.2X", arraym[(*key)]);
			int num = strtol(str, NULL, 16);
			elem[0] = '\0';
			sprintf(elem, "%d", num);
			descr = concat(descr, elem);
		}
		else if (reg_check(str, '2', elem)) {
			descr = concat(descr, elem);
			descr = concat(descr, ", ");
			elem[0] = '\0';
			for (int k = 0; k < 4; k++) {
				(*key)++;
				sprintf(str, "%.2X", arraym[(*key)]);
				elem = concat(elem, str);
				if (k == 3) {
					int num = strtol(elem, NULL, 16);
					elem[0] = '\0';
					sprintf(elem, "%d", num);

				}
			}
			descr = concat(descr, elem);
		}
		else {
			descr = concat("", printf_compound(arraym[(*key)], str, log_out(not_reg)));
			break;
		}
		break;
	default:
		sprintf(error, "%s %.2X", log_out(not_known), arraym[(*key)]);
		descr = concat(descr, error);
		break;
	}
	print_row(beg_key, (*key), arraym);
	printf(":\n\t%s\n", descr);
	free(elem);
	free(descr);
}

void program_analyze(int* hex, int hex_size, table* pg_table) {
	for (int i = 0; i < hex_size; i++) {
		find_operand(hex, &i, pg_table);
	}
}
