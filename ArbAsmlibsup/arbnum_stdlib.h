#ifndef ARBNUM_STDLIB
#define ARBNUM_STDLIB
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "defglobal.h"
#include "../arbnum.h"

enum CompiletimeConstantsStdlib {
	initialstackSize = 20,
	regAmount = 14,
};

int stackSize = initialstackSize;
num_t* stack;
int stackptr;
num_t* retstack;
int retstackptr;

num_t regs[regAmount];
enum registers {
	gr1, gr2, gr3, gr4, ir, flag,
	inplen, endian, stacsz, mstptr,
	rstptr, tme, ptme, loop,
	registerend
};

enum instructsstdlib {
	set, dset, dget, rev, sel, cton, ntoc,
	inc, dec, add, sub, mul, divi, modu,
	cmp, ucmp, rot, shf, app, len, 
	print, sprint, nprint, draw,
	firead, fiwrite, input, sinput,
	push, pop, peek, flip, ret,
	SCR, SAVE, LOAD, Ce, Cg, Cs,
	instructend
};

char registerstring[][maxKeywordLen] = { 
	"gr1", "gr2", "gr3", "gr4", "ir",
	"flag", "inplen", "endian", "stacsz", "mstptr", "rstptr",
	"time", "ptime", "loop",
	"\0end"
};

char instructstring[][maxKeywordLen] = {
	"set", "dset", "dget", "rev", "sel", "cton", "ntoc",
	"inc", "dec", "add", "sub", "mul", "div", "mod",
	"cmp", "ucmp", "rot", "shf", "app", "len",
	"print", "sprint", "nprint", "draw",
	"fread", "fwrite", "input", "sinput",
	"push", "pop", "peek", "flip", "ret", 
	"SCR", "SAVE", "LOAD", "Ce", "Cg", "Cs",
	"\0end"
};

void freestacks(){
	for(int i = stackptr; i < stackSize; i++)
		free(stack[i].nump);
	free(stack);
	for(int i = retstackptr; i < stackSize; i++)
		free(retstack[i].nump);
	free(retstack);
}

int initstd(GLOBAL* mainptrs){
	initnumarray(regAmount, regs, 7, 0, 0);

	stackptr = stackSize;
	stack = (num_t*) malloc(stackSize * sizeof(num_t));
	retstackptr = stackSize;
	retstack = (num_t*) malloc(stackSize * sizeof(num_t));

	inttonum(&regs[inplen], mainptrs->userInputLen);
	inttonum(&regs[endian], mainptrs->bigEndian);
	inttonum(&regs[loop], mainptrs->scriptLoops);

	inttonum(&regs[stacsz], stackSize);
	inttonum(&regs[mstptr], stackSize);
	inttonum(&regs[rstptr], stackSize);
	
	time_t thetime = time(&thetime);
	inttonum(&regs[tme], (int32_t) thetime);

	if(mainptrs->debug == 'v') printf("stdlib initted\n");
	return 0;
}

int updatestd(GLOBAL* mainptrs){
	mainptrs->userInputLen = numtoint(&regs[inplen], false);
	mainptrs->bigEndian = numtoint(&regs[endian], false);
	mainptrs->scriptLoops = numtoint(&regs[loop], false);

	if(stackSize != numtoint(&regs[stacsz], false)){
		freestacks();
		stackSize = numtoint(&regs[stacsz], false);
		stackptr = stackSize;
		retstackptr = stackSize;
		stack = (num_t*) malloc(stackSize * sizeof(num_t));
		retstack = (num_t*) malloc(stackSize * sizeof(num_t));
	}
	inttonum(&regs[mstptr], stackptr);
	inttonum(&regs[rstptr], retstackptr);

	time_t thetime = time(&thetime);
	inttonum(&regs[tme], (int32_t) thetime);

	if(mainptrs->debug == 'v') printf("stdlib updated\n");
	return 0;
}

int freestd(GLOBAL* mainptrs){
	freestacks();
	freenumarray(regAmount, regs);
	if(mainptrs->debug == 'v') printf("stdlib freed\n");
	return 0;
}

int instructhandlestd(GLOBAL* mainptrs){
	int instruction = strlook(mainptrs->userInput, instructstring, &mainptrs->readhead);
	switch(instruction){
		case Ce:
			if(regs[flag].nump[0] == 0) mainptrs->lookingMode = 'i';
			else mainptrs->lookingMode = 'd';
			break;
		case Cg:
			if(regs[flag].nump[0] == 1) mainptrs->lookingMode = 'i';
			else mainptrs->lookingMode = 'd';
			break;
		case Cs:
			if(regs[flag].nump[0] == 2) mainptrs->lookingMode = 'i';
			else mainptrs->lookingMode = 'd';
			break;
		default:
			mainptrs->lookingMode = 'a';
			break;
	}
	
	if(mainptrs->debug == 'v') printf("stdlib instructed\n");
	return instruction;
}

int argumenthandlestd(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("stdlib argumented\n");
	return 0;
}

#endif
