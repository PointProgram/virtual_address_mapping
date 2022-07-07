#pragma once
#include "table_content.h"
#include <string>
#include <iostream>
#define descriptor_nm 13
#define page_nm 10
#define offset_vm 9

const int DESCRIPTOR_MAX_SIZE = 8192;
const int PAGE_MAX_SIZE = 1024;

typedef struct command {
	std::string cm_name;
	std::string part_one;
	std::string part_two;
	std::string part_three;
	std::vector<std::string> cmd_types;
} cmd_code;

class command_form {
private:
	unsigned int hex_index;
	std::string hex_info;
private:
	void inc_ind(int num) {
		hex_index += num;
	}
	int get_hex_ind() {
		return hex_index;
	}
	std::string get_hex_info_slice(int pos, int len) {
		if((pos + len - 1) < hex_info.size())
			return hex_info.substr(pos, len);
		else {
			std::cerr << "Error while reading input hex code data, some info is missed" << std::endl;
			exit(1);
		}
	}
	std::string get_hex_info() {
		return hex_info;
	}
	std::string put_error_description(std::string errot_type, int num, std::string address);
	std::string get_address_message(std::string transformed_commands, std::string& error, std::vector<struct descriptor_segment> data_struct);
	bool reg_formation(std::string& cmd, std::string& error, char bit);
	void Xreg(std::string& transformed_commands, std::string& error, bool plus_reg, std::string cmd_code);
	void Xreg_toAddr(std::string& transformed_commands, std::string& error, std::vector<struct descriptor_segment> data_struct, bool addr_first, bool plus_reg, std::string cmd_code);
	void toAaddr(std::string& transformed_commands, std::string& error, std::vector<struct descriptor_segment> data_struct, std::string cmd_code);
	void reg_toLit16(std::string& transformed_commands, std::string& error, std::string cmd_code);
public:
	command_form() {
		hex_index = 0;
	}
	void set_hex_info(std::string hex) {
		hex_info = hex;
	}
	std::string hex_tranversion(std::string f_name);
	void commands_conversion(std::vector<struct descriptor_segment> data_struct, bool file_output);
};