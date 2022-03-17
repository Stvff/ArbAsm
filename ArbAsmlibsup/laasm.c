//gcc test.c -lm -o test && ./test
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "arbnum_global.h"
#include "arbnum_stdlib.h"

fun initfuncs[libAmount];

fun instructhandlers[libAmount];
fun argumenthandlers[libAmount];

fun executehandlers[libAmount];

fun updatefuncs[libAmount];
fun freefuncs[libAmount];

//############################################### <Main surroundings>
int init_main(GLOBAL* mainptrs){
	mainptrs->userInputLen = initialuserInputLen;
	mainptrs->userInput = (char*) malloc((mainptrs->userInputLen + 10)*sizeof(char));
	mainptrs->readhead = 0;

	mainptrs->lookingMode = 'i';

	mainptrs->running = true;
	mainptrs->scriptLoops = 0;
	mainptrs->inputMode = 'i';

	mainptrs->bigEndian = 0;

	mainptrs->recDep = recursionDepth;
	mainptrs->flist = (file_t*) malloc(mainptrs->recDep*sizeof(file_t));
	mainptrs->fileNr = 0;
	mainptrs->flist[mainptrs->fileNr].fp = stdin;
	mainptrs->flist[mainptrs->fileNr].rdpos = 0;
	mainptrs->flist[mainptrs->fileNr].linenr = 0;
	mainptrs->flist[mainptrs->fileNr].begin_time = time(&mainptrs->flist[mainptrs->fileNr].begin_time);

	mainptrs->debug = 's';
	if(mainptrs->debug == 'v') printf("main initted\n");
	return 0;
}

int instructhandler_main(GLOBAL* mainptrs){
	char instructstringmain[][maxKeywordLen] = {"\\", "h", "\0end"};
	enum instructsmain { slash, help };

	mainptrs->instructNr = strlook(mainptrs->userInput, instructstringmain, &mainptrs->readhead);

	switch (mainptrs->instructNr){
		case slash:
			mainptrs->lookingMode = 'd';
			mainptrs->running = false;
			break;
		case help:
			printf("To preform an operation, type an instruction mnemonic (e.g. `set`, `print`, `add`, `push`)\nand add the appropriate amount of arguments seperated by commas.\n\n");
			printf("The general purpose registers are `gr1`, `gr2`, `gr3` and `gr4`.\n\n");
			printf("To change notation from little endian (the default) to big endian, set the register `endian` to 1.\n");
			printf("To change maximum line length, set the register `inplen` to the desired value.\n\n");
			printf("Enter `\\` to close the program.\n\n");
			printf("(P.S. Did you know that the actual plural of \"comma\" is \"commata\"?)\n");
			mainptrs->lookingMode = 'd';
			break;
	}

	if(mainptrs->debug == 'v') printf("main instructed\n");
	return 0;
}

int argumenthandler_main(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("main argumented\n");
	return 0;
}

int executehandler_main(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("main executed\n");
	return 0;
}

int update_main(GLOBAL* mainptrs){
	free(mainptrs->userInput);
	mainptrs->userInput = (char*) malloc((mainptrs->userInputLen + maxKeywordLen)*sizeof(char));

	if(mainptrs->debug == 'v') printf("main updated\n");
	return 1;
}

int free_main(GLOBAL* mainptrs){
	free(mainptrs->userInput);

	free(mainptrs->flist);

	if(mainptrs->debug == 'v') printf("main freed\n");
	return 1;
}
//############################################### </Main surroundings>

//############################################### <Library declarations>
int initLibFuncPtrs(){	
	initfuncs[0] = &init_main;
	initfuncs[1] = &init_std;

	instructhandlers[0] = &instructhandler_main;
	instructhandlers[1] = &instructhandler_std;

	argumenthandlers[0] = &argumenthandler_main;
	argumenthandlers[1] = &argumenthandler_std;

	executehandlers[0] = &executehandler_main;
	executehandlers[1] = &executehandler_std;

	updatefuncs[0] = &update_main;
	updatefuncs[1] = &update_std;

	freefuncs[0] = &free_main;
	freefuncs[1] = &free_std;
	return 0;
}
//############################################### </Library declarations>

//############################################### <Main operation>
int dothing(GLOBAL* mainptrs){
	mainptrs->libNr = 0;
	mainptrs->instructNr = -1;
	mainptrs->readhead = 0;
	mainptrs->argumentNr = 0;
	mainptrs->lookingMode = 'i';
	char entry = mainptrs->userInput[mainptrs->readhead];

//	main parsing loop
	while (mainptrs->lookingMode != 'd' && entry != '\0' && entry != '\n' && entry != '\r' && entry != ';')
	{
		entry = mainptrs->userInput[mainptrs->readhead];
		if(entry != ' ' && entry != '\t'){
//			functions
			if(mainptrs->lookingMode == 'i'){

				for(mainptrs->libNr = 0; mainptrs->libNr < libAmount && mainptrs->instructNr == -1; mainptrs->libNr++){
					instructhandlers[mainptrs->libNr](mainptrs);
				}

				if(mainptrs->libNr == libAmount && mainptrs->instructNr == -1){
					printf("Not a valid instruction. (line %d)\n", mainptrs->flist[mainptrs->fileNr].linenr);
					mainptrs->lookingMode = 'd';
				}
				if(mainptrs->lookingMode == 'i'){
					mainptrs->instructNr = -1;
				}
				mainptrs->libNr--;
//			arguments
			} else if(mainptrs->lookingMode == 'a'){

				if(mainptrs->argumentNr == maxArgumentAmount) {
					mainptrs->lookingMode = 'e';
				} else if(entry == ',') {
					mainptrs->argumentNr++;
				} else
					argumenthandlers[mainptrs->libNr](mainptrs);

			}
		}
		if(mainptrs->debug == 'v') printf("entry: %c\n", entry);
		mainptrs->readhead++;
	}

	if(mainptrs->lookingMode != 'd') executehandlers[mainptrs->libNr](mainptrs);

	if(mainptrs->debug == 'v') printf("thing done\n");
	return 0;
}

int getuserInput(GLOBAL* mainptrs){
	switch (mainptrs->inputMode) {
		case 'i':
			printf("\\\\\\ ");
			fgets(mainptrs->userInput, mainptrs->userInputLen, mainptrs->flist[mainptrs->fileNr].fp);
			break;
		case 'F':
		case 'f':
			
			fgets(mainptrs->userInput, mainptrs->userInputLen, mainptrs->flist[mainptrs->fileNr].fp);
			break;
		case 'c':
			mainptrs->inputMode = 'i';
			break;
	}
	if(mainptrs->debug == 'v') printf("userInput gotten\n");
	return 0;
}
//############################################### </Main operation>
int handlecommandlineargs(int argc, char* argv[], GLOBAL* mainptrs){
	for(int i = 1; i < argc; i++){
		if(argv[i][0] == '-'){
			switch(argv[i][1]){
				case 'E':
				case 'e':
					mainptrs->running = false;
					break;
				case 'B':
				case 'b':
					mainptrs->bigEndian = true;
					break;
				case 'H':
				case 'h':
					printf("Usage: aasm <statement (without spaces)> <options>\n");
					printf("Options:\n");
					printf("  -e              Exit immediately after executing the statement that was passed as argument.\n");
					printf("  -u <statefile>  Load the designated statefile, and save to it again on exit.\n");
					printf("  -b              Sets the notation to big endian before doing anything else.\n");
					printf("  -h              Look ma! I'm on TV!\n");
					mainptrs->inputMode = 'c';
					mainptrs->running = false;
					break;
			}
		} else {
			sscanf(argv[i], "%s", mainptrs->userInput);
			mainptrs->inputMode = 'c';
		}
	}
	return 0;
}

int main(int argc, char* argv[]){
	GLOBAL mainitems;
	initLibFuncPtrs();

	for(int i = 0; i < libAmount; i++)
		initfuncs[i](&mainitems);
	handlecommandlineargs(argc, argv, &mainitems);

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
