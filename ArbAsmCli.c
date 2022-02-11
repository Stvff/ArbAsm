#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "arbnum.h"

unsigned int inputlen = 100;
char* userInput;

int bigEndian = 0;

int stackSize = 20;
num_t* stack;
int stackptr;

num_t regs[8];
enum registers {gr1, gr2, gr3, flag, inplen, endia, stacsz, tme};
enum instructs {endprog=1, h, set, inc, add, mul, cmp, scmp, print, push, pop, len};

const int TheMaximumLengthOfTheThings = 10;
char instructstring[][10] = { "\\\0", "h\0", "set\0", "inc\0", "add\0", "mul\0", "cmp\0", "scmp\0", "print\0", "push\0", "pop\0", "len\0", "\0end" };
char registerstring[][10] = { "gr1\0", "gr2\0", "gr3\0", "flag\0", "inplen\0", "endia\0", "stacsz\0", "time\0", "\0end" };

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
	num_t dummy;
	initnum(&dummy, 15, 0);
	switch (instruction){
		case set:
			copynum(args[0], args[1], 0);
			copynum(&dummy, args[0], 0);
			break;
		case inc:
			incnum(args[0]);
			copynum(&dummy, args[0], 0);
			break;
		case add:
			addnum(args[0], args[0], args[1]);
			copynum(&dummy, args[0], 0);
			break;
		case mul:
			multnum(args[0], args[0], args[1]);
			copynum(&dummy, args[0], 0);
			break;
		case cmp:
			inttonum(&regs[flag], cmpnum(args[0], args[1], false));
			copynum(&dummy, &regs[flag], 0);
			break;
		case scmp:
			inttonum(&regs[flag], cmpnum(args[0], args[1], true));
			copynum(&dummy, &regs[flag], 0);
			break;
		case print:
			copynum(&dummy, args[0], 0);
			break;
		case push:
			stackptr--;
			if(stackptr < 0){
				printf("The bottom of the stack has been reached (it currently contains %d items)\nIt is possible to change the size of the stack by modifying the 'stacsz' register, but this will clear the current stack.\n", stackSize);
				stackptr = 0;
				break;
			}
			initnum(&stack[stackptr], 1, 0);
			copynum(&stack[stackptr], args[0], 0);
			copynum(&dummy, args[0], 0);
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
			copynum(&dummy, args[0], 0);
			break;
		case len:
			inttonum(&dummy, args[0]->len);
			copynum(args[1], &dummy, 0);
			break;
	}

	inttonum(&regs[tme], (int)((double)(clock() - begin_time)/CLOCKS_PER_SEC));
	printnum(&dummy, bigEndian);
	free(dummy.nump);
}

bool dothing(){
	//printf("start dothing\n");
	bool returnbool = true;
	int startat;
	int instruction = strlook(userInput, instructstring, 0, &startat);

	switch (instruction){
		case 0:
			printf("Not a valid instruction.\n");
			goto end;
			break;
		case endprog:
			printf("Good day!\n");
			returnbool = false;
			goto end;
			break;
		case h:
			printf("To preform an operation, type an instruction (e.g. 'set', 'print', 'add', 'push') and add the appropriate amount of arguments seperated by commas, finishing the line with a semicolon (;).\nThe general purpose registers are gr1, gr2, and gr3.\nTo change notation from little endian (the default) to big endian, set the register 'endia' to 1. To change maximum line length, set the register 'inplen' to the desired value.\nEnter '\\' to close the program.\n");
			goto end;
			break;
	}
	//printf("after instructionfiguring\n");
	num_t* tmpptr[3];
	num_t tmp[3];
	initnum(&tmp[0], 1, 0); initnum(&tmp[1], 1, 0); initnum(&tmp[2], 1, 0);
	tmpptr[0] = &tmp[0]; tmpptr[1] = &tmp[1]; tmpptr[2] = &tmp[2];
	
	int or12 = 0;
	int loInputentry = 0;
	char entry;
	bool therewasnosemi = true;
	for(unsigned int i = startat; i < inputlen; i+=loInputentry){
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
				goto endsafe;
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
		printf("There is no semicolon on the end of the line.\nIf you did have a semicolon and still got this error, that means the statement is too long (maximum is %u characters).\nIt is possible to change the maximum by modifying the inplen register.\n", inputlen);
		goto endsafe;
	}
	//printf("after loop\n");

	functionswitch(instruction, tmpptr);
	//printnum(tmpptr[0]);

	endsafe:
	free(tmp[0].nump); free(tmp[1].nump); free(tmp[2].nump);
	end:
	return returnbool;
}

void updateessentials(){
	inputlen = numtoint(&regs[inplen]);
	bigEndian = numtoint(&regs[endia]);
}

void setessentialsready(){
	inttonum(&regs[inplen], inputlen);
	inttonum(&regs[endia], bigEndian);
	inttonum(&regs[stacsz], stackSize);
}

void freestack(){
	for(int i = stackptr; i < stackSize; i++)
		free(stack[i].nump);
	free(stack);
}

void flushuserInput(){
	for(unsigned int i = 0; i < inputlen; i++)
		userInput[i] = '\0';
}

int main(){
	printf("Good to see you!\nEnter 'h' for quick tips and '\\' to close the program.\n");
	initnum(&regs[gr1], 32, 0);
	initnum(&regs[gr2], 32, 0);
	initnum(&regs[gr3], 32, 0);
	initnum(&regs[flag], 1, 0);
	initnum(&regs[inplen], 15, 0);
	initnum(&regs[endia], 1, 0);
	initnum(&regs[stacsz], 15, 0);
	initnum(&regs[tme], 15, 0);
	setessentialsready();
	stackptr = stackSize;
	stack = (num_t*) malloc(stackSize * sizeof(num_t));
	
	bool running = true;
	while(running){
		if(stackSize != numtoint(&regs[stacsz])){
			freestack();
			stackSize = numtoint(&regs[stacsz]);
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
	}

	freestack();
	free(regs[gr1].nump); free(regs[gr2].nump); free(regs[gr3].nump); free(regs[flag].nump);
	free(regs[inplen].nump); free(regs[endia].nump); free(regs[stacsz].nump);
	free(regs[tme].nump);
	return 0;
}
