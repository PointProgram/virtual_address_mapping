#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <bitset>
#include "table_content.h"


std::vector<struct fixed_page> struct_transfer::page_read_write(std::string page_path) {
	std::fstream page_file;
	std::vector<struct fixed_page> page_vector;
	std::string string_line, token;
	std::string::size_type bg_pos, end_pos;
	page_file.open(page_path, std::ios::in);

	if (!page_file.is_open()) {
		std::cerr << "Error due to opening a file " << page_path << std::endl;
		exit(1);
	}

	int read_word_cnt = 0;
	while (getline(page_file, string_line)) {
		bg_pos = 0;
		struct fixed_page page_el;
		while (read_word_cnt < 3) {
			end_pos = string_line.find(';', bg_pos);
			token = string_line.substr(bg_pos, end_pos);
			if (end_pos != std::string::npos) {
				if (read_word_cnt == 0) {
					page_el.page_num = stoi(token);
				}
				else if (read_word_cnt == 1) {
					page_el.page_flag = stoi(token);
				}
			}
			else
			{
				if (read_word_cnt == 2) {
					if (page_el.page_flag == 1) {
						page_el.page_frame = std::bitset<2>(token).to_string();
					}
				}
				else {
					if (read_word_cnt == 1 && stoi(token) == 0) {
						page_el.page_flag = stoi(token);
					}
					else {
						std::cerr << "It seems like the file " << page_path << " has an empty field, check and correct it." << std::endl;
						exit(1);
					}
				}
			}
			bg_pos = end_pos + 1;
			read_word_cnt++;
		}
		read_word_cnt = 0;
		page_vector.push_back(page_el);
	}
	
	page_file.close();
	return page_vector;
}


void struct_transfer::descriptor_read_write(std::string file_name) {
	std::fstream descritor_file;
	std::string string_line, token;
	descritor_file.open(file_name, std::ios::in);
	
	struct descriptor_segment descr_struct;
	if (!descritor_file.is_open()) {
		std::cerr << "Error due to opening a file " << file_name << std::endl;
		exit(1);
	}
	int read_word_cnt = 0;
	std::string::size_type bg_pos, end_pos;
	while (getline(descritor_file, string_line)) {
		bg_pos = 0;
		while (read_word_cnt < 2) {
			end_pos = string_line.find(';', bg_pos);
			token = string_line.substr(bg_pos, end_pos);
			if (end_pos != std::string::npos)
			{	
				if (read_word_cnt == 0)
					descr_struct.path = token;
			}
			else
			{
				if (read_word_cnt == 1)
					descr_struct.page_size = stoi(token);
				else {
					std::cerr << "It seems like the file " << file_name << " has an empty field, check and correct it." << std::endl;
					exit(1);
				}
			}
			bg_pos = end_pos + 1;
			read_word_cnt++;
		}
		descr_struct.pg_table = page_read_write(descr_struct.path);
		if (descr_struct.pg_table.size() != descr_struct.page_size) {
			std::cerr << "It seems size of the file " << file_name << " doesen't align with actual page of file." << std::endl;
			exit(1);
		}

		read_word_cnt = 0; 
		segment_struct.push_back(descr_struct);
	}
	
	descritor_file.close();
}

void struct_transfer::output_struct_tables() {
	printf("___________________________________________________________\n");
	printf("|%5s|%30s|%20s|\n", "num", "address", "page_num");
	for (size_t i = 0; i < segment_struct.size(); i++) {
		printf("|_____|______________________________|____________________|\n");
		std::cout << "|" <<std::setw(5) << i << "|" << std::setw(30) << segment_struct[i].path << "|" << std::setw(20) << segment_struct[i].page_size << "|" << std::endl;
		printf("|_____|______________________________|____________________|\n");
		std::cout <<"|"<< std::setw(6) << "|" << std::setw(15) << "Pg num.|" << std::setw(16) << "Pres. bit|" << std::setw(20) << "Frame num." << "|" <<std::endl;
		for (size_t j = 0; j < segment_struct[i].pg_table.size(); j++) {
			std::cout << "|" << std::setw(6) << "|" << std::setw(14) << segment_struct[i].pg_table[j].page_num << "|" << std::setw(15) << segment_struct[i].pg_table[j].page_flag << "|" <<std::setw(20) << segment_struct[i].pg_table[j].page_frame << "|" << std::endl;
		}
	}
	printf("|_____|______________________________|____________________|\n");
}
