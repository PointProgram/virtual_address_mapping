#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "descriptor_reading.h"

//push new element to the descriptor structure array
void FILE_READ::push(std::string obj_adress, int pg_num, std::vector<page_row> link_page) {
	descriptor descriptor_element;
	descriptor_element.object_reference = obj_adress;
	descriptor_element.plan_pg_num = pg_num;
	descriptor_element.linked_page_elems = link_page;
	page_info.push_back(descriptor_element);
}

//turn digital number to binary string
std::string to_binary(int frame_digit,  unsigned length) {
	std::string binary = ""; int offset_bit = 1;
	for (size_t i = 0; i < length; i++) {
		((offset_bit & frame_digit) >= 1) ? binary = "1" + binary : binary = "0" + binary;
		offset_bit <<= 1;
	}
	return binary;
}

//divide page file to elements of page structure
page_row FILE_READ::split_page_record(std::string string_row) {
	page_row getPage;
	std::string split_value;
	std::stringstream  fraction(string_row);
	std::getline(fraction, split_value, ';');
	getPage.pg_num = stoi(split_value);
	std::getline(fraction, split_value, ';');
	getPage.ch_bit = (stoi(split_value) == 1) ? true : false;
	if (split_value == "1") {
		std::getline(fraction, split_value, ';');
		getPage.fram_num = to_binary(stoi(split_value), 2);
	}
	return getPage;
}

//transfer the data from page table file to page table vector of structures
std::vector<page_row> FILE_READ::transfer_page_table(descriptor& getStruct, std::string obj_address) {
	std::fstream page_table_;
	std::vector<page_row> vec_for_descriptor;
	page_table_.open(obj_address, std::ios::in);

	if (page_table_.is_open()) {
		std::string row_string;
		while (getline(page_table_, row_string))
			vec_for_descriptor.push_back(split_page_record(row_string));
	}
	else {
		std::cout << "Unfortunately file located in '" << obj_address << "' caused an error while opening\n";
		exit(1);
	}
	page_table_.close();
	return vec_for_descriptor;
}

//divide strings of descriptor file to elements of descriptor array structure
void FILE_READ::split_descriptor_record(descriptor& getTable, std::string string_row) {
	std::string split_value;
	std::stringstream  fraction(string_row);
	std::getline(fraction, split_value, ';');
	getTable.object_reference = split_value;
	getTable.linked_page_elems = transfer_page_table(getTable, split_value);
	std::getline(fraction, split_value, ';');
	getTable.plan_pg_num = stoi(split_value);
}

//read the content of the descriptor table file and transfer data into descriptor array structure 
void FILE_READ::fill_descriptor(std::string file) {
	std::fstream read_descriptor;
	read_descriptor.open(file, std::ios::in);
	
	descriptor descriptor_struct;
	if (read_descriptor.is_open()) {
		std::string row_string;
		while (getline(read_descriptor, row_string)) {
			split_descriptor_record(descriptor_struct, row_string);
			push(descriptor_struct.object_reference, descriptor_struct.plan_pg_num, descriptor_struct.linked_page_elems);
		}
	}
	read_descriptor.close();
}

//display content of the descriptor tables
void FILE_READ::display_descriptor(std::vector<descriptor> page) {
	std::cout << std::setw(5) << "Num:" << std::setw(15) << "Address: " << std::setw(20) << "Page num: " << std::endl;
	for (size_t i = 0; i < FILE_READ::page_info.size(); i++) {
		std::cout <<std::setw(5) << i << "->>| "<< std::setw(20) << page_info[i].object_reference << std::setw(10) << page_info[i].plan_pg_num << std::endl << std::endl;
		std::cout << "\t" << std::setw(10) << "Page num: " << std::setw(10) << "Bit of presence: " << std::setw(10) << "Frame number: " << std::endl;
		for (size_t j = 0; j < FILE_READ::page_info[i].linked_page_elems.size(); j++) {
			std::cout << std::setw(10) << page_info[i].linked_page_elems[j].pg_num << std::setw(15) << page_info[i].linked_page_elems[j].ch_bit << std::setw(15) << page_info[i].linked_page_elems[j].fram_num << std::endl;
		}
		std::cout << std::endl;
	}

}
