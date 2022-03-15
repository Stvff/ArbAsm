//gcc test.c -lm -o test && ./test
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "defglobal.h"
#include "arbnum_stdlib.h"

enum CompiletimeConstantsMain {
	libAmount = 2,
	initaluserInputLen = 256,
	recursionDepth = 10,
};

fun initfuncs[libAmount];

fun instructions[libAmount];
fun arguments[libAmount];

fun updatefuncs[libAmount];
fun freefuncs[libAmount];

//############################################### <Main surroundings>
int initmain(GLOBAL* mainptrs){
	mainptrs->userInputLen = initaluserInputLen;
	mainptrs->userInput = (char*) malloc((mainptrs->userInputLen + 10)*sizeof(char));
	mainptrs->readhead = 0;

	mainptrs->lookingMode = 'i';

	mainptrs->running = true;
	mainptrs->scriptLoops = 0;
	mainptrs->mode = 'i';

	mainptrs->bigEndian = 0;

	mainptrs->recDep = recursionDepth;
	mainptrs->flist = (file_t*) malloc(mainptrs->recDep*sizeof(file_t));
	mainptrs->fileNr = 0;
	mainptrs->flist[mainptrs->fileNr].fp = stdin;
	mainptrs->flist[mainptrs->fileNr].rdpos = 0;
	mainptrs->flist[mainptrs->fileNr].linenr = 0;
	mainptrs->flist[mainptrs->fileNr].begin_time = time(&mainptrs->flist[mainptrs->fileNr].begin_time);

	mainptrs->debug = 'v';
	if(mainptrs->debug == 'v') printf("main initted\n");
	return 0;
}

int instructionsmain(GLOBAL* mainptrs){
	char entry = mainptrs->userInput[mainptrs->readhead];
	if(entry == '\\' || entry == 'q') mainptrs->running = false;

	if(mainptrs->debug == 'v') printf("main instructed\n");
	return 0;
}

int argumentsmain(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("main argumented\n");
	return 0;
}

int updatemain(GLOBAL* mainptrs){
	free(mainptrs->userInput);
	mainptrs->userInput = (char*) malloc((mainptrs->userInputLen + maxKeywordLen)*sizeof(char));

	if(mainptrs->debug == 'v') printf("main updated\n");
	return 1;
}

int freemain(GLOBAL* mainptrs){
	free(mainptrs->userInput);

	free(mainptrs->flist);

	if(mainptrs->debug == 'v') printf("main freed\n");
	return 1;
}
//############################################### </Main surroundings>

//############################################### <Library declarations>
int initLibFuncPtrs(){	
	initfuncs[0] = &initmain;
	initfuncs[1] = &initstd;

	instructions[0] = &instructionsmain;
	instructions[1] = &instructionsstd;

	arguments[0] = &argumentsmain;
	arguments[1] = &argumentsstd;

	updatefuncs[0] = &updatemain;
	updatefuncs[1] = &updatestd;

	freefuncs[0] = &freemain;
	freefuncs[1] = &freestd;
	return 0;
}
//############################################### </Library declarations>

//############################################### <Main operation>
int dothing(GLOBAL* mainptrs){
	int libNr = 0;
	int instructNr = 0;
	mainptrs->readhead = 0;
	char entry = mainptrs->userInput[mainptrs->readhead];
	while (entry != '\0' && entry != '\n' && entry != '\r' && entry != ';'){
		entry = mainptrs->userInput[mainptrs->readhead];
		instructions[instructNr](mainptrs);
		mainptrs->readhead++;
	}
	if(mainptrs->debug == 'v') printf("thing done\n");
	return 0;
}

int getuserInput(GLOBAL* mainptrs){
	switch (mainptrs->mode) {
		case 'i':
			printf("\\\\\\ ");
		case 'f':
		case 'F':
			fgets(mainptrs->userInput, mainptrs->userInputLen, mainptrs->flist[mainptrs->fileNr].fp);
			break;
	}
	if(mainptrs->debug == 'v') printf("userInput gotten\n");
	return 0;
}
//############################################### </Main operation>

int main(){
	GLOBAL mainitems;
	initLibFuncPtrs();

	for(int i = 0; i < libAmount; i++)
		initfuncs[i](&mainitems);

	do {
		getuserInput(&mainitems);
		dothing(&mainitems);

		for(int i = 0; i < libAmount; i++)
			updatefuncs[i](&mainitems);

		if(mainitems.debug == 'v') printf("end of main while loop\n");
	} while(mainitems.running);

	for(int i = 0; i < libAmount; i++)
		freefuncs[i](&mainitems);
	return 0;
}
