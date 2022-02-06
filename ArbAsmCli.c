#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "arbnum.h"

unsigned int inputlen = 100;
int bigEndian = 0;
bool autoLengthen = false;
char* userInput;
num_t regs[7];
enum registers {gr1, gr2, gr3, inplen, enia, autl};
enum instructs {endprog=1, set, inc, add, mult, print};

const int TheMaximumLengthOfTheThings = 10;
char instructarray[][10] = { "\\\0", "set\0", "inc\0", "add\0", "mult\0", "print\0", "\0end" };
char registerarray[][10] = { "gr1\0", "gr2\0", "gr3\0", "inplen\0", "enia\0", "autl\0", "\0end" };

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
	num_t dummy;
	initnum(&dummy, 1, 0);
	switch (instruction){
		case set:
			copynum(args[0], args[1]);
			break;
		case inc:
			incnum(args[0]);
			break;
		case add:
			addnum(&dummy, args[0], args[1]);
			copynum(args[0], &dummy);
			break;
		case mult:
			multnum(&dummy, args[0], args[1]);
			copynum(args[0], &dummy);
			break;
		case print:
			break;
	}
	printnum(args[0], bigEndian);
	free(dummy.nump);
}

bool dothing(){
	//printf("start dothing\n");
	bool returnbool = true;
	int startat;
	int instruction = strlook(userInput, instructarray, 0, &startat);

	switch (instruction){
		case 0:
			printf("Not a valid instruction.\n");
			goto end;
			break;
		case 1://end cli
			printf("Good day!\n");
			returnbool = false;
			goto end;
			break;
	}
	//printf("after instructionfiguring\n");
	num_t* tmpptr[2];
	num_t tmp[2];
	initnum(&tmp[0], 1, 0); initnum(&tmp[1], 1, 0);
	tmpptr[0] = &tmp[0]; tmpptr[1] = &tmp[1];
	
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
			int id = strlook(userInput, registerarray, i, &loInputentry);
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
	free(tmp[0].nump); free(tmp[1].nump);
	end:
	return returnbool;
}

void updateessentials(){
	inputlen = numtoint(&regs[inplen]);
	bigEndian = numtoint(&regs[enia]);
}

void setessentialsready(){
	inttonum(&regs[inplen], inputlen);
	inttonum(&regs[enia], bigEndian);
}

void flushuserInput(){
	for(unsigned int i = 0; i < inputlen; i++)
		userInput[i] = '\0';
}

int main(){
	initnum(&regs[gr1], 32, 0);
	initnum(&regs[gr2], 32, 0);
	initnum(&regs[gr3], 32, 0);
	initnum(&regs[inplen], 16, 0);
	initnum(&regs[enia], 1, 0);
	setessentialsready();
	
	bool running = true;
	while(running){
		userInput = (char*) malloc((TheMaximumLengthOfTheThings + inputlen) * sizeof(char));
		flushuserInput();
		printf("\\\\\\ ");
		fgets(userInput, inputlen, stdin);
		sscanf(userInput, "%s", userInput);

		running = dothing();
		updateessentials();

		free(userInput);
	}
	free(regs[gr1].nump); free(regs[gr2].nump); free(regs[gr3].nump);
	return 0;
}
