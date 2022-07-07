#include "hex_decoder.h"
#include "descriptor_reading.h"

#define TABLE_PATH "page_tables/descriptor_table.csv"
#define DATA_FILE_PATH "program_files/prog_test_2.dat"

int main() {
	FILE_READ reader;
	//transfer data from files to structures
	reader.fill_descriptor(TABLE_PATH);
	//displat content of the files on the screen
	reader.display_descriptor(reader.page_info);
	CODE_GEN genereate_program;
	//������� ����� �������� �����
	//�������� �� �����
	std::string hexademical_repr;
	hexademical_repr = genereate_program.binary_read(DATA_FILE_PATH);
	genereate_program.code_converter(hexademical_repr, 0, reader.page_info);
}
