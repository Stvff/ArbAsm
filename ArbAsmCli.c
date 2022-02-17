#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "arbnum.h"
unsigned int inputlen = 256;
char* userInput;

bool ScriptingMode = false;
int scriptLoops = 0;

int bigEndian = 0;

int stackSize = 20;
num_t* stack;
int stackptr;

num_t regs[10];//Change it in main() too if you change it here!
enum registers {gr1, gr2, gr3, ir, flag, inplen, endia, stacsz, tme, loop};
enum instructs {endprog=1, h, set, rev, sel, inc, dec, add, sub, mul, divi, modu, cmp, ucmp, rot, shf, print, push, pop, len, SCR, Ce, Cg, Cs};

const int TheMaximumLengthOfTheThings = 10;
char instructstring[][10] = { "\\\0", "h\0", "set\0", "rev\0", "sel\0",
							"inc\0", "dec\0", "add\0", "sub\0", "mul\0", "div\0", "mod\0",
							"cmp\0", "ucmp\0", "rot\0", "shf\0",
							"print\0", "push\0", "pop\0", "len\0", 
							"SCR\0", "Ce\0", "Cg\0", "Cs\0", "\0end"};
char registerstring[][10] = { "gr1\0", "gr2\0", "gr3\0", "ir\0",
							"flag\0", "inplen\0", "endia\0", "stacsz\0", "time\0", "loop\0", "\0end" };

int strlook(char string[], char source[][TheMaximumLengthOfTheThings], int offset, int* lengthoflocated){
	int i = 0;
	int j;
	bool isthis;
	while (source[i][0] != '\0'){
		isthis = true;
		j = 0;
		while (source[i][j] != '\0'){
			if(string[j+offset] != source[i][j]) isthis = false;
			j++;
		}
		*lengthoflocated = j;
		i++;
		if(isthis) return i;
	}
	*lengthoflocated = 1;
	return 0;
}

void functionswitch(int instruction, num_t* args[]){
	clock_t begin_time = clock();
	num_t* printptr; bool doprint = !ScriptingMode;
	num_t dummy;
	initnum(&dummy, 15, 0, 0);
	switch (instruction){
		case set:
			copynum(args[0], args[1], 0);
			printptr = args[0];
			break;
		case rev:
			reversenum(args[0]);
			printptr = args[0];
			break;
		case sel:
			selectsectionnum(&dummy, args[0], numtoint(args[1], false), numtoint(args[2], false));
			copynum(args[0], &dummy, 0);
			printptr = args[0];
			break;
		case inc:
			incnum(args[0], false);
			printptr = args[0];
			break;
		case dec:
			incnum(args[0], true);
			printptr = args[0];
			break;
		case add:
			sumnum(args[0], args[0], args[1], false);
			printptr = args[0];
			break;
		case sub:
			sumnum(args[0], args[0], args[1], true);
			printptr = args[0];
			break;
		case mul:
			multnum(args[0], args[0], args[1]);
			printptr = args[0];
			break;
		case divi:
			divnum(args[0], args[2], args[0], args[1]);
			printptr = args[0];
			break;
		case modu:
			divnum(args[2], args[0], args[0], args[1]);
			printptr = args[0];
			break;
		case cmp:
			inttonum(&regs[flag], cmpnum(args[0], args[1], true));
			printptr = &regs[flag];
			break;
		case ucmp:
			inttonum(&regs[flag], cmpnum(args[0], args[1], false));
			printptr = &regs[flag];
			break;
		case rot:
			rotnum(args[0], numtoint(args[1], true));
			printptr = args[0];
			break;
		case shf:
			shiftnum(args[0], numtoint(args[1], true));
			printptr = args[0];
			break;
		case print:
			printptr = args[0];
			doprint = true;
			break;
		case push:
			stackptr--;
			if(stackptr < 0){
				printf("The bottom of the stack has been reached (it currently contains %d items)\nIt is possible to change the size of the stack by modifying the `stacsz` register, but this will clear the current stack.\n", stackSize);
				stackptr = 0;
				break;
			}
			initnum(&stack[stackptr], 1, 0, 0);
			copynum(&stack[stackptr], args[0], 0);
			printptr = args[0];
			break;
		case pop:
			if(stackptr >= stackSize){
				printf("The top of the stack has been reached (there are no elements to be popped).\n");
				stackptr = stackSize;
				break;
			}
			copynum(args[0], &stack[stackptr], 0);
			free(stack[stackptr].nump);
			stackptr++;
			printptr = args[0];
			break;
		case len:
			inttonum(args[1], args[0]->len);
			printptr = args[1];
			break;
	}

	if(doprint) printnum(printptr, bigEndian);

	inttonum(&regs[tme], (int)((double)(clock() - begin_time)/CLOCKS_PER_SEC));
	free(dummy.nump);
}

bool dothing(){
	//printf("start dothing\n");
	bool returnbool = true;
	int startat;
	int offsetbegin = 0;
	startofdothing:;
	int instruction = strlook(userInput, instructstring, offsetbegin, &startat);

	switch (instruction){
		case 0:
			printf("Not a valid instruction.\n");
			goto endofdothing;
			break;
		case endprog:
			printf("Good day!\n");
			returnbool = false;
			goto endofdothing;
			break;
		case h:
			printf("To preform an operation, type an instruction mnemonic (e.g. `set`, `print`, `add`, `push`) and add the appropriate amount of arguments seperated by commas.\n\nThe general purpose registers are `gr1`, `gr2`, and `gr3`.\n\nTo change notation from little endian (the default) to big endian, set the register `endia` to 1. To change maximum line length, set the register `inplen` to the desired value.\n\nEnter `\\` to close the program.\n\n(P.S. Did you know that the actual plural of \"comma\" is \"commata\"? Wild.)\n");
			goto endofdothing;
			break;
		case Ce:
			offsetbegin = startat + 1;
			if(regs[flag].nump[0] == 0) goto startofdothing;
			else goto endofdothing;
			break;
		case Cg:
			offsetbegin = startat + 1;
			if(regs[flag].nump[0] == 1) goto startofdothing;
			else goto endofdothing;
			break;
		case Cs:
			offsetbegin = startat + 1;
			if(regs[flag].nump[0] == 2) goto startofdothing;
			else goto endofdothing;
			break;
		case SCR:
			printf("Running script.\n");
			ScriptingMode = true;
			returnbool = false;
			goto endofdothing;
			break;
	}
	//printf("after instructionfiguring\n");
	num_t* tmpptr[3];
	num_t tmp[3];
	initnumarray(3, tmp, 1, 0, 0);
	tmpptr[0] = &tmp[0]; tmpptr[1] = &tmp[1]; tmpptr[2] = &tmp[2];
	
	int or12 = 0;
	int loInputentry = 0;
	char entry;
	bool therewasnosemi = true;
	for(unsigned int i = startat + offsetbegin; i < inputlen; i+=loInputentry){
		//printf("start loop, i: %u\n", i);
		loInputentry = 1;
		entry = userInput[i];
		//printf("%c \n", entry);
		if(entry == ';'){
			therewasnosemi = false;
			break;
		}
		//printf("in the middle of a loop, i: %u\n", i);
		if(entry == ',') or12++;
		if( (entry >= 'a' && entry <= 'z') || (entry >= 'A' && entry <= 'Z') ){
			//printf("a to z\n");
			int id = strlook(userInput, registerstring, i, &loInputentry);
			if(id == 0){
				printf("Register at argument %d is not a register.\n", or12 + 1);
				goto enddothingsafe;
			} else tmpptr[or12] = &regs[id - 1];
			//loInputentry--;
		} else if (entry >= '0' && entry <= '9'){
			//printf("0 to 9\n");
			loInputentry = inpstrtonum(&tmp[or12], userInput, i, bigEndian);
			tmpptr[or12] = &tmp[or12];
		}
		//printf("end of loop, i: %u\n", i);
	}
	if(therewasnosemi){
		printf("The statement is too long (maximum is %u characters).\nIt is possible to change the maximum by modifying the `inplen` register.\n", inputlen);
		goto enddothingsafe;
	}
	//printf("after loop\n");

	functionswitch(instruction, tmpptr);
	//printnum(tmpptr[0]);

	enddothingsafe:
	freenumarray(3, tmp);
	endofdothing:
	return returnbool;
}

void updateessentials(){
	inputlen = numtoint(&regs[inplen], false);
	bigEndian = numtoint(&regs[endia], false);
	scriptLoops = numtoint(&regs[loop], false);
}

void setessentialsready(){
	inttonum(&regs[inplen], inputlen);
	inttonum(&regs[endia], bigEndian);
	inttonum(&regs[stacsz], stackSize);
	inttonum(&regs[loop], scriptLoops);
}

void freestack(){
	for(int i = stackptr; i < stackSize; i++)
		free(stack[i].nump);
	free(stack);
}

void flushuserInput(){
	for(unsigned int i = 0; i < inputlen; i++)
		userInput[i] = ';';
}

int main(){
	printf("Good to see you!\nEnter `h` for quick tips and `\\` to close the program.\n");
	initnumarray(10, regs, 21, 0, 0);
	setessentialsready();
	stackptr = stackSize;
	stack = (num_t*) malloc(stackSize * sizeof(num_t));
	
	bool running = true;
	normalop:
	do {
		if(stackSize != numtoint(&regs[stacsz], false)){
			freestack();
			stackSize = numtoint(&regs[stacsz], false);
			stackptr = stackSize;
			stack = (num_t*) malloc(stackSize * sizeof(num_t));
		}

		userInput = (char*) malloc((TheMaximumLengthOfTheThings + inputlen) * sizeof(char));
		flushuserInput();
		printf("\\\\\\ ");
		fgets(userInput, inputlen, stdin);
		sscanf(userInput, "%s", userInput);

		running = dothing();
		updateessentials();

		free(userInput);
	} while(running);

	if(ScriptingMode){
		FILE *fp = fopen("./script.aa", "r");
		if(fp == NULL){
			printf("Could not open designated file.\n");
			ScriptingMode = false;
			running = true;
			goto normalop;
		}

		int readheadpos = 0;
		do {
			if(stackSize != numtoint(&regs[stacsz], false)){
				freestack();
				stackSize = numtoint(&regs[stacsz], false);
				stackptr = stackSize;
				stack = (num_t*) malloc(stackSize * sizeof(num_t));
			}

			userInput = (char*) malloc((TheMaximumLengthOfTheThings + inputlen) * sizeof(char));
			flushuserInput();

			if(fgetc(fp) != EOF && fgetc(fp) != '\n'){
				fseek(fp, readheadpos, SEEK_SET);

				fgets(userInput, inputlen, fp);
				//sscanf(userInput, "%s", userInput);
				//printf("%s\n", userInput);

				if(userInput[0] != '\n'){
					ScriptingMode = dothing();
					updateessentials();
				}
				for(int i = 0; userInput[i] != '\0'; i++)
					readheadpos++;

			} else if(scriptLoops != 0){
				readheadpos = 0;
				fseek(fp, readheadpos, SEEK_SET);
			} else {
				printf("Script completed.\n");
				ScriptingMode = false;
			}
			
			free(userInput);
		} while(ScriptingMode);
		
		fclose(fp);
		goto normalop;
	}

	freestack();
	freenumarray(10, regs);
	return 0;
}
