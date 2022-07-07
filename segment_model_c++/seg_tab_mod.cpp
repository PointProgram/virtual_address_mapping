#include <sstream>
#include <fstream>
#include <iostream>
#include <iomanip>
#include "seg_tab_mod.h"

//додавання елементів до вектора таблиці дескрипторів
void ReadPage::push(std::string addr, int b_size, unsigned int m_presence) {
	descriptor tmp_store;
	tmp_store.basic_adress = addr;
	tmp_store.byte_size = b_size;
	tmp_store.mem_presence = m_presence;
	page_info.push_back(tmp_store);
}

//перетворити цілочисельне число в бітове представлення
std::string convert_dig_bit(int frame_digit,  unsigned len) {
	std::string bin_str = "";
	int bit_op = 1;
	for (size_t i = 0; i < len; i++) {
		if ((bit_op & frame_digit) >= 1)
			bin_str = "1" + bin_str;
		else
			bin_str = "0" + bin_str;
		bit_op <<= 1;
	}
	return bin_str;
}

//розділити рядок csv файлу на елементи таблиці дескрипторів
descriptor ReadPage::split_record(std::string string_val) {
	descriptor pgTab;
	std::string word;
	std::stringstream  s_word(string_val);
	std::getline(s_word, word, ';');
	if (word != "") {
		pgTab.basic_adress = word;
	}
	std::getline(s_word, word, ';');
	pgTab.byte_size = stoi(word);
	std::getline(s_word, word, ';');
	pgTab.mem_presence = stoi(word);
	return pgTab;
}

//прочитати вміст файлу таблиць дескрипторів та перенести дані у вектор структури програми
void ReadPage::parse_descriptor_table(const char* filename) {
	std::fstream newfile;
	newfile.open(filename, std::ios::in);
	
	descriptor pgTab, * pageArr = NULL;
	if (newfile.is_open()) {
		std::string tp;
		while (getline(newfile, tp)) {
			pgTab = split_record(tp);
			push(pgTab.basic_adress, pgTab.byte_size, pgTab.mem_presence);
		}
	}
	newfile.close();
}

//вивести на екран вміст таблиці дескрипторів
void ReadPage::display_descriptor_page(std::vector<descriptor> page) {
	std::cout << std::setw(5) << "Num." << std::setw(20) << "Base address" << std::setw(10) << "   Byte size" << std::setw(10) << "   Mem. presence" << std::endl;
	for (size_t i = 0; i < ReadPage::page_info.size(); i++) {
		std::cout <<std::setw(5) << i << std::setw(20) << page_info[i].basic_adress << std::setw(10) << page_info[i].byte_size << std::setw(10) << page_info[i].mem_presence << std::endl;

	}

}
