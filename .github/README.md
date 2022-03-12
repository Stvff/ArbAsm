# Arbitrary Assembly
Interpreter and command line interface for an assembly language-styled scripting language, mainly featuring arbitrary digit integer arithmetic.

The `scripts/` folder has some examples of what it's like to use this language.

Very work in progress.

## Table of contents
* [Compilation (feat. GNU make)](#compilation-feat-gnu-make)
* [Running the program](#running-the-program)
* [Syntax, instructions, options](#syntax-registers-instructions)
  * [Numbers](#numbers)
  * [Strings](#strings)
  * [Registers](#registers)
  * [Instruction mnemonics](#instruction-mnemonics)
    * [Intra-number](#intra-number)
    * [Arithmetic](#arithmetic)
    * [IO](#io)
    * [Logic and Conditional statements](#logic-and-conditional-statements)
    * [Stack control](#stack-control)
  * [Scripts and files](#scripts-and-files)

## Compilation (feat. GNU make)
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
To compile for windows, enter
```
$ make forwindows
```

## Running the program
After compilation, the program can be run like so:
```
$ ./aasm
```
It might need execute permission, in which case:
```
$ sudo chmod +x ./aasm
```
should do the trick.\
For ease of reading, the rest of this document assumes the program is either in the path variable, or aliased in some way.

`aasm` can take arguments from the shell:
```
$ aasm <statement (without spaces)> <options>
```
Good replacements for spaces are dots (`.`) and underscores (`_`).
|Option|Description|
|------|-----------|
|`-e`  |Exit immediately after executing the statement that was passed as argument.|
|`-u <statefile>`|Load the designated statefile, and save to it again on exit.|
|`-b`  |Sets the notation to big endian before doing anything else.|
|`-h`  |Prints this table.|


## Syntax, registers and instructions
Every statement is of the form:
```
<instruction mnemonic(s)> <arguments seperated by commas>
```
Arguments are either registers or numbers.\
In the command line interface, when an instruction is executed, the number it returns is printed to the screen.\
The notation of both inputs and outputs is little-endian by default, opposite of what is standard in english. Should one desire it any different, endianness can be changed (see the `set` mnemonic and `endian` register).

### Numbers
Numbers can be of any length, and must be a set of decimal digits that may be interrupted by spaces. To input the sign of a number, a minus sign (`-`) can be placed at the end. The default is positive.\
Some examples:\
`314159265358979`,\
`310771-`, `00003 -`,\
`000 000 1`\
Once again, the notation is little-endian by default, so `00003` is thirty thousand. The place of the sign does not change in the different endiannesses, so in big endian notation mode, `-177013` will not be recognized and should be `177013-`.\
The `+` character is for quaternions, but those are currently work in process.

### Strings
A string is internally the same as a number, so it is possible to save them to registers and such. However, instead of 0-9, every digit is ASCII encoded, so arithmetic operations preform what C devs would call 'Undefined Behaviour'.\
Inputting a string is done by starting with a double quote character (`"`), then typing the desired string, and closing it again with a double quote character.\
These are the supported escaped characters:\
`\0`, `\r`, `\n`, `\\`\
Some examples of strings:\
`"Hello world!\n"`,\
`"I am using a DOS system...\r\n"`,\
`"I am a null-terminated string.\0"`

### Registers
Registers are effectively built-in variables.
|Register|Length  |Can be changed (with effect)|Description|
|--------|--------|----------------------------|-----------|
|`gr1`, `gr2`, `gr3`, `gr4`|variable|yes|The general purpose registers, intended for storing and modifying numbers.|
|`ir`    |variable|yes|The index register, intended for keeping track of continuously incremented (or decremented) numbers in loops.|
|`inplen`|variable|yes|Holds the maximum length of user input.|
|`endian`|1       |yes|Holds the endianness, 0 for little endian, 1 for big endian.|
|`stacsz`|variable|yes|Holds the size of the both the main and return stack, note that both stacks will be cleared if this register is re-set.|
|`mstptr`|variable|no |Holds the main stack pointer. As the main stack grows, this number goes to zero.|
|`rstptr`|variable|no |Holds the return stack pointer. As the return stack grows, this number goes to zero.|
|`time`  |variable|no |Contains the amount of seconds the previous statement took to execute.|
|`flag`  |1       |yes-(ish)|It contains the result of the comparison instructions `cmp` and `scmp`.|
|`loop`  |1       |yes|Holds either a `1` or `0`. Scripts loop if the register is `1` at the end of the script.|

### Instruction mnemonics
The first thing in a statement is always the instruction mnemonic (first column). The result of an operation (fourth column) is stored in the first argument (if it is a register), unless stated otherwise.\
The second column gives the syntax with expected 'types' for its corresponding instruction mnemonic. The types are:\
`n`: Number\
`s`: String\
`r`: Register

If one of the arguments is expected to be a number or string, a register can always be substituted, unlike if an argument is expected to be a register, in which case replacing it with a number or string results in a no-op, or Undefined Behaviour.

#### Intra-number
|Mnemonic|Syntax               |Name and description|Returns|
|--------|---------------------|--------------------|-------|
|`set`   |`<r>, <n/s>`         |Set. Sets the first argument as the second argument|The second argument|
|`dget`  |`<n/s>, <n/s>`       |Digit get. Gets, from the first argument, the digit at the index given by the second argument.|The selected digit|
|`dset`  |`<r>, <n/s>, <n/s>`  |Digit set. Sets, in the first argument, the digit at the index given by the second argument to the first digit of the third argument.|The first argument with one changed digit|
|`len`   |`<n/s>, <r>`         |Length. Gets the length of the first argument and stores the it the second argument|The length of the first argument|
|`rot`   |`<n/s>, <n/s>`       |Rotate. Rotates the first argument by the second argument.|The rotated argument|
|`shf`   |`<n/s>, <n/s>`       |Shift. Arithmetically shifts (base 10) the first argument by the second argument.|The shifted argument|
|`rev`   |`<n/s>`              |Reverse. Reverses the first argument.|The reversed argument|
|`sel`   |`<n/s>, <n/s>, <n/s>`|Selection. Selects a region of the first argument, starting at the index given by the second argument and ending at the index given by the third argument.|The selected region|
|`app`   |`<n/s>, <n/s>`       |Append. Appends the second argument to the end (so at the side of the most significant digits) of the first argument.|The second argument appended to the first|
|        |`*******************`|||

#### Arithmetic
|Mnemonic|Syntax         |Name and description|Returns|
|--------|---------------|--------------------|-------|
|`inc`   |`<n>`          |Increment. Increments the first argument by 1.|The incremented argument|
|`dec`   |`<n>`          |Decrement. Decrements the first argument by 1.|The decremented argument|
|`add`   |`<n>, <n>`     |Addition. Adds the two arguments together.|The sum of the arguments|
|`sub`   |`<n>, <n>`     |Subtraction. Subtracts the second argument from the first argument.|The first argument minus the second argument|
|`mul`   |`<n>, <n>`     |Multiplication. Multiplies the two arguments together.|The product of the two arguments|
|`div`   |`<n>, <n>, <r>`|Division. Divides (integer division) the first argument by the second argument, storing the remainder in the third argument|The first argument divided by the second argument|
|`mod`   |`<n>, <n>, <r>`|Modulo. Preforms a modulo operation where the second argument is the modulus, storing the division of the first argument by the second argument in the third argument|The remainder of the first argument divided by the second argument|
|        |`*************`|||

#### IO
|Mnemonic|Syntax              |Name and description|Returns|
|--------|--------------------|--------------------|-------|
|`print` |`<n/s>`             |Print. Prints the first argument to the console, starting a new line afterwards. It attempts to correctly guess the type and prints it accordingly.|The first argument|
|`nprint`|`<n/s>`             |Number print. Prints the first argument as a number, not starting a new line afterwards.|The first argument|
|`sprint`|`<s>`               |String print. Prints the first argument as string, not automatically starting a new line afterwards.|The first argument|
|`input` |`<r>`               |Input. Pauses the program to read user input, as number or string.|The number inputted by the user|
|`sinput`|`<r>`               |String input. Pauses the program to read user input, as direct keyboard inputs.|The user input, as string|
|`fread` |`<r>, <s>, <n>`     |File read. Reads a section from the file of which the path is given by the second argument, starting at the position given by the third argument. The length of the section is given by the first argument.|The selected section from the file, as string|
|`fwrite`|`<r>, <s>, <n>, <n>`|File write. Writes the first argument as ASCII string to the file of which the path is given by the second argument, starting at the position given by the third argument. If the third argument is 1, it first truncates the designated file.|The first argument, as string|
|        |`******************`|||

#### Logic and Conditional statements
|Mnemonic|Syntax       |Name and description|Returns|
|--------|-------------|--------------------|-------|
|`cmp`   |`<n>, <n>`   |Signed compare. Compares the two arguments. Stores the result in the `flag` register.|`0` if the arguments are equal, `1` if the first argument is the largest, `2` if the second argument is the largest.|
|`ucmp`  |`<n>, <n>`   |Unsigned compare. Compares the two arguments as if they were unsigned. Stores the result in the `flag` register.|`0` if the arguments are equal, `1` if the first argument is the largest, `2` if the second argument is the largest.|
|`Ce`    |`<statement>`|Condition equal. Preforms the statement that follows it when the `flag` register is `0`.|Nothing or the result of the statement|
|`Cg`    |`<statement>`|Condition greater. Preforms the statement that follows it when the `flag` register is `1`.|Nothing or the result of the statement|
|`Cs`    |`<statement>`|Condition smaller. Preforms the statement that follows it when the `flag` register is `2`.|Nothing or the result of the statement|
|        |`***********`|||

#### Stack control
There are two stacks: The main stack and the return stack. The main stack is meant to be used for temporarily relieving registers so that they can be used for different operations. The return stack is meant to be used to store numbers that will be returned at the end of the script or procedure it is being used in.
|Mnemonic|Syntax |Name and description|Returns|
|--------|-------|--------------------|-------|
|`push`  |`<n/s>`|Push. Pushes the first argument onto the main stack.|The first argument|
|`pop`   |`<n/s>`|Pop. Pops the number on top of the main stack into the first argument.|The number that was popped|
|`peek`  |`<r>`  |Peek. Copies the number on top of the main stack into the first argument|The number on top of the main stack|
|`flip`  |       |Flip. Pops the number on top of the main stack and pushes it onto the return stack.|The number that was popped and pushed|
|`ret`   |       |Return. Pops the number on top of the return stack and pushes (returns) it onto the main stack.|The number that was popped and pushed|

### Scripts and statefiles
A script is a simple text file with the `.aa` (arbitrary assembly) extension, containing multiple instructions on seperate lines. Scripts can be executed with `SCR`:
```
SCR <path relative to the current working directory>
```
The interpreter will execute the script line by line, where now numbers only get printed if the `print` mnemonic is used. The script loops depending on the `loop` register. If the register is `0`, the script will not loop, and return to the normal command prompt. If `loop` is `1`, the script will loop until `loop` is `0` again.\
A script runs in the instance it is called in.\
It is possible to run a script from a script, but the maximum 'recursion' depth is currently set at 10, so that means:\
A script in a script in a script in a script in a script in a script in a script in a script in a script in a script.

It is possible to save the state of the registers and stacks to a file, to later retrieve it again, or for debugging purposes. As convention, statefiles get the extension `.aal`.\
Saving:
```
SAVE <path relative to the current working directory>
```
Loading:
```
LOAD <path relative to the current working directory>
```
These also work in scripts.
