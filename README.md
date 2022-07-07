<h1>Mapping from a virtual to a physical address\</h1>


## General info
Reading a sequence of commands (programs) from a file and replacing virtual addresses with physical ones in commands defined by an option. The type of memory organization is also determined by the option. Address replacement occurs if the page and/or segment is in RAM. If the required virtual page and/or segment is missing from the OP, then a page/segment missing error message should be displayed and command parsing should continue. The table of pages/segments is specified in a CSV format file.

## Main components

1. The module for reading a table of pages/segments from a CSV file and creating an internal representation of the corresponding table (or tables);
2. A module with the implementation of the functions of reading, analyzing and changing commands from a binary file, which replaces virtual addresses in the read commands with physical ones;
3. Testing module containing tests of the implemented program.


## Technologies
Project is created with:
* C11: ISO/IEC 9899:2011
* gcc: version 6.3.0

	
## Setup
To run this project, install CodeBlocks or similar IDE with complilers that support MingW64 features, so that \<windows.h> and \<conio.h> libraries could be launched. Then git clone and run a program with the following commands:

## Input data

The following files are submitted to the program input:
• CSV files with segment and/or page tables - memory table files.

• A binary file containing some sequence of commands (a program) — a program file.

## Command codes

Features of this architecture and notations:
• commands of the command system operate on data located in one of the 16 32-bit general-purpose registers (R0-R15) or in the RAM;
• the flags are similar to the Intel architecture flags;
• <reg> in table 1 denotes one of the general-purpose registers;
• <addr> — virtual address;
• <shift8> — a signed integer shift relative to the current command;
• <lit8>, <lit16>, <lit32> - constant;
• command codes in the table are indicated by hexadecimal digits and /regX symbols,
/addr, /shift;
• /reg, /reg1, /reg2, /reg3 — register number, 4 bits;
• /addr — address, 32 bits;
• /shift — shift relative to the current address, 8 bits;
• /lit8, /lit16, /lit32 — an unsigned number of 8, 16, and 32 bits, respectively.


| N  | Command                       | Command code (0X)         | Description                                                                                                                                                                                                                                                                      |
|----|-------------------------------|---------------------------|----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| 1  | MOV \<reg1>, \<reg2>            | 1A /reg1 /reg2            | shift the value from the register \<reg1> to register \<reg2>                                                                                                                                                                                                                      |
| 2  | MOV \<reg>, \<addr>             | 1B 0 /reg /addr           | move the value from the OP at the address \<addr> to register \<reg>                                                                                                                                                                                                               |
| 3  | MOV \<addr>, \<reg>             | 1B 1 /reg /addr           | move the value from the register \<reg> to the OP at the address \<addr>                                                                                                                                                                                                           |
| 4  | ADD \<reg1>, \<reg2>            | 01 /reg1 /reg2            | adding the value from the register \<reg1> to the value from the register \<reg2> and saving the result in the register \<reg1>                                                                                                                                                     |
| 5  | ADD \<reg>, \<addr>             | 02 0 /reg /addr           | adding value from register \<reg> to a 4-byte value from the OP at the address \<addr> and saving the result in the register \<reg>                                                                                                                                                 |
| 6  | ADD \<reg1>, \<reg2>, \<reg3>   | 03 0 /reg1 /reg2 /reg3    | adding the value from the register \<reg2> to the value from the register \<reg3> and saving the result in the register \<reg1>                                                                                                                                                     |
| 7  | ADD \<reg1>, \<reg2>, \<addr>    | 04 /reg1 /reg2 /addr      | adding the value from the register \<reg2> to the 4-byte value from the OP at the address \<addr> and saving the result in the register \<reg1>                                                                                                                                     |
| 8  | SUB \<reg1>, \<reg2>            | 0A /reg1 /reg2            | subtracting from the value in register \<reg1> the value from register \<reg2> and saving the result in register \<reg1>                                                                                                                                                            |
| 9  | SUB \<reg>, \<addr>             | 0B 0 /reg /addr           | subtraction from the value in the \<reg> register of the 4-byte value from the OP at the address \<addr> and saving the result in the \<reg> register                                                                                                                               |
| 10 | SUB \<reg1>, \<reg2>, \<reg3>    | 0C 0 /reg1 /reg2 /reg3    | subtracting from the value in register \<reg2> the value from register \<reg3> and saving the result in register \<reg1>                                                                                                                                                            |
| 11 | SUB \<reg1>, \<reg2>, \<addr>    | 0D /reg1 /reg2 /addr      | subtraction from the value in the \<reg2> register of the 4-byte value from the OP at the address \<addr> and saving the result in the \<reg1> register                                                                                                                             |
| 12 | MUL \<reg1>, \<reg2>            | 20 /reg1 /reg2            | multiplying the value from register \<reg1> by the value from register \<reg2> and saving the result in register \<reg1>. Multiplication occurs without expansion, i.e. without saving higher digits, if the result has a bit size greater than 32 bits                             |
| 13 | MUL \<reg1>, \<reg2>, \<reg3>    | 21 0 /reg1 /reg2 /reg3    | multiplying the value from register \<reg2> by the value from register \<reg3> and storing the result in registers \<reg1> (higher bits of the result) and \<reg2> (lower bits of the result)                                                                                        |
| 14 | MUL \<reg>, \<addr>             | 22 0 /reg /addr           | multiplying the value from the \<reg> register by the 4-byte value from the OP with the address \<addr> and saving the result in the \<reg> register. Multiplication occurs without expansion, i.e. without saving higher digits, if the result has a bit size greater than 32 bits |
| 15 | MUL \<reg1>, \<reg2>, \<addr>    | 23 /reg1 /reg2 /addr      | multiplying the value from the register \<reg2> by the 4-byte value from the OP at the address \<addr> and saving the result in the registers \<reg1> (higher bits of the result) and \<reg2> (lower bits of the result)                                                             |
| 16 | DIV \<reg1>, \<reg2>            | 2A /reg1 /reg2            | dividing the value from register \<reg1> by the value from register \<reg2> and saving the result in register \<reg1>. The whole part from the division is saved, and the remainder is not saved                                                                                    |
| 17 | DIV \<reg1>, \<reg2>, \<reg3>    | 2B 0 /reg1 /reg2 /reg3    | dividing the value from register \<reg2> by the value from register \<reg3> and saving the result in registers \<reg2> (whole part) and \<reg1> (remainder)                                                                                                                          |
| 18 | DIV \<reg>, \<addr>             | 2C 0 /reg /addr           | dividing the value in the \<reg> register by the 4-byte value from the OP at the address \<addr> and saving the result in the \<reg> register. The whole part from the division is saved, and the remainder is not saved                                                            |
| 19 | DIV \<reg1>, \<reg2>, \<addr>    | 2D /reg1 /reg2 /addr      | dividing the value from register \<reg2> by the 4-byte value from OP at address \<addr> and saving the result in registers \<reg2> (whole part) and \<reg1> (remainder)                                                                                                              |
| 20 | JMP \<shift>                   | 90 /shift                 | unconditional shift by 1-byte relative offset \<shift>                                                                                                                                                                                                                            |
| 21 | JMP \<addr>                    | 91 /addr                  | unconditional transition at address \<addr>                                                                                                                                                                                                                                       |
| 22 | JL \<shift>                    | 92 /shift                 | shift by 1-byte relative offset \<shift> if SF ≠ OF                                                                                                                                                                                                                               |
| 23 | JL \<addr>                     | 93 /addr                  | jump by 4-byte address \<addr> if SF ≠ OF                                                                                                                                                                                                                                         |
| 24 | JG \<shift>                    | 94 /shift                 | shift by 1-byte relative offset \<shift> if ZF = 0 and SF = OF                                                                                                                                                                                                                    |
| 25 | JG \<addr>                     | 95 /addr                  | jump to 4-byte address \<addr> if ZF = 0 and SF = OF                                                                                                                                                                                                                              |
| 26 | CMP \<reg1>, \<reg2>            | 80 /reg1 /reg2            | comparing two values and setting the corresponding flags                                                                                                                                                                                                                         |
| 27 | MOV \<reg>, \<lit8>             | 1C 0 /reg /lit8           | moving a 1-byte number into the \<reg> register                                                                                                                                                                                                                                   |
| 28 | MOV \<reg>, \<lit16>            | 1C 1 /reg /lit16          | moving a 2-byte number into the \<reg> register                                                                                                                                                                                                                                   |
| 29 | MOV \<reg>, \<lit32>            | 1C 2 /reg /lit32          | moving a 4-byte number into the \<reg> register                                                                                                                                                                                                                                   |

## Recognition and output of commands

Let us have the following contents of the program file:

1C 00 02 // MOV R0, 2
1B 01 04 00 00 1A // MOV R1, [0x00C0001A]
01 01 // ADD R0, R1
91 FF FF FF 00 // JMP [0xFFFFFF00]

(Contents are given as hexadecimal numbers for clarity - they should be binary numbers when performing the task. To quickly create such binary files, you can use online converters, such as the one at tomeko.net.)
When reading each subsequent command, a line with a text representation of the command should be output to the terminal - in the example above, such a line is marked after //. Square brackets indicate a virtual or physical address.

## Types of memory organization

1. Page
2. Segment
3. Segment-Page

## Page memory organization

When memory is paged, the virtual address space is divided into pages
— fragments of the address space of a fixed length. Since the virtual address space is usually much larger than the size of main memory, the required pages are loaded into main memory when they are needed (an instruction on a page is moved, or data on that page is accessed). At the same time, the program code operates with virtual addresses. To obtain a physical address, the address in main memory at which the addressed instruction or data resides a page table is used, which contains information about whether a particular virtual page is present in main memory, and if so, then which page frame corresponds to it (that is, where is this virtual page in main memory).

Let's suppose that there is 32-bit address, where 20 bits are responsible for the number of the virtual page and 12 bits are in charge of the page offset. Consequently, page size is 4 Kbytes (2 ^ 12).

The size of the physical address is 14 bits, that is, the main memory can contain up to 16 Kbytes. It is worth noting that the size of the page may differ, accordingly, the bit size of the page number field in 32 bits of the virtual address may also differ.

Page table: 
1 column (page number):  | 4 | 3 | 2 | 1 | 0 |
2 column (presence bit): | 1 | 0 | 1 | 0 | 1 |
3 column (frame number): | 10|   | 00|   | 01|

Physical address:

| 0 | 0 | 0 | 0 | 1 | 1 | 1 | 1 | 0 | 1 | 1 | 0 | 0 | 1 |
| frame |				offset in the page				|
| number|												|

Virtual address:

0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|0|1|0|0|0|1|1|1|1|0|1|1|0|0|1|
		Page number					   | offset in the page    |
		
The structure of the CSV file of the page table is as follows: each row contains a page number,
the page presence flag in the main memory and the page frame number of the virtual page.
If during the conversion of the virtual address it turns out that the virtual page is missing in the main memory, it is necessary to display a page missing error message in the terminal, specifying the entire command and the specific address for which the error occurred.
If during the conversion of the virtual address it turns out that the virtual page specified in the address does not exist, a corresponding message should also be displayed.
		
## Segment memory organization

A segment is a separate address space that contains some structure, procedure, array, or other object. Memory segmentation makes it possible to represent the virtual address space as a set of independent blocks of variable length (segments). The program can consist of several segments. Each segment is described in a segment descriptor stored in the segment descriptor table. Virtual addresses in segmental organization consist of the segment number (number of the segment descriptor in the table) and the offset in this segment.

Virtual addres structure:

[31 - 16] | [15 - 0]
Segment descriptor number | offset in the segment

It is worth noting that the bit size of the descriptor number can be different within the 32 bits of the virtual address.
To convert a virtual address into a physical one, it is necessary to take the number of the segment descriptor from the virtual address and obtain the corresponding descriptor (Fig. 3). The descriptor consists of a base address, a segment size field (in bytes), and flags, including the presence in main memory flag. From the segment descriptor, it is necessary to obtain information about the base address (initial address) of the segment in the main memory. Then add the offset from the virtual address to the base address and get the physical address.

Segment descriptor:

[63 - 32] | [31 - 12] | [11 - 0]
Segment descriptor number | offset in the segment | flags

The structure of the CSV file of the table of page segment descriptors is as follows: each line contains the base address, the size of the segment (in bytes), and the presence flag of the segment in main memory.
If during the conversion of the virtual address it turns out that the segment is missing in the main memory, it is necessary to output a segment missing error message to the terminal, specifying the entire command and the specific address for which the error occurred.
If during virtual address conversion it turns out that the segment specified in the address does not exist, or if the offset in the segment exceeds the segment size, a corresponding message should also be output.

## Segment-page memory organization

With segment-page memory organization, the program is divided into segments, and the segments are divided into fixed-length pages. Accordingly, the virtual address in the address space of the program consists of a segment identifier, a virtual page number, and an offset within the page. It is worth noting that the bit size of the descriptor number and the page size can be different within the 32 bits of the virtual address (specified as an option).

Virtual addres structure:

[31 - 22] | [21 - 12] | [11 - 0] 
Segment descriptor number | page number | offset in the page

Unlike the segment organization, the descriptor of segments with segment-page organizations instead of the base address of the segment contains the address of the page table, the number of pages of the segment and a set of flags. We will assume that the page table is stored not in the main memory, but in a separate one, so when implementing a reference to the table in C, you should use a reference to the table object instead of an address.

Segment descriptor:

[63 - 32] | [31 - 22] | [21 - 0]
Address of the page table | page number | flags

The structure of the CSV file of the page segment descriptor table is as follows: each line contains the path to the page table file and the number of pages of the segment (there are no flags).
The structure of the page table CSV file is the same as for paged memory.
If, during the conversion of the virtual address, it turns out that the segment is missing in the table of descriptors, it is necessary to display a segment missing error message in the terminal, specifying the entire command and the specific address for which the error occurred. Similarly, a page-missing fault must be reported when a page is missing from main memory.
If the page number exceeds the size of the segment (the number of its pages), a corresponding message should also be displayed.
