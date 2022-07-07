#include "bin_analysis.h"
#include "seg_tab_mod.h"
#include <stdio.h>

#define TABLE_PATH "data_files/segment_table.csv"
#define DATA_FILE_PATH "data_files/pr_1_1.dat"

int main() {
	std::string hex_ar;
	ReadPage reading;
	//����������� ����� ����� � ��������� ��������
	reading.parse_descriptor_table(TABLE_PATH);
	//��������� �� ����� ������� ��������
	reading.display_descriptor_page(reading.page_info);
	DataAnalysis chng;
	//������� ����� �������� �����
	//�������� �� �����
	hex_ar = chng.binary_read(DATA_FILE_PATH);
	//hex_ar = chng.binary_read("pr_2_2.dat");
	//hex_ar = read_bin_file("program_data2.dat");
	//��������� ���������� �������� �� �����
	printf("\nProgram code: \n__________________________________\n");
	chng.iterative_analysis(hex_ar, reading.page_info);
}
