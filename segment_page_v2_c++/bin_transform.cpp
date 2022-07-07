#include <sstream>
#include <fstream>
#include <streambuf>
#include <iomanip>
#include <bitset>
#include "bin_transform.h"
#include "csv_read.h"

std::string command_form::put_error_description(std::string errot_type, int num, std::string address) {
	std::string error_string;
	if (errot_type == "descriptor_overflow") {
		std::cerr << "Error caused due to descriptor max size of array overflow" << std::endl;
		exit(1);
	}
	else if (errot_type == "page_overflow") {
		std::cerr << "Error caused due to page max size of array overflow" << std::endl;
		exit(1);
	}
	if (errot_type == "absent_descriptor")
		error_string = "Error caused due to absent descriptor segment with number " + std::to_string(num) + " while converting virtual address [0x" + address + "] ";
	else if (errot_type == "absent_page")
		error_string = "Error caused due to absent page segment with number " + std::to_string(num) + " while converting virtual address [0x" + address + "] ";
	else if (errot_type == "absent_memory")
		error_string = "Error caused due to absent page segment in the main memory with number " + std::to_string(num)  + " while converting virtual address [0x" + address + "] ";
	else if (errot_type == "register_malfunction")
		error_string = "Error caused due to syntax mistake in register hex code " + address;
	else if (errot_type == "unidentified_element")
		error_string = "Error caused due to unidentified element " + address + "\n";
	return error_string;
}

std::string command_form::get_address_message(std::string transformed_commands, std::string& error, std::vector<struct descriptor_segment> data_struct) {
	std::string addr = get_hex_info_slice(get_hex_ind(), 8), memory = addr;
	unsigned int descriptor, page, offset;
	std::string virtual_address = std::bitset<32>(stoll(memory, 0, 16)).to_string();
	inc_ind(8);

	descriptor = stoi(virtual_address.substr(0, descriptor_nm), 0, 2);
	page = stoi(virtual_address.substr(descriptor_nm, page_nm), 0, 2);
	offset = stoi(virtual_address.substr(descriptor_nm + page_nm, offset_vm), 0, 2);

	if (data_struct.size() > DESCRIPTOR_MAX_SIZE) {
		put_error_description("descriptor_overflow", 0, "");
	}
	else if (data_struct.size() < descriptor) {
		error = put_error_description("absent_descriptor", descriptor, memory);
	}
	else if (data_struct[descriptor].pg_table.size() > PAGE_MAX_SIZE) {
		put_error_description("page_overflow", 0, "");
	}
	else if (page > data_struct[descriptor].pg_table.size()) {
		error = put_error_description("absent_page", page, memory);
	}
	else if (!data_struct[descriptor].pg_table[page].page_flag) {
		error = put_error_description("absent_memory", page, memory);
	}
	else {
		std::stringstream physical_address;
		std::string addr_str = data_struct[descriptor].pg_table[page].page_frame + virtual_address.substr(descriptor_nm + page_nm, offset_vm);
		physical_address << std::setfill('0') << std::uppercase << std::setw(8) << std::hex << stoi(addr_str, 0, 2);
		return transformed_commands + physical_address.str() + "]";
	}
	return transformed_commands + addr + "]";
}

std::string command_form::hex_tranversion(std::string f_name) {
	std::ostringstream modified_hex;
	std::ifstream bin_data;
	bin_data.open(f_name, std::ios::in | std::ios::binary);
	if (!bin_data) {
		std::cerr << "Error due to opening a file " << f_name << std::endl;
		exit(1);
	}

	bin_data.seekg(0, std::ios::beg);
	std::vector<unsigned char> contents(std::istreambuf_iterator<char>(bin_data), {});

	for (size_t i = 0; i < contents.size(); i++)
		modified_hex << std::hex << std::uppercase << std::setfill('0') << std::setw(2) << (unsigned int)contents[i];
	bin_data.close();
	return modified_hex.str();
}

std::string hex_num_to_int(char hex_num) {
	int convert_hex;
	if (hex_num >= 'A' && hex_num <= 'F')
		convert_hex = hex_num - 'A' + 10;
	else if (hex_num >= '0' && hex_num <= '9')
		convert_hex = hex_num - '0';
	else
		return "";
	return std::to_string(convert_hex);
}

bool command_form::reg_formation(std::string &cmd, std::string &error, char bit) {
	inc_ind(2);
	std::string reg_num, byte = get_hex_info_slice(get_hex_ind(), 2);
	if (byte[0] == bit) {
		reg_num = hex_num_to_int(byte[1]);
		if (!reg_num.empty()) {
			cmd = 'R' + reg_num;
			return true;
		}
	}
	cmd = byte;
	error += put_error_description("register_malfunction", 0, byte);
	return false;
}

void command_form::Xreg(std::string &transformed_commands, std::string &error, bool plus_reg, std::string cmd_code) {
	std::string cmd;
	transformed_commands += cmd_code;
	if(reg_formation(cmd, error, '0'))
		transformed_commands += cmd + ", ";
	else 
		transformed_commands += "--, ";

	if (plus_reg) {
		if (reg_formation(cmd, error, '0'))
			transformed_commands += cmd + ", ";
		else
			transformed_commands += "--, ";
	}

	if(reg_formation(cmd, error, '0'))
		transformed_commands += cmd;
	else
		transformed_commands += "--";
	inc_ind(2);
}

void command_form::Xreg_toAddr(std::string &transformed_commands, std::string &error, std::vector<struct descriptor_segment> data_struct, bool addr_first, bool plus_reg, std::string cmd_code) {
	std::string cmd, reg_s;
	if (addr_first) {
		transformed_commands += cmd_code + "[0x";

		if (reg_formation(cmd, error, '1'))
			reg_s = cmd;
		else
			reg_s = "--";
		inc_ind(2);

		transformed_commands = get_address_message(transformed_commands, error, data_struct) + ", " + reg_s;
	}
	else {
		transformed_commands += cmd_code ;
		if (reg_formation(cmd, error, '0'))
			transformed_commands += cmd;
		else 
			transformed_commands += "--";
		if (plus_reg) {
			if (reg_formation(cmd, error, '0'))
				transformed_commands += ", " + cmd;
			else
				transformed_commands += ", --";
		}
		
		transformed_commands += ", [0x";
		inc_ind(2);
		transformed_commands = get_address_message(transformed_commands, error, data_struct);
	}
}

void command_form::reg_toLit16(std::string& transformed_commands, std::string& error, std::string cmd_code) {
	std::string cmd;
	transformed_commands += cmd_code;
	if (reg_formation(cmd, error, '1'))
		transformed_commands += cmd + ", ";
	else
		transformed_commands += "--, ";
	inc_ind(2);
	transformed_commands += std::to_string(stoi(get_hex_info_slice(get_hex_ind(), 4), 0, 16));
	inc_ind(4);
}

void command_form::toAaddr(std::string& transformed_commands, std::string& error, std::vector<struct descriptor_segment> data_struct, std::string cmd_code) {
	transformed_commands += cmd_code + "[0x";
	inc_ind(2);
	transformed_commands = get_address_message(transformed_commands, error, data_struct);
}

std::string split_output(std::string slice_str) {
	std::string code_format;
	for (size_t i = 0; i < slice_str.size(); i += 2) {
		for (int j = 0; j < 2; j++) 
			code_format += slice_str[i + j];
		code_format += " ";
	}
	return code_format;
}

void commands_output(std::string code_format, std::string word_format, std::string error, std::ofstream &program, bool to_file) {
	std::cout << ("___________________________") << std::endl;
	std::cout << "|" << std::setw(25) << std::left << split_output(code_format) << "|" << std::endl;
	if (!error.empty())
		std::cout << "|" << std::setw(25) << std::left << error << std::endl;
	std::cout << "|" << std::setw(25) << std::left << word_format << "|" << std::endl;
	std::cout << "|_________________________|" << std::endl;
	if (to_file) {
		program << split_output(code_format) << std::endl;
		if (!error.empty())
			program << error << std::endl;
		program << word_format << std::endl;
	}
}

void command_form::commands_conversion(std::vector<struct descriptor_segment> data_struct, bool file_output) {
	std::ofstream program_pr;
	program_pr.open("data_files/program/code.txt");
	while (hex_index < get_hex_info().size()) {
		std::string transformed_commands, error;
		int name_code = stoi(get_hex_info_slice(get_hex_ind(), 2), 0, 16), ind_f = hex_index;
		switch (name_code) {
			case 0x1A: 
				Xreg(transformed_commands, error, false, "MOV ");
				break;
			case 0x1B: 
				Xreg_toAddr(transformed_commands, error, data_struct, true, false, "MOV ");
				break;
			case 0x03: 
				Xreg(transformed_commands, error, true, "ADD ");
				break;
			case 0x04:
				Xreg_toAddr(transformed_commands, error, data_struct, false, true, "MOV ");
				break;
			case 0x20: 
				Xreg(transformed_commands, error, false, "MUL ");
				break;
			case 0x22: 
				Xreg_toAddr(transformed_commands, error, data_struct, false, false, "MUL ");
				break;
			case 0x95:
				toAaddr(transformed_commands, error, data_struct, "JG ");
				break;
			case 0x80:
				Xreg(transformed_commands, error, false, "CMP ");
				break;
			case 0x1C:
				reg_toLit16(transformed_commands, error, "MOV ");
				break;
			default:
				transformed_commands += put_error_description("unidentified_element", 0, get_hex_info_slice(get_hex_ind(), 2));
				inc_ind(2);
				break;
		}
		commands_output(get_hex_info_slice(ind_f, get_hex_ind() - ind_f), transformed_commands, error, program_pr, file_output);
	}
	program_pr.close();
}