# Arbitrary Assembly
Command line interface and interpreter for an assembly language-styled scripting language with a strong focus on arithmetic for its main data type: arbitrary precision integers.

A long term, distant goal is making a standalone handheld calculator with the functions and conventions outlined by this implementation.

## Table of contents
* [Compilation](#compilation)
* [Syntax, instructions, options](#syntax-instructions-options)
  * [Numbers](#numbers)
  * [Registers](#registers)
  * [Instruction mnemonics](#instruction-mnemonics)
    * [Register control and Information](#register-control-and-information)
    * [Arithmetic](#arithmetic)
    * [Logic and Conditional statements](#logic-and-conditional-statements)
    * [Stack control](#stack-control)
  * [Scripts and files](#scripts-and-files)

## Compilation
To compile it, enter
```
$ make
```
in your favorite shell.\
To compile and run it, enter
```
$ make run
```
It uses gcc, but apperantly the gcc command is symlinked to clang sometimes. That being said, it has also been tested with clang, so it should be alright either way.\
There is a syntax highlighting file for the text editor [micro](https://github.com/zyedidia/micro) in the scripts directory. If `formicro` is added as a flag for `make`, it will place the syntax file in what should be the right place:
```
$ make formicro
```
After compilation, the program can be run like so:
```
$ ./aasm
```
It might need execute permission, in which case:
```
$ sudo chmod +x ./aasm
```
should do the trick.

## Syntax, instructions, options
Every statement is of the form:
```
<instruction mnemonic(s)> <arguments seperated by commas>
```
Arguments are either registers or numbers.\
In the command line interface, when an instruction is executed, the number it returns is printed to the screen.\
The notation of both inputs and outputs is little-endian by default, opposite of what is standard in english. Should one desire it any different, endianness can be changed (see the `set` mnemonic and `endian` register).

### Numbers
Numbers can be of any length, and must be a string of decimal digits that may be interrupted by spaces. To input the sign of a number, a `+` or `-` can be placed at the end. The default is positive.\
Some examples:\
`314159265358979`, `310771-`, `00003+`, `000 000 1`\
Once again, the notation is little-endian by default, so `00003` is thirty thousand. The place of the sign does not change in the different endiannesses, so in big endian notation mode, `-177013` will not be recognized and should be `177013-`.

### Registers
Registers are effectively built-in variables.
|Register|Length|Can be changed (with effect)|Description|
|--------|------|----------------------------|-----------|
|`gr1`, `gr2`, `gr3`, `gr4`|variable|yes|The general purpose registers, intended for storing and modifying numbers.|
|`ir`|variable|yes|The index register, intended for keeping track of continuously incremented (or decremented) numbers in loops.|
|`inplen`|variable|yes|Holds the maximum length of user input.|
|`endian`|1|yes|Holds the endianness, 0 for little endian, 1 for big endian.|
|`stacsz`|variable|yes|Holds the size of the both the main and return stack, note that both stacks will be cleared if this register is re-set.|
|`staptr`|variable|no|Holds the main stack pointer. As the main stack grows, this number goes to zero.|
|`rstptr`|variable|no|Holds the return stack pointer. As the return stack grows, this number goes to zero.|
|`time`|variable|no|Contains the amount of seconds the previous statement took to execute.|
|`flag`|1|yes-(ish)|It contains the result of the comparison instructions `cmp` and `scmp`.|
|`loop`|1|yes|Holds either a `1` or `0`. Scripts loop if the register is `1` at the end of the script.|

### Instruction mnemonics
The first thing in a statement is always the instruction mnemonic. The result of an operation is stored in the first argument (if it is a register), unless stated otherwise.

#### Register control and Information
|Mnemonic|Intended Syntax|Name and description|Returns|
|--------|---------------|--------------------|-------|
|`set`|`<register>, <register/number>`|Set. Sets the first argument as the second argument|The second argument.|
|`print`|`<register/number>`|Print. Prints the number of the first argument to the console. It is effectively a no-op in the cli.|The first argument|
|`len`|`<register/number>, <register>`|Length. Gets the length of the first argument and stores the it the second argument|The length of the first argument|
|`rot`|`<register/number>, <register/number>`|Rotate. Rotates the first argument by the second argument.|The rotated argument|
|`shf`|`<register/number>, <register/number>`|Shift. Arithmetically shifts the first argument by the second argument.|The shifted argument|
|`rev`|`<register/number>`|Reverse. Reverses the first argument.|The reversed argument|
|`sel`|`<register/number>, <register/number>, <register/number>`|Selection. Selects a region of the first argument, starting at the index given by the second argument and ending at the index given by the third argument.|The selected region|

#### Arithmetic
|Mnemonic|Intended Syntax|Name and description|Returns|
|--------|---------------|--------------------|-------|
|`inc`|`<register/number>`|Increment. Increments the first argument by 1.|The incremented argument|
|`dec`|`<register/number>`|Decrement. Decrements the first argument by 1.|The decremented argument|
|`add`|`<register/number>, <register/number>`|Addition. Adds the two arguments together.|The sum of the arguments|
|`sub`|`<register/number>, <register/number>`|Subtraction. Subtracts the second argument from the first argument.|The first argument minus the second argument|
|`mul`|`<register/number>, <register/number>`|Multiplication. Multiplies the two arguments together.|The product of the two arguments|
|`div`|`<register/number>, <register/number>, <register>`|Division. Divides (integer division) the first argument by the second argument, storing the remainder in the third argument|The first argument divided by the second argument|
|`mod`|`<register/number>, <register/number>, <register>`|Modulo. Preforms a modulo operation where the second argument is the modulus, storing the division of the first argument by the second argument in the third argument|The remainder of the first argument divided by the second argument|

#### Logic and Conditional statements
|Mnemonic|Intended Syntax|Name and description|Returns|
|--------|---------------|--------------------|-------|
|`cmp`|`<register/number>, <register/number>`|Signed compare. Compares the two arguments. Stores the result in the `flag` register.|`0` if the arguments are equal, `1` if the first argument is the largest, `2` if the second argument is the largest.|
|`ucmp`|`<register/number>, <register/number>`|Unsigned compare. Compares the two arguments as if they were unsigned. Stores the result in the `flag` register.|`0` if the arguments are equal, `1` if the first argument is the largest, `2` if the second argument is the largest.|
|`Ce`|`<statement>`|Condition equal. Preforms the statement that follows it when the `flag` register is `0`.|Nothing or the result of the statement|
|`Cg`|`<statement>`|Condition greater. Preforms the statement that follows it when the `flag` register is `1`.|Nothing or the result of the statement|
|`Cs`|`<statement>`|Condition smaller. Preforms the statement that follows it when the `flag` register is `2`.|Nothing or the result of the statement|

#### Stack control
There are two stacks: The main stack and the return stack. The main stack is meant to be used for temporarily relieving registers so that they can be used for different operations. The return stack is meant to be used to store numbers that will be returned at the end of the script or procedure it is being used in.
|Mnemonic|Intended Syntax|Name and description|Returns|
|--------|---------------|--------------------|-------|
|`push`|`<register/number>`|Push. Pushes the first argument onto the main stack.|The first argument|
|`pop`|`<register>`|Pop. Pops the number on top of the main stack into the first argument.|The number that was popped|
|`peek`|`<register>`|Peek. Copies the number on top of the main stack into the first argument|The number on top of the main stack|
|`flip`||Flip. Pops the number on top of the main stack and pushes it onto the return stack.|The number that was popped and pushed|
|`ret`||Return. Pops the number on top of the return stack and pushes (returns) it onto the main stack.|The number that was popped and pushed|

### Scripts and files
A script is a simple text file with the `.aa` (arbitrary assembly) extension, containing multiple instructions on seperate lines. Scripts can be executed with `SCR`:
```
\\\ SCR <path relative to the current working directory>
```
Which only works in the CLI (for now).\
The interpreter will execute the script line by line, where now numbers only get printed if the `print` mnemonic is used. The script loops depending on the `loop` register. If the register is `0`, the script will not loop, and return to the normal command prompt. If `loop` is `1`, the script will loop until `loop` is `0` again.\
A script runs in the instance it is called in, so this is a way to pass user inputs.
