#include "hex_decoder.h"
#include "descriptor_reading.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

//vonvertion of binary string to hexademical
std::string bin_to_hex(std::vector<unsigned char> buf, bool upper_case)
{
	std::ostringstream ret;
	for (unsigned int i = 0; i < buf.size(); ++i)
		ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase) << (int)buf[i];
	return ret.str();
}

//read binary and convert to the hexademical view
std::string CODE_GEN::binary_read(std::string filename) {
	std::ifstream if_str(filename, std::ios::in | std::ios::binary);
	if (!if_str) {
		std::cout << "Error: cannot open binary file" << std::endl;
		exit(0);
	}
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(if_str), {});
	std::string hex_str = bin_to_hex(buffer, true);
	return hex_str;
}

//get error message
std::string CODE_GEN::get_error_msg(enum error_tp value, std::string obj_addr, int num_error) {
	std::string error_message;
	switch (value) {
	case exceed_descriptor:
		error_message = "Err - descriptor segment is missing in the table by sequence number " + std::to_string(num_error);
		error_message += ": ";
		break;
	case exceed_page:
		error_message = "Err - page segment is missing in the table by sequence number " + std::to_string(num_error);
		error_message += ": ";
		break;
	case memory_missing:
		error_message = "Err - segment is not present in the main memory: ";
		break;
	case reg_message:
		error_message = "Err - synrax mistake in register code: ";
		break;
	case unknown:
		error_message = "Err - found unknown element: ";
		break;
	}
	return error_message;
}

//transform register code to string code
bool CODE_GEN::register_code(std::string hex_code, char cmp_ch, std::string& reg_element) {
	reg_element = "";
	if ((hex_code[0] == cmp_ch) && ((hex_code[1] >= '0' && hex_code[1] <= '9') || (hex_code[1] >= 'A' && hex_code[1] <= 'F'))) {
		reg_element += 'R';
		reg_element += std::to_string((hex_code[1] >= 'A') ? (hex_code[1] - 'A' + 10) : (hex_code[1] - '0'));
		return true;
	}
	reg_element = hex_code;
	return false;
}

//from virtual memory to physical memory
std::string CODE_GEN::physical_memory_decoder(std::string virt, std::vector<descriptor> descr, bool& err) {
	descriptor page_el;
	std::string bin_virtual = to_binary(stoll(virt, 0, 16), 32);
	int descriptor_num = stoi(bin_virtual.substr(0, DESCRIPTOR_SIZE), 0, 2), page_num;
	std::stringstream final_convertion;
	if (descr.size() > descriptor_num) {
		page_el = descr[descriptor_num];
		page_num = stoi(bin_virtual.substr(DESCRIPTOR_SIZE, 10), 0, 2);
		if (page_num < page_el.linked_page_elems.size()) {
			if (page_el.linked_page_elems[page_num].ch_bit) {
				final_convertion << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << stoi(bin_virtual.substr(20, 12), 0, 2) + page_num + descriptor_num;
				err = true;
				return final_convertion.str();
			}
			else 
				final_convertion << get_error_msg(memory_missing, "", 0) << "[0x" << virt << "]";
		}
		else
			final_convertion << get_error_msg(exceed_page, "", 0) << "[0x" << virt << "]";
	}
	else 
		final_convertion << get_error_msg(exceed_descriptor, "", descriptor_num) << "[0x" << virt << "]";
		
	err = false;
	return final_convertion.str();
}

std::string CODE_GEN::virtual_address_analyser(int& hex_index, std::string decoded_hex, std::string code_message, std::vector<descriptor> descr, bool &check, int tros) {
	std::string physickal_address = physical_memory_decoder(decoded_hex, descr, check), res_msg;
	hex_index += tros;
	if (check) {
		res_msg = code_message + physickal_address + "]";
	}
	else
		res_msg = physickal_address + "\n" + code_message + decoded_hex + "]";
	return res_msg;
}

//���������� ��������� ������������ ���� ������
std::string cmd_message(enum type_cmd command) {
	std::string message;
	if (command == MOV)
		message = "MOV ";
	else if (command == SUB)
		message = "SUB ";
	else if (command == JL)
		message = "JL ";
	else if(command == CMP)
		message = "CMP ";
	return message;
}

//hex strings as bytes
void divide_to_bytes(std::string hex_str, int beg, int str_len, int split_num) {
	for (int i = beg; i < beg + str_len; i++) {
		for (int j = 0; j < split_num; j++) {
			std::cout << hex_str[i + j];
		}
		std::cout << " ";
		i ++;
	} 
	std::cout << std::endl;
}

//modify state of the program
void mod_state(std::string &code, std::string concat_, int &hex_index, int gap) {
	code += concat_; hex_index += gap;
}

//get register code
bool  CODE_GEN::get_register(std::string tmp_modification, std::string &code_message, std::string &tmp_str, std::string &error, char first_byte) {
	if (!register_code(tmp_modification, first_byte, tmp_str)) {
		error = get_error_msg(reg_message, "", 0) + tmp_modification;
		return true;
	}
	return false;
}

//REG_REG, REG_ADR, SHIFT, ADDR, REG_LIT8, ADDR_REG_OUT
void CODE_GEN::reg_reg(std::string &code_message, int &hex_index, bool &if_reg1, bool &if_reg2, std::string &error1, std::string &error2, std::string decoded_hex, enum type_cmd TP) {
	std::string tmp_str;
	mod_state(code_message, cmd_message(TP), hex_index, 2);
	if_reg1 = get_register(decoded_hex.substr(hex_index, 2), code_message, tmp_str, error1, '0');
	mod_state(code_message, tmp_str + ", ", hex_index, 2);

	if_reg2 = get_register(decoded_hex.substr(hex_index, 2), code_message, tmp_str, error2, '0');
	mod_state(code_message, tmp_str, hex_index, 2);
}

void CODE_GEN::reg_adr(std::string &code_message, int&hex_index, std::string &error1, std::string decoded_hex, bool &if_reg1, std::vector<descriptor> descr, bool reverse_mod, enum type_cmd TP) {
	std::string tmp_str, tmp_modification, addr;
	bool error_switch = true;
	if (reverse_mod) {
		mod_state(code_message, cmd_message(TP) + "[0x", hex_index, 2);
		if_reg1 = get_register(decoded_hex.substr(hex_index, 2), code_message, tmp_str, error1, '1');
		mod_state(addr, tmp_str + "", hex_index, 2);

		tmp_modification = decoded_hex.substr(hex_index, 8);
		code_message = virtual_address_analyser(hex_index, tmp_modification, code_message, descr, error_switch, 8);
		mod_state(code_message, ", " + addr, hex_index, 0);
	}
	else {
		mod_state(code_message, cmd_message(TP), hex_index, 2);
		if_reg1 = get_register(decoded_hex.substr(hex_index, 2), code_message, tmp_str, error1, '0');
		tmp_modification = decoded_hex.substr(hex_index, 2);

		mod_state(code_message, tmp_str + ", [0x", hex_index, 2);
		tmp_modification = decoded_hex.substr(hex_index, 8);
		code_message = virtual_address_analyser(hex_index, tmp_modification, code_message, descr, error_switch, 8);
	}
}

void CODE_GEN::shift(std::string& code_message, int& hex_index, std::string decoded_hex, enum type_cmd TP) {
	std::string tmp_modification, tmp_str;
	mod_state(code_message, cmd_message(TP), hex_index, 2);
	tmp_modification = decoded_hex.substr(hex_index, 2);
	tmp_str = to_binary(stoi(tmp_modification, 0, 16), 8);
	if (tmp_str[0] == '1') {
		tmp_str = tmp_str.substr(1, 7);
		code_message += std::to_string(stoi(tmp_str, 0, 2));
	}
	else
		code_message += std::to_string(stoi(tmp_str, 0, 2));
	hex_index += 2;
}

void CODE_GEN::addr(std::string& code_message, int& hex_index, std::string decoded_hex, std::vector<descriptor> descr, enum type_cmd TP) {
	std::string tmp_modification;
	bool error_switch = true;
	mod_state(code_message, cmd_message(TP) + "[0x", hex_index, 2);
	tmp_modification = decoded_hex.substr(hex_index, 8);

	code_message = virtual_address_analyser(hex_index, tmp_modification, code_message, descr, error_switch, 8);
}

void CODE_GEN::reg_lit8(std::string& code_message, int& hex_index, std::string decoded_hex, enum type_cmd TP) {
	std::string tmp_modification, tmp_str;
	mod_state(code_message, cmd_message(TP), hex_index, 2);

	tmp_modification = decoded_hex.substr(hex_index, 2);
	if (register_code(tmp_modification, '0', tmp_str)) {
		mod_state(code_message, tmp_str + ", ", hex_index, 2);
		tmp_modification = decoded_hex.substr(hex_index, 2);
		mod_state(code_message, std::to_string(stoi(tmp_modification, 0, 16)), hex_index, 2);
	}
	else
		code_message += tmp_modification + get_error_msg(reg_message, "", 0);
}


//convert and form final program code
void CODE_GEN::code_converter(std::string decoded_hex, int hex_index, std::vector<descriptor> descr) {
	while (hex_index < decoded_hex.size()) {
		bool if_reg1 = false, if_reg2 = false;
		std::string code_message, error1, error2;
		int hex_input = stoi(decoded_hex.substr(hex_index, 2), 0, 16), beg_hex_index = hex_index;
		if (hex_input == 0x1A) {
			reg_reg(code_message, hex_index, if_reg1, if_reg2, error1, error2, decoded_hex, MOV);
		}
		else if (hex_input == 0x1B) {
			if (decoded_hex[hex_index + 2] == '1')
				reg_adr(code_message, hex_index, error1, decoded_hex, if_reg1, descr, true, MOV);
			else if (decoded_hex[hex_index + 2] == '0')
				reg_adr(code_message, hex_index, error1, decoded_hex, if_reg1, descr, false, MOV);
		}
		else if (hex_input == 0x0A) {
			reg_reg(code_message, hex_index, if_reg1, if_reg2, error1, error2, decoded_hex, SUB);
		}
		else if (hex_input == 0x0B) {
			reg_adr(code_message, hex_index, error1, decoded_hex, if_reg1, descr, false, SUB);
		}
		else if (hex_input == 0x92) {
			shift(code_message, hex_index, decoded_hex, JL);
		}
		else if (hex_input == 0x93) {
			addr(code_message, hex_index, decoded_hex, descr, JL);
		}
		else if (hex_input == 0x80) {
			reg_reg(code_message, hex_index, if_reg1, if_reg2, error1, error2, decoded_hex, CMP);
		}
		else if (hex_input == 0x1C) {
			reg_lit8(code_message, hex_index, decoded_hex, MOV);
		}
		else {
			code_message += get_error_msg(unknown, "", 0) + decoded_hex.substr(hex_index, 2);
			std::cout << "\n" << code_message << std::endl;
			hex_index += 2;
			continue;
		}
		std::cout << ">>|----------------------------" << std::endl;
		divide_to_bytes(decoded_hex, beg_hex_index, hex_index - beg_hex_index, 2);
		if (if_reg1)
			std::cout << error1 << std::endl;
		if (if_reg2)
			std::cout << error2 << std::endl;
		std::cout << code_message << std::endl;
		std::cout << "_______________________________" << std::endl;
	}
}



