# ArbAsmCli
Console calculator for arbitrary precision arithmetic with an assembly language-styled syntax.\
I find the notation and syntax of assembly languages to be an interesting candidate for using in calculators. A long term, distant goal is making a standalone handheld calculator with the functions and conventions outlined by this implementation.\
Internally, it does not have much to do with actual assembly language. The prime example of this the dynamic nature of the registers and their lengths (that also being its main feature).\

## Compilation
I'm not an expert on this, but
```
gcc ArbAsmCli.c -lm -o ArbAsmCli
```
should do the trick (it's what I do).

## Overview
Every statement looks like this:
```
<instruction mnemonic> <arguments seperated by commata>;
```
(note the semicolon at the end)\
Arguments are either registers or numbers.\
When an instruction is executed, its primary result is printed to the screen. What its primary result is, is detailed in the 'Instruction mnemonics' section.\
The notation of both inputs and outputs is little-endian by default, opposite of what is standard in english. Should one desire it any different, endianness can be changed (see the `set` mnemonic and `endia` register).

### Numbers
Numbers can be of any length, and must be a continuous string of decimal digits. To input the sign of a number, a `+` or `-` can be placed at the end. The default is positive.\
Some examples:\
`314159265358979`, `310771-` `00003+`\
Once again, the notation is little-endian by default, so `00003` is thirty thousand. The place of the sign does not change in the different endiannesses, so in big endian notation mode, `-177013` will not be recognized and should be `177013-`.

### Registers
Registers are effectively built-in variables. There are three types of registers: General purpose, Environment and `flag`.\
There are 3 general purpose registers: `gr1`, `gr2`, `gr3`. These are supposed to be used for storing and modifying numbers.
The Environment registers influence the operation of the program:
|Register|Length|Can be changed (with effect)|Description|
|--------|------|--------------|--------|
|`inplen`|variable|yes|Holds the maximum length of user input.|
|`endia`|1|yes|Holds the endianness, 0 for little endian, 1 for big endian.|
|`stacsz`|variable|yes|Holds the size of the stack, note that the stack will be cleared if this register is re-set.|
|`time`|variable|no|Contains the amount of seconds the previous statement took to execute.|

The final type of register is the `flag` register. It holds the result of the comparison instructions `cmp` and `scmp`.

### Instruction mnemonics
The first thing in a statement is always the instruction mnemonic.
|Mnemonic|Intended Syntax|Name and description|Primary result|
|--------|---------------|-----------|--------------|
|`set`|`<register>, <register/number>`|Set. Sets the first argument equal tot the second argument, completely overriding the first argument|The number that the first argument has been set to|
|`inc`|`<register/number>`|Increment. Increments the argument by 1, re-setting the first argument by the result (if it is a register).|The result of the operation|
|`add`|`<register/number>, <register/number>`|Addition. Adds the two arguments together, storing the result in the first argument (if it is a register).|The result of the operation|
|`mul`|`<register/number>, <register/number>`|Multiplication. Multiplies the two arguments together, storing the result in the first argument (if it is a register).|The result of the operation|
|`cmp`|`<register/number>, <register/number>`|Unsigned compare. Compares the two numbers as if they were unsigned, stores the result in the `flag` register.|The `flag` register|
|`scmp`|`<register/number>, <register/number>`|Signed compare. Compares the two numbers as if they were unsigned, stores the result in the `flag` register.|The `flag` register|
|`print`|`<register/number>`|Print. Prints the number of the first argument to the console. It is effectively a no-op in the cli.|The first argument|
|`push`|`<register/number>`|Push. Pushes the first argument to the stack.|The first argument|
|`pop`|`<register>`|Pop. Pops the number on top of the stack and sets the first argument to that number.|The number that was popped|
|`len`|`<register/number>, <register>`|Length. Gets the length of the number of the first argument and sets the second argument into that number|The length of the number of the first argument|
