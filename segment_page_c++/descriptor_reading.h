#pragma once
#include <vector>
#include <string>
//page structure
typedef struct page_table {
	int pg_num;
	bool ch_bit;
	std::string fram_num;
} page_row;

//descriptor structure
typedef struct segment_table {
	std::string object_reference;
	std::vector<page_row> linked_page_elems;
	int plan_pg_num;
} descriptor;

//size of descriptor (10 bits)
const int DESCRIPTOR_SIZE = 10;
std::string to_binary(int frame_digit, unsigned length);

class FILE_READ {
public:
	std::vector <descriptor> page_info;

private:
	void push(std::string obj_adress, int pg_num, std::vector<page_row> link_page);
	void split_descriptor_record(descriptor& getTable, std::string string_row);
	page_row split_page_record(std::string string_row);
	std::vector<page_row> transfer_page_table(descriptor& getStruct, std::string obj_address);
public:
	void fill_descriptor(std::string file);
	void display_descriptor(std::vector<descriptor> page);
};



