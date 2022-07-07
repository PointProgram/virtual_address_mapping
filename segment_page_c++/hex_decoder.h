#pragma once
#include "descriptor_reading.h"

enum error_tp { exceed_descriptor, exceed_page, memory_missing, reg_message, unknown};
enum type_cmd { MOV, SUB, JL, CMP };
enum type { REG_REG, REG_ADR, SHIFT, ADDR, REG_LIT8, ADDR_REG_OUT };

class CODE_GEN {
public:
	std::vector <char> page_data;

private:
	std::string get_error_msg(enum error_tp value, std::string obj_addr, int num_error);
	bool register_code(std::string hex_code, char cmp_ch, std::string& reg_element);
	std::string physical_memory_decoder(std::string virt, std::vector<descriptor> d_table, bool& er_bool);
	std::string virtual_address_analyser(int& key, std::string decoded_hex, std::string descr, std::vector<descriptor> d_table, bool& er_bool, int gap);
	bool get_register(std::string get_slice, std::string& code_message, std::string& tmp_get, std::string& error, char first_byte);
	void reg_reg(std::string& code_message, int& key, bool& if_reg1, bool& if_reg2, std::string& error1, std::string& error2, std::string decoded_hex, enum type_cmd TP);
	void reg_adr(std::string& code_message, int& key, std::string& error1, std::string decoded_hex, bool& if_reg1, std::vector<descriptor> d_table, bool reverse_mod, enum type_cmd TP);
	void shift(std::string& code_message, int& key, std::string decoded_hex, enum type_cmd TP);
	void addr(std::string& code_message, int& key, std::string decoded_hex, std::vector<descriptor> d_table, enum type_cmd TP);
	void reg_lit8(std::string& code_message, int& key, std::string decoded_hex, enum type_cmd TP);
public:
	std::string binary_read(std::string filename);
	void code_converter(std::string, int key, std::vector<descriptor> d_table);
};