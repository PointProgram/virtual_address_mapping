# General problem

Implementation of a program in the C language that reads a sequence of commands (programs) from a file and replaces virtual addresses with physical ones in commands defined by an option. The type of memory organization is also determined by the option. Address replacement occurs if the page and/or segment is in RAM. If the required virtual page and/or segment is missing from the OP, then a page/segment missing error message should be displayed and command parsing should continue. The table of pages/segments is specified in a CSV format file.

# Main components

1. The module for reading a table of pages/segments from a CSV file and creating an internal representation of the corresponding table (or tables);

2. A module with the implementation of the functions of reading, analyzing and changing commands from a binary file, which replaces virtual addresses in the read commands with physical ones;

3. Testing module containing tests of the implemented program.

| ***Type of memory organization\*** | ***Virtual Address Options\*** | ***List of commands\*** |
| ----------------------------- | ----------------------------------- | ----------------------------- |
| segment-page | RS: 512 bytes, RTD: 8192 | 1, 3, 6, 7, 12, 14, 25, 26, 28 |

***Command system:\***

| ***№\*** | ***Team\*** | ***Command Code (\******0x\******)\*** | ***Description\*** |
| -------- | --------------------------- | ------------------------------------- | ----------------------------------------------------- ---------- |
| 1 | MOV <reg1>, <reg2> | 1A /reg1 /reg2 | move value from register <reg1> to register <reg2> |
| 3 | MOV <addr>, <reg> | 1B 1 ​​/reg /addr | move the value from the register <reg> to the OP at the address <addr> |
| 6 | ADD <reg1>, <reg2>, <reg3> | 03 0 /reg1 /reg2 /reg3 | adding the value from register <reg2> to the value from register <reg3> and saving the result in register <reg1> |
| 7 | ADD <reg1>, <reg2>, <addr> | 04 /reg1 /reg2 /addr | adding the value from the register <reg2> to the 4-byte value from the OP at the address <addr> and saving the result in the register <reg1> |
| 12 | MUL <reg1>, <reg2> | 20 /reg1 /reg2 | multiplying the value from register <reg1> by the value from register <reg2> and saving the result in register <reg1>. Multiplication takes place without expansion, i.e. without saving higher digits, if the result has a digit greater than 32 bits |
| 14 | MUL <reg>, <addr> | 22 0 /reg /addr | multiplying the value from the <reg> register by the 4-byte value from the OP with the address <addr> and saving the result in the <reg> register. Multiplication takes place without expansion, i.e. without saving higher digits, if the result has a digit greater than 32 bits |
| 25 | JG <addr> | 95 /addr | jump by 4-byte address <addr> if ZF = 0 and SF = OF |
| 26 | CMP <reg1>, <reg2> | 80 /reg1 /reg2 | comparing two values ​​and setting the appropriate flags |
| 28 | MOV <reg>, <lit16> | 1C 1 /reg /lit16 | moving a 2-byte number to the register <reg> |