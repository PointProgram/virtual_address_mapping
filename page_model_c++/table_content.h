/*
PRESENT INFO:
Virtual memory structure:
|DESCRIPTOR_NUM |PAGE_NUM    |PAGE_OFFSET|
| ************* | ********** | ********* |
|	13 bits		|	10 bits	 |	9 bits	 |
*/
#pragma once
#include <string>
#include <vector>

struct fixed_page {
	int page_num;
	short page_flag;
	std::string page_frame;
};

struct descriptor_segment {
	std::string path;
	int page_size;
	std::vector<struct fixed_page> pg_table;
};

class struct_transfer {
public:
	std::vector <struct descriptor_segment> segment_struct;
private:
	std::vector<struct fixed_page> page_read_write(std::string page_path);
public:
	void descriptor_read_write(std::string file_name);
	void output_struct_tables();
};



