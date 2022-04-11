#ifndef AASM_STDLIB
#define AASM_STDLIB 1
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "aasm_global.h"
#include "arbnum.h"

//############################################### <Std globals>
enum CompiletimeConstantsStdlib {
	initialstackSize = 64
};

int stackSize = initialstackSize;
num_t* stack;
int stackptr;
num_t* retstack;
int retstackptr;

int64_t decimalpoint;

num_t* args[maxArgumentAmount];
num_t tmps[maxArgumentAmount];
enum typesstd { UnDef, Number, String };

enum registers {
	gr1, gr2, gr3, gr4, ans, ir, flag,
	inplen, endian, stacsz, mstptr,
	rstptr, tme, ptme, loop, decip,
	regAmount
};
num_t regs[regAmount];

enum instructsstdlib {
	set, dset, dget, rev, sel, cut, ins,
	cton, ntoc,
	inc, dec, add, sub, mul, divi, modu,
	rnd, root,
	cmp, ucmp, rot, shf, app, len, trun,
	print, sprint, nprint,
	firead, fiwrite, input, sinput,
	push, pop, peek, flip, ret,
	run, SAVE, LOAD, Ce, Cg, Cs
};

char registerstring[][maxKeywordLen] = { 
	"gr1", "gr2", "gr3", "gr4", "ans", "ir",
	"flag", "inplen", "endian", "stacsz", "mstptr", "rstptr",
	"time", "ptime", "loop", "decip",
	"\0end"
};

char instructstring[][maxKeywordLen] = {
	"set", "dset", "dget", "rev", "sel", "cut", "ins",
	"cton", "ntoc",
	"inc", "dec", "add", "sub", "mul", "div", "mod",
	"rand", "root",
	"cmp", "ucmp", "rot", "shf", "app", "len", "trun",
	"print", "sprint", "nprint",
	"fread", "fwrite", "input", "sinput",
	"push", "pop", "peek", "flip", "ret", 
	"run", "SAVE", "LOAD", "Ce", "Cg", "Cs",
	"\0end"
};
//############################################### </Std globals>

//############################################### <Std internal functions>
void freestacks(){
	for(int i = stackptr; i < stackSize; i++)
		free(stack[i].nump);
	free(stack);
	for(int i = retstackptr; i < stackSize; i++)
		free(retstack[i].nump);
	free(retstack);
}

bool pushtomst(num_t* num){
	stackptr--;
	if(stackptr < 0){
		printf("The bottom of the stack has been reached (it currently contains %d items)\n", stackSize);
		printf("It is possible to change the size of the stack by modifying the `stacsz` register, but this will clear the current stack.\n");
		stackptr = 0;
		return false;
	}
	initnum(&stack[stackptr], 1, 0, 0);
	copynum(&stack[stackptr], num, 0);
	return true;
}
bool popfrommst(num_t* num){
	if(stackptr >= stackSize){
		printf("The top of the stack has been reached (there are no elements to be popped).\n");
		stackptr = stackSize;
		return false;
	}
	copynum(num, &stack[stackptr], 0);
	free(stack[stackptr].nump);
	stackptr++;
	return true;
}

bool fliperonie(){
	if(stackptr >= stackSize){
		printf("The top of the main stack has been reached (there are no elements to be flipped).\n");
		stackptr = stackSize;
		return false;
	}
	retstackptr--;
	if(retstackptr < 0){
		printf("The bottom of the return stack has been reached (it currently contains %d items)\n", stackSize);
		printf("It is possible to change the size of both stacks by modifying the `stacsz` register, but this will clear the current state of both stacks.\n");
		retstackptr = 0;
		return false;
	}
	initnum(&retstack[retstackptr], 1, 0, 0);
	copynum(&retstack[retstackptr], &stack[stackptr], 0);
	free(stack[stackptr].nump);
	stackptr++;
	return true;
}
bool reteronie(){
	if(retstackptr >= stackSize){
		printf("The top of the return stack has been reached (there are no elements to be returned).\n");
		retstackptr = stackSize;
		return false;
	}
	stackptr--;
	if(stackptr < 0){
		printf("The bottom of the main stack has been reached (it currently contains %d items)\n", stackSize);
		printf("It is possible to change the size of both stacks by modifying the `stacsz` register, but this will clear the current state of both stacks.\n");
		stackptr = 0;
		return false;
	}
	initnum(&stack[stackptr], 1, 0, 0);
	copynum(&stack[stackptr], &retstack[retstackptr], 0);
	free(retstack[retstackptr].nump);
	retstackptr++;
	return true;
}

int saveload(char path[], char saveorload, GLOBAL* mainptrs){
	FILE* fp;
	if(saveorload == 's'){
		fp = fopen(path, "wb+");
		if(fp == NULL){
			printf("Could not open file to save to: '%s'.\n", path);
			return 1;
		}
		for(int i = 0; i < regAmount; i++)
			savenum(fp, &regs[i]);
		for(int i = stackSize-1; i >= stackptr; i--)
			savenum(fp, &stack[i]);
		for(int i = stackSize-1; i >= retstackptr; i--)
			savenum(fp, &retstack[i]);

		if(mainptrs->inputMode == 'i') printf("Saved state to '%s'.\n", path);
	} else {
		fp = fopen(path, "rb");
		if(fp == NULL){
			printf("Could not open file to load from: '%s'.\n", path);
			return 1;
		}
		for(int i = 0; i < regAmount; i++)
			loadnum(fp, &regs[i]);

		freestacks();
		stackSize = numtoint(&regs[stacsz], false);
		stack = (num_t*) malloc(stackSize * sizeof(num_t));
		retstack = (num_t*) malloc(stackSize * sizeof(num_t));
		stackptr = numtoint(&regs[mstptr], false);
		retstackptr = numtoint(&regs[rstptr], false);

		for(int i = stackSize-1; i >= stackptr; i--){
			initnum(&stack[i], 1, 0, 0);
			loadnum(fp, &stack[i]);}
		for(int i = stackSize-1; i >= retstackptr; i--){
			initnum(&retstack[i], 1, 0, 0);
			loadnum(fp, &retstack[i]);}

		if(mainptrs->inputMode == 'i') printf("Loaded state from '%s'.\n", path);
	}
	fclose(fp);
	return 0;
}
//############################################### </Std internal functions>

//############################################### <Std essentials>
int init_std(GLOBAL* mainptrs){
	initnumarray(regAmount, regs, 7, 0, 0);

	decimalpoint = 0;

	stackptr = stackSize;
	stack = (num_t*) malloc(stackSize * sizeof(num_t));
	retstackptr = stackSize;
	retstack = (num_t*) malloc(stackSize * sizeof(num_t));

	inttonum(&regs[decip], decimalpoint);

	inttonum(&regs[inplen], mainptrs->userInputLen);
	inttonum(&regs[endian], mainptrs->bigEndian);
	inttonum(&regs[loop], mainptrs->scriptLoops);

	inttonum(&regs[stacsz], stackSize);
	inttonum(&regs[mstptr], stackSize);
	inttonum(&regs[rstptr], stackSize);

	time_t thetime = time(&thetime);
	inttonum(&regs[tme], (int64_t) thetime);

	for(int i = 0; i < maxArgumentAmount; i++){
		args[i] = &tmps[i];
		initnum(args[i], 1, 0, 0);
		tmps[i].nump[0] = 0;
	}

	if(mainptrs->debug == 'v') printf("stdlib initted\n");
	return 0;
}

int update_std(GLOBAL* mainptrs){
	mainptrs->userInputLen = numtoint(&regs[inplen], false);

	decimalpoint = numtoint(&regs[decip], true);

	if(regs[endian].nump[0] != 1 || regs[endian].nump[0] == 0) mainptrs->bigEndian = 0;
	else mainptrs->bigEndian = 1;
	inttonum(&regs[endian], mainptrs->bigEndian);

	if(regs[loop].nump[0] != 1 || regs[loop].nump[0] == 0) mainptrs->scriptLoops = 0;
	else mainptrs->scriptLoops = 1;
	inttonum(&regs[loop], mainptrs->scriptLoops);

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

	for(int i = 0; i < maxArgumentAmount; i++){
		free(tmps[i].nump);
		args[i] = &tmps[i];
		initnum(args[i], 1, 0, 0);
		tmps[i].nump[0] = 0;
	}

	time_t thetime = time(&thetime);
	inttonum(&regs[tme], (int64_t) thetime);

	if(mainptrs->debug == 'v') printf("stdlib updated\n");
	return 0;
}

int free_std(GLOBAL* mainptrs){
	for(int i = 0; i < maxArgumentAmount; i++)
		free(tmps[i].nump);

	freestacks();
	freenumarray(regAmount, regs);
	if(mainptrs->debug == 'v') printf("stdlib freed\n");
	return 0;
}

int instructhandler_std(GLOBAL* mainptrs){
	mainptrs->instructNr = strlook(mainptrs->userInput, instructstring, &mainptrs->readhead);
	switch(mainptrs->instructNr){
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
	return 0;
}

int argumenthandler_std(GLOBAL* mainptrs){
	char entry = mainptrs->userInput[mainptrs->readhead];
	int registerNr = -1;

	if( (entry >= 'a' && entry <= 'z') || (entry >= 'A' && entry <= 'Z') ){
		registerNr = strlook(mainptrs->userInput, registerstring, &mainptrs->readhead);
		if(registerNr == -1){
			printf("Register at argument %d on line %d is not a register.\n", mainptrs->argumentNr + 1, mainptrs->flist[mainptrs->fileNr].lineNr);
			mainptrs->lookingMode = 'd';
		} else {
			args[mainptrs->argumentNr] = &regs[registerNr];
		}
	} else if( entry >= '0' && entry <= '9' ){
		mainptrs->readhead +=-1 + inpstrtonum(args[mainptrs->argumentNr], mainptrs->userInput, mainptrs->readhead, mainptrs->bigEndian);
	} else if( entry == '\"' ) {
		mainptrs->readhead += strtostrnum(args[mainptrs->argumentNr], mainptrs->userInput, 1 + mainptrs->readhead);
	}

	if(mainptrs->debug == 'v') printf("stdlib argumented\n");
	return 0;
}

int executehandler_std(GLOBAL* mainptrs){
	time_t begin_time = time(&begin_time);
	bool doprint = true;
	if(mainptrs->inputMode != 'i') doprint = false;
	int rettype = UnDef;
	num_t* printptr = args[0];
	FILE* quicfptr;

	num_t dummy;
	initnum(&dummy, 15, 0, 0);
	switch (mainptrs->instructNr){
		case set:
			copynum(args[0], args[1], 0);
			break;
		case dset:
			args[0]->nump[numtoint(args[1], false) % args[0]->len] = args[2]->nump[0];
			break;
		case dget:
			free(dummy.nump);
			initnum(&dummy, 1, args[0]->sign, args[0]->dim);
			dummy.nump[0] = args[0]->nump[numtoint(args[1], false) % args[0]->len];
			copynum(args[2], &dummy, 0);
			printptr = args[2];
			break;
		case rev:
			reversenum(args[0]);
			break;
		case sel:
			selectsectionnum(args[0], args[1], numtoint(args[2], false), numtoint(args[0], false) + numtoint(args[2], false));
			break;
		case cut:
			selectsectionnum(&dummy, args[0], numtoint(args[1], false), numtoint(args[2], false) + 1);
			copynum(args[0], &dummy, 0);
			break;
		case ins:
			insertnum(args[0], args[1], numtoint(args[2], false));
			break;
		case cton:
			uint8tonum(args[0], args[0]->nump[numtoint(args[1], false) % (int64_t)args[0]->len]);
			rettype = Number;
			break;
		case ntoc:
			dummy.nump[0] = numtouint8(args[0]);
			free(args[0]->nump);
			initnum(args[0], 1, 0, 0);
			args[0]->nump[0] = dummy.nump[0];
			rettype = String;
			break;
		case inc:
			incnum(args[0], false);
			break;
		case dec:
			incnum(args[0], true);
			break;
		case add:
			sumnum(args[0], args[0], args[1], false);
			break;
		case sub:
			sumnum(args[0], args[0], args[1], true);
			break;
		case mul:
			multnum(args[0], args[0], args[1]);
			if(decimalpoint != 0){
				shiftnum(args[0], -1*decimalpoint);
			}
			break;
		case divi:
			if(decimalpoint != 0){
				shiftnum(args[0], decimalpoint);
			}
			divnum(args[0], args[2], args[0], args[1]);
			break;
		case modu:
			if(decimalpoint != 0){
				shiftnum(args[0], decimalpoint);
			}
			divnum(args[2], args[0], args[0], args[1]);
			break;
		case rnd:
			printf("`rand` not yet implemented.\n"); //TODO rand
			break;
		case root:
			printf("`root` not yet implemented.\n"); //TODO root
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
			break;
		case shf:
			shiftnum(args[0], numtoint(args[1], true));
			break;
		case app:
			appendnum(args[0], args[1]);
			break;
		case trun:
			truncatenum(args[0]);
			break;
		case print:
			doprint = true;
			if(args[0]->nump[0] > 9) rettype = String;
			break;
		case nprint:
			doprint = false;
			rettype = Number;
			printnum(args[0], mainptrs->bigEndian + 2);
			break;
		case sprint:
			doprint = false;
			rettype = String;
			printstrnum(args[0]);
			break;
		case firead:
			quicfptr = fopen((char*)(args[1]->nump), "rb");
			if(quicfptr == NULL){ printf("Could not open file '%s'.\n", args[1]->nump); doprint = false; break;}

			args[0]->len = numtoint(args[0], false);
			free(args[0]->nump);
			initnum(args[0], args[0]->len, 0, 0);
			
			fseek(quicfptr, numtoint(args[2], false), SEEK_SET);
			fread(args[0]->nump, 1, args[0]->len, quicfptr);

			fclose(quicfptr);
			rettype = String;
			break;
		case fiwrite:
			if(args[2]->sign == 0) quicfptr = fopen((char*)(args[1]->nump), "wb");
			else quicfptr = fopen((char*)(args[1]->nump), "wb+");
			if(quicfptr == NULL){ printf("Could not open or create file '%s'.\n", args[1]->nump); doprint = false; break;}
			fseek(quicfptr, numtoint(args[2], false), SEEK_SET);
			fwrite(args[0]->nump, 1, args[0]->len, quicfptr);

			fclose(quicfptr);
			rettype = String;
			break;
		case input:
			inputfailed:
			fgets(mainptrs->userInput, mainptrs->userInputLen, stdin);
			if(mainptrs->userInput[0] >= '0' && mainptrs->userInput[0] <= '9')
				inpstrtonum(args[0], mainptrs->userInput, 0, mainptrs->bigEndian);
			else if (mainptrs->userInput[0] == '"'){
				strtostrnum(args[0], mainptrs->userInput, 1);
				rettype = String;
			} else {printf("Input must be a number or string\n"); goto inputfailed;}
			break;
		case sinput:
			fgets(mainptrs->userInput, mainptrs->userInputLen, stdin);
			free(args[0]->nump);
			for(int i = 0; mainptrs->userInput[i] != '\0'; i++)
				if(mainptrs->userInput[i] == '\n') initnum(args[0], i, 0, 0);
			for(uint64_t i = 0; i < args[0]->len; i++)
				if(mainptrs->userInput[i] != '\n') args[0]->nump[i] = mainptrs->userInput[i];
				else args[0]->nump[i] = '\0';
			rettype = String;
			break;
		case push:
			if(!pushtomst(args[0])) doprint = false;
			break;
		case pop:
			if(!popfrommst(args[0])) doprint = false;
			break;
		case peek:
			if(stackptr == stackSize){
				printf("There are no elements on the stack\n");
				doprint = false;
				break;
			}
			copynum(args[0], &stack[stackptr], 0);
			break;
		case flip:
			if(isnumzero(args[0])) args[0]->nump[0] = (uint8_t) 1;
			for(int i = numtoint(args[0], false); i > 0; i--){
				if(!fliperonie()){ doprint = false; break;}
				if(i == 1) printptr = &retstack[retstackptr];
			}
			break;
		case ret:
			if(isnumzero(args[0])) args[0]->nump[0] = (uint8_t) 1;
			for(int i = numtoint(args[0], false); i > 0; i--){
				if(!reteronie()){ doprint = false; break;}
				if(i == 1) {
					printptr = &stack[stackptr];
					copynum(&regs[ans], printptr, 0);
				}
			}
			break;
		case len:
			inttonum(args[1], args[0]->len);
			printptr = args[1];
			break;
		case SAVE:
			saveload((char*)args[0]->nump, 's', mainptrs);
			doprint = false;
			break;
		case LOAD:
			saveload((char*)args[0]->nump, 'l', mainptrs);
			doprint = false;
			break;
		case run:
			mainptrs->flist[++mainptrs->fileNr].fp = fopen((char*)args[0]->nump, "r");
			if(mainptrs->flist[mainptrs->fileNr].fp == NULL){
				printf("Could not open script '%s'.\n", args[0]->nump);
				mainptrs->fileNr--;
			} else {
				mainptrs->inputMode = 'f';
				mainptrs->flist[mainptrs->fileNr].rdpos = 0;
				mainptrs->flist[mainptrs->fileNr].lineNr = 0;
				mainptrs->flist[mainptrs->fileNr].begin_time = time(&mainptrs->flist[mainptrs->fileNr].begin_time);
			}
			doprint = false;
			break;
	}
	if(rettype == UnDef){
		if(printptr->nump[0] > 9) rettype = String;
		else rettype = Number;
	}

	if(doprint){
		switch (rettype){
			case Number:
				printnum(printptr, mainptrs->bigEndian);
				break;
			case String:
				printstrnum(printptr);
				printf("\n");
				break;
		}
		copynum(&regs[ans], printptr, 0);
	}
	free(dummy.nump);

	time_t end_time = time(&end_time);
	inttonum(&regs[ptme], (int64_t) end_time - begin_time);

	if(mainptrs->debug == 'v') printf("stdlib executed\n");
	return 0;
}

int libFuncPtrs_AASM_STDLIB(){
	initfuncs[AASM_STDLIB] = &init_std;
	instructhandlers[AASM_STDLIB] = &instructhandler_std;
	argumenthandlers[AASM_STDLIB] = &argumenthandler_std;
	executehandlers[AASM_STDLIB] = &executehandler_std;
	updatefuncs[AASM_STDLIB] = &update_std;
	freefuncs[AASM_STDLIB] = &free_std;

	return 0;
}
//############################################### </Std essentials>

#endif
