# General problem

Implementation of a program in the C language that reads a sequence of commands (programs) from a file and replaces virtual addresses with physical ones in commands defined by an option. The type of memory organization is also determined by the option. Address replacement occurs if the page and/or segment is in RAM. If the required virtual page and/or segment is missing from the OP, then a page/segment missing error message should be displayed and command parsing should continue. The table of pages/segments is specified in a CSV format file.

# Main components

1. The module for reading a table of pages/segments from a CSV file and creating an internal representation of the corresponding table (or tables);

2. A module with the implementation of the functions of reading, analyzing and changing commands from a binary file, which replaces virtual addresses in the read commands with physical ones;

3. Testing module containing tests of the implemented program.

# Type of the memory and requiremenets for solving the problem

| Type of memory organization | Virtual Address Options | List of commands |
| ---------------------- | --------------------------- | --------------------------------- |
| page | PC: 4 KB | 1, 2, 12, 14, 24, 25, 26, 27, 29 |

***Command system:\***

| No. | Team | Command code (0x) | Description |
| ---- | -------------------- | ---------------- | ----------------------------------------------------- ---------- |
| 1 | MOV <reg1>, <reg2> | 1A /reg1 /reg2 | move value from register <reg1> to register <reg2> |
| 2 | MOV <reg>, <addr> | 1B 0 /reg /addr | move the value from the OP at the address <addr> to the register <reg> |
| 12 | MUL <reg1>, <reg2> | 20 /reg1 /reg2 | multiplying the value from register <reg1> by the value from register <reg2> and saving the result in register <reg1>. Multiplication takes place without expansion, i.e. without saving higher digits, if the result has a digit greater than 32 bits |
| 14 | MUL <reg>, <addr> | 22 0 /reg /addr | multiplying the value from the <reg> register by the 4-byte value from the OP with the address <addr> and saving the result in the <reg> register. Multiplication takes place without expansion, i.e. without saving higher digits, if the result has a digit greater than 32 bits |
| 24 | JG <shift> | 94 /shift | shift by 1-byte relative offset <shift> if ZF = 0 and SF = OF |
| 25 | JG <addr> | 95 /addr | jump by 4-byte address <addr> if ZF = 0 and SF = OF |
| 26 | CMP <reg1>, <reg2> | 80 /reg1 /reg2 | comparing two values ​​and setting the appropriate flags |
| 27 | MOV <reg>, <lit8> | 1C 0 /reg /lit8 | moving a 1-byte number to the register <reg> |
| 29 | MOV <reg>, <lit32> | 1C 2 /reg /liy32 | moving a 4-byte number to the register <reg> |