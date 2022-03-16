#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include "arbnum.h"
#include "arbnum_quats.h"
unsigned int inputlen = 256;
char* userInput;

bool reCurse = false;
bool ScriptingMode = false;
int scriptLoops = 0;

int bigEndian = 0;

int stackSize = 20;
num_t* stack;
int stackptr;
num_t* retstack;
int retstackptr;

int regamount = 14;
num_t regs[14];
enum registers {gr1, gr2, gr3, gr4, ir, flag,
				inplen, endian, stacsz, mstptr,
				rstptr, tme, ptme, loop};
enum instructs {endprog=1, slashn, wincrap, space, tab, semicolon, h,
				set, dset, dget, rev, sel, cton, ntoc,
				inc, dec, add, sub, mul, divi, modu,
				cmp, ucmp, rot, shf, app, len, 
				print, sprint, nprint, draw,
				firead, fiwrite, input, sinput,
				push, pop, peek, flip, ret,
				SCR, SAVE, LOAD, Ce, Cg, Cs};

const int TheMaximumLengthOfTheThings = 10;
char registerstring[][10] = { "gr1", "gr2", "gr3", "gr4", "ir",
							"flag", "inplen", "endian", "stacsz", "mstptr", "rstptr",
							"time", "ptime", "loop", "\0end" };
char instructstring[][10] = { "\\", "\n", "\r", " ", "	", ";", "h",
							"set", "dset", "dget", "rev", "sel", "cton", "ntoc",
							"inc", "dec", "add", "sub", "mul", "div", "mod",
							"cmp", "ucmp", "rot", "shf", "app", "len",
							"print", "sprint", "nprint", "draw",
							"fread", "fwrite", "input", "sinput",
							"push", "pop", "peek", "flip", "ret", 
							"SCR", "SAVE", "LOAD", "Ce", "Cg", "Cs", "\0end" };

int qregamount = 4;
qua_t qregs[4];
enum quaternionregisters {qr1, qr2, qr3, qr4};
enum quaternioninstructs {qset=1, qnget, qnset, qadd, qsub, qmul, qsmul};

char quatregistring[][10] = { "qr1", "qr2", "qr3", "qr4", "\0end" };

enum typeenum {number, quater, string, image};

typedef struct FileInfo {
	FILE *fp;
	int rdpos;
	int linenr;
	time_t begin_time;
} file_t;

int recdep = 10;
file_t flist[10];

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

void functionswitch(int instruction, num_t* args[], int types[], qua_t* qargs[]){
	time_t begin_time = time(&begin_time);
	bool doprint = !ScriptingMode;
	int rettype = types[0];
	num_t* printptr = args[0];
	qua_t* qprintptr = qargs[0];
	FILE* quicfptr;

	num_t dummy;
	initnum(&dummy, 15, 0, 0);
	switch (instruction){
		case set:
			if(types[0]==quater)
				copyquat(qargs[0], qargs[1], 0);
			else
				copynum(args[0], args[1], 0);
			if(types[1]==string) rettype = string;
			break;
		case dset:
			args[0]->nump[numtoint(args[1], false) % args[0]->len] = args[2]->nump[0];
			break;
		case dget:
			free(dummy.nump);
			initnum(&dummy, 1, args[0]->sign, args[1]->dim);
			dummy.nump[0] = args[0]->nump[numtoint(args[1], false) % args[0]->len];
			copynum(args[0], &dummy, 0);
			break;
		case rev:
			reversenum(args[0]);
			break;
		case sel:
			selectsectionnum(&dummy, args[0], numtoint(args[1], false), numtoint(args[2], false));
			copynum(args[0], &dummy, 0);
			break;
		case cton:
			uint8tonum(args[0], args[0]->nump[numtoint(args[1], false) % (int32_t)args[0]->len]);
			rettype = number;
			break;
		case ntoc:
			dummy.nump[0] = numtouint8(args[0]);
			free(args[0]->nump);
			initnum(args[0], 1, 0, 0);
			args[0]->nump[0] = dummy.nump[0];
			rettype = string;
			break;
		case inc:
			incnum(args[0], false);
			break;
		case dec:
			incnum(args[0], true);
			break;
		case add:
			if(types[0]==quater)
				sumquat(qargs[0], qargs[0], qargs[1], false);
			else
				sumnum(args[0], args[0], args[1], false);
			break;
		case sub:
			if(types[0]==quater)
				sumquat(qargs[0], qargs[0], qargs[1], true);
			else
				sumnum(args[0], args[0], args[1], true);
			break;
		case mul:
			if(types[0]==quater)
				multquat(qargs[0], qargs[0], qargs[1]);
			else
				multnum(args[0], args[0], args[1]);
			break;
		case divi:
			divnum(args[0], args[2], args[0], args[1]);
			break;
		case modu:
			divnum(args[2], args[0], args[0], args[1]);
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
		case print:
			doprint = true;
			if(args[0]->nump[0] > 9) rettype = string;
			break;
		case nprint:
			doprint = false;
			rettype = number;
			printnum(args[0], bigEndian + 2);
			break;
		case sprint:
			doprint = false;
			rettype = string;
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
			rettype = string;
			break;
		case fiwrite:
			if(args[2]->sign == 0) quicfptr = fopen((char*)(args[1]->nump), "wb");
			else quicfptr = fopen((char*)(args[1]->nump), "wb+");
			if(quicfptr == NULL){ printf("Could not open or create file '%s'.\n", args[1]->nump); doprint = false; break;}
			fseek(quicfptr, numtoint(args[2], false), SEEK_SET);
			fwrite(args[0]->nump, 1, args[0]->len, quicfptr);

			fclose(quicfptr);
			rettype = string;
			break;
		case input:
			inputfailed:
			fgets(userInput, inputlen, stdin);
			if(userInput[0] >= '0' && userInput[0] <= '9')
				inpstrtonum(args[0], userInput, 0, bigEndian);
			else if (userInput[0] == '"'){
				strtostrnum(args[0], userInput, 1);
				rettype = string;
			} else {printf("Input must be a number or string\n"); goto inputfailed;}
			break;
		case sinput:
			fgets(userInput, inputlen, stdin);
			free(args[0]->nump);
			for(int i = 0; userInput[i] != '\n'; i++)
				if(userInput[i + 1] == '\n') initnum(args[0], i + 1, 0, 0);
			for(unsigned int i = 0; i < args[0]->len; i++)
				args[0]->nump[i] = userInput[i];
			rettype = string;
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
			if(!fliperonie()) doprint = false;
			else printptr = &retstack[retstackptr];
			break;
		case ret:
			if(!reteronie()) doprint = false;
			else printptr = &stack[stackptr];
			break;
		case len:
			inttonum(args[1], args[0]->len);
			printptr = args[1];
			break;
	}

	if(doprint) switch (rettype){
		case number:
			printnum(printptr, bigEndian);
			break;
		case quater:
			printquat(qprintptr, bigEndian);
			break;
		case string:
			printstrnum(printptr);
			printf("\n");
			break;
	};

	free(dummy.nump);	
	time_t end_time = time(&end_time);
	inttonum(&regs[ptme], end_time - begin_time);
}

void freestack(){
	for(int i = stackptr; i < stackSize; i++)
		free(stack[i].nump);
	free(stack);
	for(int i = retstackptr; i < stackSize; i++)
		free(retstack[i].nump);
	free(retstack);
}

int saveload(char* path[], bool save){
//	int readfilei = 0;
	FILE* fp;
	if(save){
		fp = fopen(*path, "wb+");
		if(fp == NULL){
			printf("Could not open file to save to: '%s'.\n", userInput);
			return 1;
		}
		for(int i = 0; i < regamount; i++)
			savenum(fp, &regs[i]);
		for(int i = stackSize-1; i >= stackptr; i--)
			savenum(fp, &stack[i]);
		for(int i = stackSize-1; i >= retstackptr; i--)
			savenum(fp, &retstack[i]);

		if(!ScriptingMode) printf("Saved state to '%s'.\n", *path);
	} else {
		fp = fopen(*path, "rb");
		if(fp == NULL){
			printf("Could not open file to load from: '%s'.\n", userInput);
			return 1;
		}
		for(int i = 0; i < regamount; i++)
			loadnum(fp, &regs[i]);

		freestack();
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

		if(!ScriptingMode) printf("Loaded state from '%s'.\n", *path);
	}
	fclose(fp);
	return 0;
}

bool dothing(file_t file){
//	printf("start dothing\n");
	bool returnbool = true;
	int startat;
	int offsetbegin = 0;
	startofdothing:;
	int instruction = strlook(userInput, instructstring, offsetbegin, &startat);
	int readfilei;//this is used in the things that take strings as arguments
	int thatoff;  //idem

	switch (instruction){
		case 0:
			printf("Not a valid instruction. (line %d)\n", file.linenr);
			goto donothing;
			break;
		case endprog:
			printf("Good day!\n");
			returnbool = false;
			goto donothing;
			break;
		case slashn:
		case wincrap:
		case semicolon:
			goto donothing;
			break;
		case h:
			printf("To preform an operation, type an instruction mnemonic (e.g. `set`, `print`, `add`, `push`)\nand add the appropriate amount of arguments seperated by commas.\n\n");
			printf("The general purpose registers are `gr1`, `gr2`, `gr3` and `gr4`.\n\n");
			printf("To change notation from little endian (the default) to big endian, set the register `endian` to 1.\n");
			printf("To change maximum line length, set the register `inplen` to the desired value.\n\n");
			printf("Enter `\\` to close the program.\n\n");
			printf("(P.S. Did you know that the actual plural of \"comma\" is \"commata\"?)\n");
			goto donothing;
			break;
		case space:
		case tab:
			offsetbegin++;
			goto startofdothing;
			break;
		case Ce:
			offsetbegin = startat + 1;
			if(regs[flag].nump[0] == 0) goto startofdothing;
			else goto donothing;
			break;
		case Cg:
			offsetbegin = startat + 1;
			if(regs[flag].nump[0] == 1) goto startofdothing;
			else goto donothing;
			break;
		case Cs:
			offsetbegin = startat + 1;
			if(regs[flag].nump[0] == 2) goto startofdothing;
			else goto donothing;
			break;
		case SCR:
			if(ScriptingMode) reCurse = true;
			ScriptingMode = true;
			thatoff = offsetbegin + 4;
			for(readfilei = thatoff; userInput[readfilei] != '\0' && userInput[readfilei] != '\r' && userInput[readfilei] != '\n'; readfilei++)
				userInput[readfilei - thatoff] = userInput[readfilei];
			userInput[readfilei - thatoff] = '\0';

			goto donothing;
			break;
		case SAVE:
			thatoff = offsetbegin + 5;
			for(readfilei = thatoff; userInput[readfilei] != '\0' && userInput[readfilei] != '\r' && userInput[readfilei] != '\n'; readfilei++)
				userInput[readfilei - thatoff] = userInput[readfilei];
			userInput[readfilei - thatoff] = '\0';

			saveload(&userInput, true);
			goto donothing;
			break;
		case LOAD:
			thatoff = offsetbegin + 5;
			for(readfilei = thatoff; userInput[readfilei] != '\0' && userInput[readfilei] != '\r' && userInput[readfilei] != '\n'; readfilei++)
				userInput[readfilei - thatoff] = userInput[readfilei];
			userInput[readfilei - thatoff] = '\0';

			saveload(&userInput, false);
			goto donothing;
			break;
	}

	int argam = 3;
	num_t* tmpptr[3];
	num_t tmp[3];

	qua_t* tmpqptr[3];
	qua_t tmpq[3];

	int types[3];

	for(int i = 0; i < argam; i++){
		initnum(&tmp[i], 1, 0, 0);
		tmp[i].nump[0] = 0;
		tmpptr[i] = &tmp[i];
		initquat(&tmpq[i]);
		tmpqptr[i] = &tmpq[i];

		types[i] = number;
	}

	int argn = 0;
	int diminq = 0;
	int loInputentry = 0;
	char entry;
	bool therewasnosemi = true;
	for(unsigned int i = startat + offsetbegin; i < inputlen; i+=loInputentry){
		loInputentry = 1;
		entry = userInput[i];

		if(entry == ';'){
			therewasnosemi = false;
			break;
		}

		if(argn < argam){
			if(entry == ','){ argn++; diminq = 0;}
			if(entry == '+'){ diminq++; types[argn] = quater;}
			if( (entry >= 'a' && entry <= 'z') || (entry >= 'A' && entry <= 'Z') ){

				int id;
				if(entry != 'q'){
					id = strlook(userInput, registerstring, i, &loInputentry);
					if(id == 0){
						printf("Register at argument %d on line %d is not a register.\n", argn + 1, file.linenr);
						goto donothingsafe;
					} else {
						tmpptr[argn] = &regs[id - 1];
						copynum(&tmpqptr[argn]->q[diminq%4], tmpptr[argn], false);
					}
				} else {
					id = strlook(userInput, quatregistring, i, &loInputentry);
					if(id == 0){
						printf("Register at argument %d on line %d is not a (quaternion) register.\n", argn + 1, file.linenr);
						goto donothingsafe;
					} else {
						types[argn] = quater;
						tmpqptr[argn] = &qregs[id - 1];
						tmpptr[argn] = &qregs[id - 1].q[diminq%4];
//						copynum(tmpptr[argn], &tmpqptr[argn]->q[diminq%4], false);
					}
				}

			} else if (entry >= '0' && entry <= '9'){
			
				loInputentry = inpstrtonum(&tmpqptr[argn]->q[diminq%4], userInput, i, bigEndian);
				tmpptr[argn] = &tmpqptr[argn]->q[diminq%4];
//				copynum(&tmpqptr[argn]->q[diminq%4], tmpptr[argn], false);

			} else if (entry == '"'){

				loInputentry = strtostrnum(tmpptr[argn], userInput, i+1) + 1;
				types[argn] = string;

			}
		}
	}
	if(therewasnosemi){
		printf("The statement on line %d is too long (maximum is %u characters).\nIt is possible to change the maximum by modifying the `inplen` register.\n", file.linenr, inputlen);
		goto donothingsafe;
	}
//	for(int i = 0; i < argam; i++) printquat(tmpqptr[i], bigEndian);

	functionswitch(instruction, tmpptr, types, tmpqptr);

	donothingsafe:
	for(int i = 0; i < argam; i++){
		freequat(&tmpq[i]);
		free(tmp[i].nump);
	}
	donothing:
	return returnbool;
}

void updateessentials(){
	inputlen = numtoint(&regs[inplen], false);
	bigEndian = numtoint(&regs[endian], false);
	scriptLoops = numtoint(&regs[loop], false);
	inttonum(&regs[mstptr], stackptr);
	inttonum(&regs[rstptr], retstackptr);
	time_t thetime = time(&thetime);
	inttonum(&regs[tme], (int32_t) thetime);
}

void setessentialsready(){
	inttonum(&regs[inplen], inputlen);
	inttonum(&regs[endian], bigEndian);
	inttonum(&regs[stacsz], stackSize);
	inttonum(&regs[loop], scriptLoops);
	inttonum(&regs[mstptr], stackSize);
	inttonum(&regs[rstptr], stackSize);
	time_t thetime = time(&thetime);
	inttonum(&regs[tme], (int32_t) thetime);
}

void flushuserInput(){
	for(unsigned int i = 0; i < inputlen; i++)
		userInput[i] = ';';
}

int main(int argc, char* argv[]){
	initnumarray(regamount, regs, 7, 0, 0);
	initquatarray(qregamount, qregs);
	stackptr = stackSize;
	stack = (num_t*) malloc(stackSize * sizeof(num_t));
	retstackptr = stackSize;
	retstack = (num_t*) malloc(stackSize * sizeof(num_t));

	userInput = (char*) malloc((TheMaximumLengthOfTheThings + inputlen) * sizeof(char));
	flushuserInput();

	file_t file;
	file.fp = stdin;
	file.rdpos = 0;
	file.linenr = 0;
	file.begin_time = time(&file.begin_time);
	int watch = 0;
	bool recursing = false;

	bool running = true;
	bool prevmode = false;
	bool cont = true;
	bool wascommand = false;
	int8_t ataste;
	time_t end_time;

	bool using = false;
	int usingarg = 0;
	for(int i = 1; i < argc; i++){
		if(argv[i][0] == '-'){
			switch(argv[i][1]){
				case 'E':
				case 'e':
					cont = false;
					break;
				case 'U':
				case 'u':
					if(i+1 < argc){
						i++;
						saveload(&argv[i], false);
						using = true;
						usingarg = i;
					} else printf("Statefile name missing.\n");
					break;
				case 'B':
				case 'b':
					bigEndian = true;
					break;
				case 'H':
				case 'h':
					printf("Usage: aasm <statement (without spaces)> <options>\n");
					printf("Options:\n");
					printf("  -e              Exit immediately after executing the statement that was passed as argument.\n");
					printf("  -u <statefile>  Load the designated statefile, and save to it again on exit.\n");
					printf("  -b              Sets the notation to big endian before doing anything else.\n");
					printf("  -h              Look ma! I'm on TV!\n");
					cont = false;
					wascommand = true;
					break;
			}
		} else {
			sscanf(argv[i], "%s", userInput);
			wascommand = true;
		}
	}

	setessentialsready();

	if(cont) printf("Good to see you!\nEnter `h` for quick tips and `\\` to close the program.\n");

	do {
		if(stackSize != numtoint(&regs[stacsz], false)){
			freestack();
			stackSize = numtoint(&regs[stacsz], false);
			stackptr = stackSize;
			retstackptr = stackSize;
			stack = (num_t*) malloc(stackSize * sizeof(num_t));
			retstack = (num_t*) malloc(stackSize * sizeof(num_t));
		}

		if(ScriptingMode){
			ataste = fgetc(file.fp);
			file.linenr++;
			fseek(file.fp, file.rdpos, SEEK_SET);
			if(ataste == EOF && scriptLoops == 1){
				file.rdpos = 0;
				file.linenr = 0;
				fseek(file.fp, file.rdpos, SEEK_SET);
			} else if(ataste == EOF){
				file.rdpos = 0;
				file.linenr = 0;
				fclose(file.fp);
				end_time = time(&end_time);
				inttonum(&regs[ptme], end_time - file.begin_time);
//				printf("time: "); printnum(&regs[tme], bigEndian);
				if(watch == 0){
//					printf("Script completed.\n");
					file.fp = stdin;
					ScriptingMode = false;
					wascommand = true;
					recursing = false;
				} else if(recursing){
					file = flist[--watch];
					wascommand = true;
				}
			}
		}
		
		if(!wascommand){
			if(!ScriptingMode) printf("\\\\\\ ");
			fgets(userInput, inputlen, file.fp);
//			printf("userInput: '%s', readheadpos: %d\n", userInput, readheadpos);

			for(int i = 0; userInput[i] != '\0' && ScriptingMode; i++)
				file.rdpos++;
		} wascommand = false;

		running = dothing(file);
		updateessentials();

		if(reCurse){
			flist[watch++] = file;
			prevmode = false;
			reCurse = false;
			recursing = true;
		}

		if(prevmode != ScriptingMode && ScriptingMode){
			file.fp = fopen(userInput, "r");
			if(file.fp == NULL){
				printf("Could not open script '%s'.\n", userInput);
				ScriptingMode = false;
				file.fp = stdin;
			} else file.begin_time = time(&file.begin_time);
			file.rdpos = 0;
		} prevmode = ScriptingMode;

		free(userInput);
		userInput = (char*) malloc((TheMaximumLengthOfTheThings + inputlen) * sizeof(char));
		flushuserInput();
//		printf("running %d, cont %d, ScriptingMode %d\n", running, cont, ScriptingMode);
	} while ((running && cont) || ScriptingMode);

	if(using) saveload(&argv[usingarg], true);

	free(userInput);
	freestack();
	freenumarray(regamount, regs);
	freequatarray(qregamount, qregs);
	return 0;
}
