#include "bin_analysis.h"
#include "seg_tab_mod.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <iomanip>

//повернути текст помилки 
std::string DataAnalysis::error_type(enum er_type value) {
	std::string warning;
	switch (value) {
	case out_page:
		warning = "ERROR: failed to convert, descriptor number out of table - address ";
		break;
	case out_memory:
		warning = "ERROR: segment is not present in memory: ";
		break;
	case out_reg:
		warning = "ERROR: register code syntax mistake: ";
		break;
	case out_recognition:
		warning = "ERROR: element is not recognizable: ";
		break;
	case offset_crash:
		warning = "ERROR: offset of the virtual memory exceeds the actual size of the segment: ";
		break;
	}
	return warning;
}

//перетворити б≥нарний код у ш≥стнадц€тковий
std::string bin_to_hex(std::vector<unsigned char> buf, bool upper_case)
{
	std::ostringstream ret;
	for (unsigned int i = 0; i < buf.size(); ++i)
		ret << std::hex << std::setfill('0') << std::setw(2) << (upper_case ? std::uppercase : std::nouppercase) << (int)buf[i];
	return ret.str();
}

//прочитати б≥нарний файл, перетворити у ш≥стнадц€тковий та повернути €к р€док
std::string DataAnalysis::binary_read(std::string filename) {
	std::ifstream wf(filename, std::ios::in | std::ios::binary);
	if (!wf) {
		std::cout << "Cannot open binary file!" << std::endl;
		exit(0);
	}
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(wf), {});
	std::string hex_str = bin_to_hex(buffer, true);
	return hex_str;
}

//перетворити ш≥стнадц€тковий код рег≥стра у текстове представленн€ або ж повернути у раз≥ помилки
bool DataAnalysis::register_transform(std::string str, char s_char, std::string& el) {
	el = "";
	if ((str[0] == s_char) && ((str[1] >= '0' && str[1] <= '9') || (str[1] >= 'A' && str[1] <= 'F'))) {
		el += 'R';
		el += std::to_string((str[1] >= 'A') ? (str[1] - 'A' + 10) : (str[1] - '0'));
		return true;
	}
	el = str;
	return false;
}

//обрахунок та формуванн€ ф≥зичноњ адреси з в≥ртуальноњ
std::string DataAnalysis::virt_memory_analyse(std::string virt, std::vector<descriptor> d_table, bool& er_bool) {
	descriptor page_el;
	std::string virt_char = convert_dig_bit(stoll(virt, 0, 16), 32);
	std::string page_num_s = virt_char.substr(0, DESCR_SIZE), page_s;
	int page_num_d = stoi(page_num_s, 0, 2), page_n;
	std::stringstream out;
	
	if (d_table.size() > page_num_d) {
		page_el = d_table[page_num_d];
		page_s = virt_char.substr(DESCR_SIZE, 32);
		page_n = stoi(page_s, 0, 2);
		if (page_n < page_el.byte_size) {
			if (page_el.mem_presence != 0) {
				out << std::setfill('0') << std::setw(8) << std::hex << std::uppercase << stoi(d_table[page_num_d].basic_adress, 0, 16) + page_n;
				er_bool = true;
				return out.str();
			}
			else 
				out << error_type(out_memory) << "[0x" << virt << "]";
		}
		else
			out << error_type(offset_crash) << "[0x" << virt << "]";
	}
	else 
		out << error_type(out_page) << "[0x" << virt << "]";
		
	er_bool = false;
	return out.str();
}

//отриманн€ ф≥зичноњ адреси, зб≥льшенн€ кроку проходу р€дка та поверненн€ результату або ж помилки 
std::string DataAnalysis::adress_check(int& key, std::string arraym, std::string out_string, std::vector<descriptor> d_table, bool &error_swithc, int gap) {
	std::string phys = virt_memory_analyse(arraym, d_table, error_swithc);
	key += gap;
	return (error_swithc) ? out_string+phys+"]" : phys + "\n" + out_string + arraym + "]";
}

//поверненн€ текстових представлень код≥в команд
std::string command_type(enum cm_type command) {
	std::string cm;
	switch (command)
	{
	case MOV:
		cm = "MOV ";
		break;
	case ADD:
		cm = "MUL ";
		break;
	case JMP:
		cm = "JG ";
		break;
	case CMP:
		cm = "CMP ";
		break;
	}
	return cm;
}

//вивести ш≥стнадц€ткове представленн€ команди у вигл€д≥ байт≥в
void split_hex(std::string hex_str, int beg, int str_len, int split_num) {
	for (int i = beg; i < beg + str_len; i++) {
		for (int j = 0; j < split_num; j++) {
			std::cout << hex_str[i + j];
		}
		std::cout << " ";
		i ++;
	} 
	std::cout << std::endl;
}

//зб≥льшити крок команди та модиф≥кувати текстове представленн€ команди
void get_command_statement(std::string &str_command, std::string parse_concat, int &key, int gap) {
	str_command += parse_concat;
	key += gap;
}

//повернути true у раз≥ усп≥шного отриманн€ тексту рег≥стра або ж false у раз≥ помилки
bool  DataAnalysis::get_register(std::string get_slice, std::string &out_string, std::string &tmp_get, std::string &error, char first_byte) {
	if (!register_transform(get_slice, first_byte, tmp_get)) {
		error = error_type(out_reg) + get_slice;
		return true;
	}
	return false;
}

//прох≥д по р€дку ш≥стнадц€ткових символ≥в ≥з формуванн€м та виводом коду програми
void DataAnalysis::iterative_analysis(std::string hex_string, std::vector<descriptor> d_table) {
	int bg_str = 0, end_str = bg_str + 2;
	std::string get_slice;
	int i = 0;
	while (i < hex_string.size()) {
		bind_display_operand(hex_string, i, d_table);
	}
}

//реал≥зац≥€ формуванн€ команд процесора та њх вив≥дf
void DataAnalysis::bind_display_operand(std::string arraym, int &key, std::vector<descriptor> d_table) {
	bool error_swithc = true, reg_bool1 = false, reg_bool2 = false;
	int beg_key = key, len_get = 2;
	std::string get_slice = arraym.substr(key, len_get), tmp_get, out_string, error1, error2;
	int hex_input = stoi(get_slice, 0, 16);
	if (hex_input == 0x1A) {
		get_command_statement(out_string, command_type(MOV), key, 2);
		reg_bool1 = get_register(arraym.substr(key, len_get), out_string, tmp_get, error1, '0');
		get_command_statement(out_string, tmp_get + ", ", key, 2);

		reg_bool2 = get_register(arraym.substr(key, len_get), out_string, tmp_get, error2, '0');
		get_command_statement(out_string, tmp_get, key, 2);
	}
	else if (hex_input == 0x1B) {
		std::string addr;
		get_command_statement(out_string, command_type(MOV) + "[0x", key, 2);
		reg_bool1 = get_register(arraym.substr(key, len_get), out_string, tmp_get, error1, '1');
		get_command_statement(addr, tmp_get + "", key, 2);

		get_slice = arraym.substr(key, 8);
		out_string = adress_check(key, get_slice, out_string, d_table, error_swithc, 8);
		get_command_statement(out_string, ", " + addr , key, 0);
	}
	else if (hex_input == 0x01) {
		get_command_statement(out_string, command_type(ADD), key, 2);
		reg_bool1 = get_register(arraym.substr(key, len_get), out_string, tmp_get, error1, '0');
		get_command_statement(out_string, tmp_get + ", ", key, 2);

		reg_bool2 = get_register(arraym.substr(key, len_get), out_string, tmp_get, error2, '0');
		get_command_statement(out_string, tmp_get, key, 2);
	}
	else if (hex_input == 0x02) {
		get_command_statement(out_string, command_type(ADD), key, 2);
		reg_bool1 = get_register(arraym.substr(key, len_get), out_string, tmp_get, error1, '0');
		get_slice = arraym.substr(key, len_get);

		get_command_statement(out_string, tmp_get + ", [0x", key, 2);
		get_slice = arraym.substr(key, 8);
		out_string = adress_check(key, get_slice, out_string, d_table, error_swithc, 8);
	}
	else if (hex_input == 0x90) {
		get_command_statement(out_string, command_type(JMP), key, 2);
		get_slice = arraym.substr(key, len_get);
		tmp_get = convert_dig_bit(stoi(get_slice, 0, 16), 8);
		if (tmp_get[0] == '1') {
			tmp_get = tmp_get.substr(1, 7);
			out_string += std::to_string(stoi(tmp_get, 0, 2));
		}
		else
			out_string += std::to_string(stoi(tmp_get, 0, 2));
		key += 2;
	}
	else if (hex_input == 0x91) {

		get_command_statement(out_string, command_type(JMP) + "[0x", key, 2);
		get_slice = arraym.substr(key, 8);

		out_string = adress_check(key, get_slice, out_string, d_table, error_swithc, 8);
	}
	else if (hex_input == 0x80) {
		get_command_statement(out_string, command_type(CMP), key, 2);
		reg_bool1 = get_register(arraym.substr(key, len_get), out_string, tmp_get, error1, '0');
		get_command_statement(out_string, tmp_get + ", ", key, 2);

		reg_bool2 = get_register(arraym.substr(key, len_get), out_string, tmp_get, error2, '0');
		get_command_statement(out_string, tmp_get, key, 2);
	}
	else if (hex_input == 0x1C) {
		get_command_statement(out_string, command_type(MOV), key, 2);

		get_slice = arraym.substr(key, len_get);
		if (register_transform(get_slice, '0', tmp_get)) {
			get_command_statement(out_string, tmp_get + ", ", key, 2);
			get_slice = arraym.substr(key, len_get);
			get_command_statement(out_string, std::to_string(stoi(get_slice, 0, 16)), key, 2);
		}
		else if (register_transform(get_slice, '1', tmp_get)) {
			get_command_statement(out_string, tmp_get + ", ", key, 2);
			get_slice = arraym.substr(key, 4);
			get_command_statement(out_string, std::to_string(stoi(get_slice, 0, 16)), key, 4);
		}
		else 
			out_string += get_slice + error_type(out_reg);
	}
	else {
		out_string += error_type(out_recognition) + arraym.substr(key, len_get);
		std::cout << "\n" << out_string << std::endl;
		std::cout << "|-----" << std::endl;
		key += 2;
		return;
	}
	split_hex(arraym, beg_key, key - beg_key, 2);
	if (reg_bool1) 
		std::cout << error1 << std::endl;
	if(reg_bool2)
		std::cout << error2 << std::endl;
	std::cout << out_string << std::endl;
	std::cout << "|-----" << std::endl;
}



