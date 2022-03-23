# Arbitrary Assembly
Interpreter and command line interface for an assembly language-styled scripting language, mainly featuring arbitrary digit integer arithmetic.

The `scripts/` folder has some examples of what it's like to use this language.

Very work in progress.

## Table of contents
* [Compilation (feat. GNU make)](#compilation-feat-gnu-make)
* [Running the program](#running-the-program)
* [Scripting and the CLI](#scripting-and-the-cli)
* [Syntax, registers and instructions](#syntax-registers-and-instructions)

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
$ aasm <script> <options>
```
|Option          |Description|
|----------------|-----------|
|`-c`            |Enter command line after the script has stopped running.|
|`-i <statement>`|Executes the designated statement. This statement can not contain spaces.|
|`-e`            |Exit immediately after executing the statement that was passed as argument of -i.|
|`-l <statefile>`|Loads the designated statefile before interpreting any statements.|
|`-b`            |Sets the notation to big endian before interpreting any statements.|
|`-v`            |Displays the name, version and libraries.|
|`-h`            |Prints this table.|
|`--------------`||

Regarding `-i`, should one need them, good replacements for spaces are dots (`.`), underscores (`_`) or colons (`:`).

## Scripting and the CLI
A script is a simple text file with the `.aa` (arbitrary assembly) extension. One line can not contain more than one instruction. comments start at a semicolon (';') and end at the end of the line. The interpreter executes the script line by line, looping at the end depending on the `loop` register. If the register is `0`, the script will not loop, and exit the script. If `loop` is `1`, the script will loop until `loop` is `0` again.\
A script runs in the instance it is called in.\
Running a script can be run from the shell with `aasm`, and in the arbasm CLI with (the standard library instruction)\
`SCR <string or register containing a string>`,\
which runs the script the path of which is given by the string.\
It is possible to run a script from a script, but the maximum 'recursion' depth is currently set at 16.\

If `aasm` is run without any arguments, or with the appropriate options, the program enters the command line interface. Here it will prompt the user with three backslashes (`\\\ `), and statements can be entered. After a statement has been executed, the result it returns is printed to the console.

## Syntax, registers and instructions
Every statement is of the form:
```
<instruction mnemonic(s)> <arguments seperated by commas>
```
Elaborate documentation lives on the [wiki](../../../wiki)
