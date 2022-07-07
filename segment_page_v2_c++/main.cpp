#include "bin_transform.h"
#include "csv_read.h"

#define TABLE_PATH "data_files/descriptor/descriptor_tab.csv"
#define DATA_FILE_PATH "data_files/program/test1.dat"

int main() {
	struct_transfer convert_files;
	std::string desc_path, test_path, hex_data;

	//transfer info from files to vector
	convert_files.descriptor_read_write(TABLE_PATH);
	//display info from files on the screen
	convert_files.output_struct_tables();

	command_form genereate_program;
	//get vector with table data
	std::vector <struct descriptor_segment> descrptr = convert_files.segment_struct;

	//test_path = "data_files/program/test2.dat";
	//test_path = "data_files/program/test3.dat";

	hex_data = genereate_program.hex_tranversion(DATA_FILE_PATH);
	//convert binary data to hex
	genereate_program.set_hex_info(hex_data);
	bool file_output = true;
	//output transformed program
	genereate_program.commands_conversion(descrptr, file_output);
}
