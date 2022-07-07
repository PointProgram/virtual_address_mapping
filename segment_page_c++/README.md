# General problem

Implementation of a program in the C language that reads a sequence of commands (programs) from a file and replaces virtual addresses with physical ones in commands defined by an option. The type of memory organization is also determined by the option. Address replacement occurs if the page and/or segment is in RAM. If the required virtual page and/or segment is missing from the OP, then a page/segment missing error message should be displayed and command parsing should continue. The table of pages/segments is specified in a CSV format file.

# Main components

1. The module for reading a table of pages/segments from a CSV file and creating an internal representation of the corresponding table (or tables);

2. A module with the implementation of the functions of reading, analyzing and changing commands from a binary file, which replaces virtual addresses in the read commands with physical ones;

3. Testing module containing tests of the implemented program.

# Type of the memory and requirements for solving the problem

| **Type of memory organization** | **Virtual Address Options** | **List of commands** |
| --------------------------- | --------------------------------- | ---------------------------- |
| segment-page | RS: 4 KB, RTD: 1024 | 1, 2, 3, 8, 9, 22, 23, 26, 27 |

***Command system:\***

| **No.** | **Team** | **Command Code (0x****)** | **Description** |
| ----- | -------------------- | ------------------------ | ----------------------------------------------------- ---------- |
| 1 | MOV <reg1>, <reg2> | 1A /reg1 /reg2 | move value from register <reg1> to register <reg2> |
| 2 | MOV <reg>, <addr> | 1B 0 /reg /addr | move the value from the OP at the address <addr> to the register <reg> |
| 3 | MOV <addr>, <reg> | 1B 1 ​​/reg /addr | move the value from the register <reg> to the OP at the address <addr> |
| 8 | SUB <reg1>, <reg2> | 0A /reg1 /reg2 | subtracting from the value in the register <reg1> the value from the register <reg2> and saving the result in the register <reg1> |
| 9 | SUB <reg>, <addr> | 0B 0 /reg /addr | subtraction from the value in the <reg> register of the 4-byte value from the OP at the address <addr> and saving the result in the <reg> register |
| 22 | JL <shift> | 92 /shift | transition by 1-byte relative offset <shift> in case SF ≠ OF |
| 23 | JL <addr> | 93 /addr | jump by 4-byte address <addr> if SF ≠ OF |
| 26 | CMP <reg1>, <reg2> | 80 /reg1 /reg2 | comparing two values ​​and setting the appropriate flags |
| 27 | MOV <reg>, <lit8> | 1C 0 /reg /lit8 | moving a 1-byte number to the register <reg> |