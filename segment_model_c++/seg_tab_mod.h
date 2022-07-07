#pragma once
#include <vector>
#include <string>

typedef struct segment_table {
	std::string basic_adress = "";
	int byte_size = 0;
	unsigned int mem_presence = 0;
} descriptor;

const int DESCR_SIZE = 10;

class ReadPage {
public:
	std::vector <descriptor> page_info;

private:
	void push(std::string addr, int b_size, unsigned int m_presence);
	descriptor split_record(std::string string_val);
public:
	void parse_descriptor_table(const char* filename);
	void display_descriptor_page(std::vector<descriptor> page);
};

std::string convert_dig_bit(int frame_digit, unsigned len);


