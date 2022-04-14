#ifndef AASM_MAIN
#define AASM_MAIN 0
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "aasm_global.h"
#include "aasm_stdlib.h"
#include "aasm_quatlib.h"

int libFuncPtrs_AASM_MAIN();

int (*libFuncPtrs[libAmount])() = {
	libFuncPtrs_AASM_MAIN
	,libFuncPtrs_AASM_STDLIB
	,libFuncPtrs_AASM_QUATLIB
};

//############################################### <Main essentials>
int init_main(GLOBAL* mainptrs) {

	mainptrs->userInputLen = initialuserInputLen;
	mainptrs->userInput = (char*) malloc((mainptrs->userInputLen + 10)*sizeof(char));
	mainptrs->userInput[0] = '\0';
	mainptrs->readhead = 0;

	mainptrs->lookingMode = 'i';

	mainptrs->running = true;
	mainptrs->scriptLoops = 0;
	mainptrs->inputMode = 'i';

	mainptrs->bigEndian = 0;

	mainptrs->flist = (file_t*) malloc(recursionDepth*sizeof(file_t));
	mainptrs->fileNr = 0;
	mainptrs->flist[mainptrs->fileNr].fp = stdin;
	mainptrs->flist[mainptrs->fileNr].rdpos = 0;
	mainptrs->flist[mainptrs->fileNr].lineNr = 0;
	mainptrs->flist[mainptrs->fileNr].begin_time = time(&mainptrs->flist[mainptrs->fileNr].begin_time);

	mainptrs->debug = 's';
	if(mainptrs->debug == 'v') printf("main initted\n");
	return 0;
}

int instructhandler_main(GLOBAL* mainptrs){
	char instructstringmain[][maxKeywordLen] = {"\\", "h", ">", "\0end"};
	enum instructsmain { slash, help, early };
	time_t end_time;
	file_t* thefile = &mainptrs->flist[mainptrs->fileNr];

	mainptrs->instructNr = strlook(mainptrs->userInput, instructstringmain, &mainptrs->readhead);

	switch (mainptrs->instructNr){
		case slash:
			mainptrs->lookingMode = 'd';
			mainptrs->running = false;
			printf("Good day!\n");
			break;
		case help:
			printf("To preform an operation, type an instruction mnemonic (e.g. `set`, `print`, `add`, `push`)\nand add the appropriate amount of arguments seperated by commas.\n\n");
			printf("The general purpose registers are `gr1`, `gr2`, `gr3` and `gr4`.\n\n");
			printf("To change notation from little endian (the default) to big endian, enter `set endian, 1`\n");
			printf("To change maximum line length, set the register `inplen` to the desired value.\n\n");
			printf("Enter `\\` to close the program.\n\n");
			printf("More information can be found on https://www.github.com/StevenClifford/ArbAsm/wiki\n\n");
			printf("(P.S. Did you know that the actual plural of \"comma\" is \"commata\"?)\n");
			mainptrs->lookingMode = 'd';
			break;
		case early:
			if(mainptrs->fileNr > 0){
				thefile->lineNr = 0;
				mainptrs->userInput[0] = '\0';
				fclose(thefile->fp);
				mainptrs->fileNr--;
				end_time = time(&end_time);
				inttonum(&regs[ptme], (int64_t) end_time - thefile->begin_time);
				if(mainptrs->fileNr == 0) mainptrs->inputMode = 'i';
			} else printf("This instruction only works in a script.\n");
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
	mainptrs->userInput[0] = '\0';

	if(mainptrs->debug == 'v') printf("main updated\n");
	return 0;
}

int free_main(GLOBAL* mainptrs){
	free(mainptrs->userInput);

	free(mainptrs->flist);

	if(mainptrs->debug == 'v') printf("main freed\n");
	return 0;
}

int libFuncPtrs_AASM_MAIN(){
	initfuncs[AASM_MAIN] = &init_main;
	instructhandlers[AASM_MAIN] = &instructhandler_main;
	argumenthandlers[AASM_MAIN] = &argumenthandler_main;
	executehandlers[AASM_MAIN] = &executehandler_main;
	updatefuncs[AASM_MAIN] = &update_main;
	freefuncs[AASM_MAIN] = &free_main;

	return 0;
}
//############################################### </Main essentials>

//############################################### <Library declarations>
int initLibFuncPtrs(){
	for(int libNr = 0; libNr < libAmount; libNr++){
		libFuncPtrs[libNr]();
	}
	return 0;
}
//############################################### </Library declarations>

//############################################### <Main operation>
int handlecommandlineargs(int argc, char* argv[], GLOBAL* mainptrs){
	for(int i = 1; i < argc; i++){
		if(argv[i][0] == '-'){
			switch(argv[i][1]){
				case 'E':
				case 'e':
					mainptrs->running = false;
					break;
				case 'C':
				case 'c':
					mainptrs->running = !mainptrs->running;
					break;
				case 'D':
				case 'd':
					mainptrs->debug = 'v';
					printf("Debugging mode enabled.\n");
					break;
				case 'V':
				case 'v':
					printversion();
					mainptrs->inputMode = 'w';
					mainptrs->running = false;
					break;
				case 'B':
				case 'b':
					mainptrs->bigEndian = true;
					inttonum(&regs[endian], 1);
					break;
				case 'P':
					if(i+1 < argc){
						i++;
						strtostrnum(&regs[path], argv[i], 0);		
					} else printf("Path missing. Enter `aasm -h` for help.\n");
					break;
				case 'p':
					if(i+1 < argc){
						i++;
						inpstrtonum(&regs[decip], argv[i], 0, mainptrs->bigEndian);
						decimalpoint = numtoint(&regs[decip], true);
					} else printf("Number missing. Enter `aasm -h` for help.\n");
					break;
				case 'L':
				case 'l':
					if(i+1 < argc){
						i++;
						saveload(argv[i], 'l', mainptrs);
					} else printf("Statefile missing. Enter `aasm -h` for help.\n");
					break;
				case 'I':
				case 'i':
					if(i+1 < argc){
						i++;
						sscanf(argv[i], "%s", mainptrs->userInput);
						mainptrs->inputMode = 'w';
					} else printf("Statement missing. Enter `aasm -h` for help.\n");
					break;
				case 'H':
				case 'h':
					printf("Usage: aasm <script> <options>\n");
					printf("Options:\n");
					printf("  -c              Enter command line after the script has stopped running.\n");
					printf("  -i <statement>  Executes the designated statement. This statement can not contain spaces.\n");
					printf("  -e              Exit immediately after executing the statement that was passed as an argument of -i.\n");
					printf("  -l <statefile>  Loads the designated statefile before interpreting any statements.\n");
					printf("  -b              Sets the notation to big endian before interpreting any statements.\n");
					printf("  -p <number>     Sets the virtual decimal point to the number that was passed as an argument of -p.\n");
					printf("  -P <path>       Sets the path variable to the path that was passed as an argument of -P\n");
					printf("  -v              Displays the name, version and libraries.\n");
					printf("  -h              Look ma! I'm on TV!\n");
					mainptrs->inputMode = 'w';
					mainptrs->running = false;
					break;
			}
		} else {
			mainptrs->flist[++mainptrs->fileNr].fp = fopen(argv[i], "r");
			if(mainptrs->flist[mainptrs->fileNr].fp == NULL){
				printf("Could not open script '%s'.\n", argv[i]);
				mainptrs->fileNr--;
				mainptrs->inputMode = 'w';
				mainptrs->running = !mainptrs->running;
			} else {
				mainptrs->inputMode = 'f';
				mainptrs->flist[mainptrs->fileNr].rdpos = 0;
				mainptrs->flist[mainptrs->fileNr].lineNr = 0;
				mainptrs->flist[mainptrs->fileNr].begin_time = time(&mainptrs->flist[mainptrs->fileNr].begin_time);
				mainptrs->running = !mainptrs->running;
			}
		}
	}
	return 0;
}

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
		if(mainptrs->debug == 'v') printf("On position %d, entry: %c\n", mainptrs->readhead, entry);

		if(entry != ' ' && entry != '\t' && entry != '\0' && entry != '\n' && entry != '\r' && entry != ';'){
//			functions
			if(mainptrs->lookingMode == 'i'){

				for(mainptrs->libNr = 0; mainptrs->libNr < libAmount && mainptrs->instructNr == -1; mainptrs->libNr++){
					instructhandlers[mainptrs->libNr](mainptrs);
				}

				if(mainptrs->libNr == libAmount && mainptrs->instructNr == -1){
					printf("Invalid instruction on line %d", mainptrs->flist[mainptrs->fileNr].lineNr);
					if(mainptrs->inputMode == 'f'){
						printf(", in file %d:\n", mainptrs->fileNr);
						printf("'%s'\n", mainptrs->userInput);
					} else printf(".\n");
					mainptrs->lookingMode = 'd';
					mainptrs->inputMode -= 0x20;
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
		mainptrs->readhead++;
		if(mainptrs->readhead >= mainptrs->userInputLen){
			printf("The statement on line %d ", mainptrs->flist[mainptrs->fileNr].lineNr);
			if(mainptrs->inputMode == 'f'){
				printf("in file %d ", mainptrs->fileNr);
			}
			printf("is too long (maximum is %u characters).\nIt is possible to change the maximum by modifying the `inplen` register.\n", mainptrs->userInputLen);
			mainptrs->lookingMode = 'd';
			mainptrs->inputMode = 'W';
		}
	}

	if(mainptrs->lookingMode != 'd') executehandlers[mainptrs->libNr](mainptrs);

	if(mainptrs->debug == 'v') printf("thing done\n");
	return 0;
}

int getuserInput(GLOBAL* mainptrs){
	file_t* thefile = &mainptrs->flist[mainptrs->fileNr];
	time_t end_time;
	switch (mainptrs->inputMode){
		case 'I':
			mainptrs->inputMode = 'i';
		case 'i':
			printf("\\\\\\ ");
			fgets(mainptrs->userInput, mainptrs->userInputLen, thefile->fp);
			mainptrs->userInput[mainptrs->userInputLen] = '\0';
			break;

		case 'F':
			mainptrs->userInput[0] = '\0';
			fclose(thefile->fp);
			mainptrs->fileNr--;
			if(mainptrs->fileNr == 0) mainptrs->inputMode = 'i';
			break;
		case 'f':			
			thefile->lineNr++;
			if(fgets(mainptrs->userInput, mainptrs->userInputLen, thefile->fp) == NULL){
				thefile->lineNr = 0;
				mainptrs->userInput[0] = '\0';
				if(mainptrs->scriptLoops == 1){
					fseek(thefile->fp, 0, SEEK_SET);
				} else {
					fclose(thefile->fp);
					mainptrs->fileNr--;
					end_time = time(&end_time);
					inttonum(&regs[ptme], (int64_t) end_time - thefile->begin_time);
					if(mainptrs->fileNr == 0) mainptrs->inputMode = 'i';
				}
			}
			mainptrs->userInput[mainptrs->userInputLen] = '\0';
			break;

		case 'W':
			fgets(mainptrs->userInput, mainptrs->userInputLen, thefile->fp);
			mainptrs->userInput[0] = '\0';
		case 'w':
			mainptrs->inputMode = 'i';
			break;
	}
	if(mainptrs->debug == 'v') printf("userInput gotten\n");
	return 0;
}
//############################################### </Main operation>

int main(int argc, char* argv[]){
	GLOBAL mainitems;
	initLibFuncPtrs();

	for(int i = 0; i < libAmount; i++)
		initfuncs[i](&mainitems);
	handlecommandlineargs(argc, argv, &mainitems);

	if(mainitems.inputMode == 'i')
		printf("Good to see you!\nEnter `h` for quick tips and `\\` to close the program.\n");

	do {
		getuserInput(&mainitems);
		dothing(&mainitems);

		for(int i = 0; i < libAmount; i++)
			updatefuncs[i](&mainitems);

		if(mainitems.debug == 'v') printf("end of main while loop\n");
	} while(mainitems.running || mainitems.inputMode == 'f');

	for(int i = 0; i < libAmount; i++)
		freefuncs[i](&mainitems);
	return 0;
}

#endif
