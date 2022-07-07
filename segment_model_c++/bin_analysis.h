#pragma once
#include "seg_tab_mod.h"

enum er_type { out_page, out_memory, out_reg, out_recognition, offset_crash};
enum cm_type { MOV, ADD, JMP, CMP };

class DataAnalysis {
public:
	std::vector <char> page_data;

private:
	std::string error_type(enum er_type value);
	bool register_transform(std::string str, char s_char, std::string& el);
	std::string virt_memory_analyse(std::string virt, std::vector<descriptor> d_table, bool& er_bool);
	std::string adress_check(int& key, std::string arraym, std::string descr, std::vector<descriptor> d_table, bool& er_bool, int gap);
	void bind_display_operand(std::string arraym, int& key, std::vector<descriptor> d_table);
	bool get_register(std::string get_slice, std::string& out_string, std::string& tmp_get, std::string& error, char first_byte);
public:
	std::string binary_read(std::string filename);
	void iterative_analysis(std::string hex_string, std::vector<descriptor> d_table);
};