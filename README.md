# ArbAsmCli
Console calculator for arbitrary precision arithmetic with an assembly language-styled syntax.\
I find the notation and syntax of assembly languages to be an interesting candidate for using in calculators. A long term, distant goal is making a standalone handheld calculator with the functions and conventions outlined by this implementation.\
Internally, it does not have much to do with actual assembly language. The prime example of this the dynamic nature of the registers and their lengths (that also being its main feature).

## Compilation
I'm not an expert on this, but
```
gcc ArbAsmCli.c -lm -o ArbAsmCli
```
should do the trick (it's what I do).

## Overview
Every statement looks like this:
```
<instruction mnemonic> <arguments seperated by commata>
```
Arguments are either registers or numbers.\
When an instruction is executed, the number it returns is printed to the screen. The notation of both inputs and outputs is little-endian by default, opposite of what is standard in english. Should one desire it any different, endianness can be changed (see the `set` mnemonic and `endia` register).

### Numbers
Numbers can be of any length, and must be a continuous string of decimal digits. To input the sign of a number, a `+` or `-` can be placed at the end. The default is positive.\
Some examples:\
`314159265358979`, `310771-` `00003+`\
Once again, the notation is little-endian by default, so `00003` is thirty thousand. The place of the sign does not change in the different endiannesses, so in big endian notation mode, `-177013` will not be recognized and should be `177013-`.

### Registers
Registers are effectively built-in variables.
|Register|Length|Can be changed (with effect)|Description|
|--------|------|----------------------------|-----------|
|`gr1`, `gr2`, `gr3`|variable|yes|The general purpose registers, indented for storing and modifying numbers.|
|`inplen`|variable|yes|Holds the maximum length of user input.|
|`endia`|1|yes|Holds the endianness, 0 for little endian, 1 for big endian.|
|`stacsz`|variable|yes|Holds the size of the stack, note that the stack will be cleared if this register is re-set.|
|`time`|variable|no|Contains the amount of seconds the previous statement took to execute.|
|`flag`|1|yes-(ish)|It contains the result of the comparison instructions `cmp` and `scmp`.|

### Instruction mnemonics
The first thing in a statement is always the instruction mnemonic. The result of an operation is stored in the first argument (if it is a register), unless stated otherwise.
#### Register control and Information
|Mnemonic|Intended Syntax|Name and description|Returns|
|--------|---------------|--------------------|--------------|
|`set`|`<register>, <register/number>`|Set. Sets the first argument as the second argument|The second argument.|
|`print`|`<register/number>`|Print. Prints the number of the first argument to the console. It is effectively a no-op in the cli.|The first argument|
|`push`|`<register/number>`|Push. Pushes the first argument to the stack.|The first argument|
|`pop`|`<register>`|Pop. Pops the number on top of the stack and sets the first argument to that number.|The number that was popped|
|`len`|`<register/number>, <register>`|Length. Gets the length of the first argument and stores the it the second argument|The length of the first argument|
|`rot`|<register/number>, <register/number>|Rotate. Rotates the first argument by the second argument.|The rotated argument|
|`shf`|<register/number>, <register/number>|Shift. Arithmetically shifts the first argument by the second argument.|The shifted argument|
|`rev`|<register/number>|Reverse. Reverses the first argument.|The reversed argument|
|`sel`|<register/number>, <register/number>, <register/number>|Selection. Selects a region of the first argument, starting at the index given by the second argument and ending at the index given by the third argument.|The selected region|

#### Arithmetic
|Mnemonic|Intended Syntax|Name and description|Returns|
|--------|---------------|--------------------|--------------|
|`inc`|`<register/number>`|Increment. Increments the first argument by 1.|The incremented argument|
|`dec`|`<register/number>`|Decrement. Decrements the first argument by 1.|The decremented argument|
|`add`|`<register/number>, <register/number>`|Addition. Adds the two arguments together.|The sum of the arguments|
|`sub`|`<register/number>, <register/number>`|Subtraction. Subtracts the second argument from the first argument.|The first argument minus the second argument|
|`mul`|`<register/number>, <register/number>`|Multiplication. Multiplies the two arguments together.|The product of the two arguments|

#### Logic
|Mnemonic|Intended Syntax|Name and description|Returns|
|--------|---------------|--------------------|--------------|
|`cmp`|`<register/number>, <register/number>`|Signed compare. Compares the two arguments. Stores the result in the `flag` register.|0 if the arguments are equal, 1 if the first argument is the largest, 2 if the second argument is the largest.|
|`ucmp`|`<register/number>, <register/number>`|Unsigned compare. Compares the two arguments as if they were unsigned. Stores the result in the `flag` register.|0 if the arguments are equal, 1 if the first argument is the largest, 2 if the second argument is the largest.|
