# General problem

Implementation of a program in the C language that reads a sequence of commands (programs) from a file and replaces virtual addresses with physical ones in commands defined by an option. The type of memory organization is also determined by the option. Address replacement occurs if the page and/or segment is in RAM. If the required virtual page and/or segment is missing from the OP, then a page/segment missing error message should be displayed and command parsing should continue. The table of pages/segments is specified in a CSV format file.

# Main components

1. The module for reading a table of pages/segments from a CSV file and creating an internal representation of the corresponding table (or tables);

2. A module with the implementation of the functions of reading, analyzing and changing commands from a binary file, which replaces virtual addresses in the read commands with physical ones;

3. Testing module containing tests of the implemented program.

# Type of the memory and requiremenets for solving the problem

| Type of memory organization | Virtual Address Options | List of commands |
| ---------------------- | --------------------------- | ----------------------------- |
| Segmental | RTD: 1024 | 1, 3, 4, 5, 20, 21, 26, 27, 28 |

 

| No. | Team | Command code (0x) | Description |
| ---- | ------------------- | ----------------- | ----------------------------------------------------- ---------- |
| 1 | MOV <reg1>, <reg2> | 1A /reg1 /reg2 | move value from register <reg1> to register <reg2> |
| 3 | MOV <addr>, <reg> | 1B 1 ​​/reg /addr | move the value from the register <reg> to the OP at the address <addr> |
| 4 | ADD <reg1>, <reg2> | 01 /reg1 /reg2 | adding the value from register <reg1> to the value from register <reg2> and saving the result in register <reg1> |
| 5 | ADD <reg>,<addr> | 02 0 /reg /addr | adding the value from the register <reg> to the 4-byte value from the OP at the address <addr> and saving the result in the register <reg> |
| 20 | JMP <shift> | 90 /shift | unconditional shift by 1-byte relative offset <shift> |
| 21 | JMP <addr> | 91 /addr | unconditional transition at address <addr> |
| 26 | CMP <reg1>, <reg2> | 80 /reg1 /reg2 | comparing two values ​​and setting the appropriate flags |
| 27 | MOV <reg>, <lit8> | 1C 0 /reg /lit8 | moving a 1-byte number to the register <reg> |
| 28 | MOV <reg>, <lit16> | 1C 1 /reg /lit16 | moving a 2-byte number to the register <reg> |
 