//gcc test.c -lm -o test && ./test
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "defglobal.h"
#include "arbnum_stdlib.h"

enum CompiletimeConstants {
	libAmount = 2,
	userInputLength = 256,
	recursionDepth = 10
};

fun initfuncs[libAmount];
fun updatefuncs[libAmount];
fun freefuncs[libAmount];
fun instructions[libAmount];
fun arguments[libAmount];

//############################################### <Main surroundings>
int initmain(GLOBAL* mainptrs){
	mainptrs->userInputLen = userInputLength;
	mainptrs->userInput = (char*) malloc((mainptrs->userInputLen + 10)*sizeof(char));
	mainptrs->readhead = 0;

	mainptrs->running = true;
	mainptrs->mode = 'i';

	mainptrs->lookingMode = 'i';

	mainptrs->bigEndian = 0;

	mainptrs->recDep = recursionDepth;
	mainptrs->flist = (file_t*) malloc(mainptrs->recDep*sizeof(file_t));
	mainptrs->fileNr = 0;
	mainptrs->flist[mainptrs->fileNr].fp = stdin;
	mainptrs->flist[mainptrs->fileNr].rdpos = 0;
	mainptrs->flist[mainptrs->fileNr].linenr = 0;
	mainptrs->flist[mainptrs->fileNr].begin_time = time(&mainptrs->flist[mainptrs->fileNr].begin_time);

	printf("main initted\n");
	return 0;
}

int freemain(GLOBAL* mainptrs){
	free(mainptrs->userInput);

	free(mainptrs->flist);

	printf("main freed\n");
	return 1;
}

int updatemain(GLOBAL* mainptrs){
	free(mainptrs->userInput);
	mainptrs->userInput = (char*) malloc((mainptrs->userInputLen + 10)*sizeof(char));

	printf("main updated\n");
	return 1;
}
//############################################### </Main surroundings>

//############################################### <Library declarations>
int initLibFuncPtrs(){	
	initfuncs[0] = &initmain;
	initfuncs[1] = &initstd;

	updatefuncs[0] = &updatemain;
	updatefuncs[1] = &updatestd;

	freefuncs[0] = &freemain;
	freefuncs[1] = &freestd;
	return 0;
}
//############################################### </Library declarations>

//############################################### <Main operation>
int dothing(GLOBAL* mainptrs){
	mainptrs->readhead = 0;
	char entry = mainptrs->userInput[mainptrs->readhead];
	while (entry != '\0' && entry != '\n' && entry != '\r'){
		entry = mainptrs->userInput[mainptrs->readhead];
		
	}
	
	return 0;
}

int getuserInput(GLOBAL* mainptrs){
	switch (mainptrs->mode) {
		case 'i':
			printf("<> ");
		case 'f':
		case 'F':
			fgets(mainptrs->userInput, mainptrs->userInputLen, mainptrs->flist[mainptrs->fileNr].fp);
			break;
	}

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

	} while(mainitems.running);

	for(int i = 0; i < libAmount; i++)
		freefuncs[i](&mainitems);
	return 0;
}
