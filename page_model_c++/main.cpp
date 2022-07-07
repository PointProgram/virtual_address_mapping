#include "commands_implementation.h"
#include "table_content.h"

//path to the location of descriptor table
#define DESCRIPTOR_TABLE_PATH "data_files/descriptor/descriptor_tab.csv"
//path to the location of test file
#define DATA_FILE_PATH "data_files/program/test1.dat"

int main() {
	struct_transfer convert_files;
	std::string desc_path, test_path, hex_data;

	//transfer info from files to vector
	convert_files.descriptor_read_write(DESCRIPTOR_TABLE_PATH);
	//display info from files on the screen
	convert_files.output_struct_tables();

	command_form genereate_program;
	//get vector with table data
	std::vector <struct descriptor_segment> descrptr = convert_files.segment_struct;
	
	//path to the location of test file
	//test_path = "data_files/program/test2.dat";
	//test_path = "data_files/program/test3.dat";

	hex_data = genereate_program.hex_tranversion(DATA_FILE_PATH);
	//convert binary data to hex
	genereate_program.set_hex_info(hex_data);
	bool file_output = true;
	//output transformed program
	genereate_program.commands_conversion(descrptr, file_output);
}
