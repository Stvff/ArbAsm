#ifndef GLOBAL_DEF
#define GLOBAL_DEF
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>

#define maxKeywordLen 10
#define maxArgumentAmount 4
#define libAmount 3
#define initialuserInputLen 256

void printversion(){
	printf("\n");
//	printf("        Arbitrary Assembly Vga    \n");
	printf("      Arbitrary Assembly pVga.qa   \n");
	printf("              x-------x            \n");
	printf("              | A r b |            \n");
	printf("              | A s m |            \n");
//	printf("              | V g a |            \n");
	printf("              |pVga.qa|            \n");
	printf("              x-------x            \n");
	printf("   github.com/StevenClifford/ArbAsm\n\n");
	printf("   Libraries:\n");
	printf("             stdlib\n");
	printf("             quatlib\n");
	printf("\n");
}

typedef struct FileInfo {
	FILE* fp;
	char* mfp;
	int64_t len;
	int64_t pos;
	int lineNr;
	time_t begin_time;
	int64_t* labelposs;
	char (*labels)[maxKeywordLen];
} file_t;

typedef struct global {
	char* userInput;
	int userInputLen;
	int readhead;

	char lookingMode;
	// 'i' is for instruction mnemonics
	// 'a' is for arguments
	// 'd' is for done
	// 'e' is for execute
	int libNr;
	int instructNr;
	int argumentNr;

	bool running;
	int scriptLoops;
	char inputMode;
	// 'i' is from stdin
	// 'f' is from file
	// 'w' is there already was a command
	// if they are capitalized there was an error

	int bigEndian;
	
	int fileNr;
	file_t** flist;

	char debug;
	// 's' is silent
	// 'v' is verbose
} GLOBAL;

typedef int (*fun)(GLOBAL*);

fun initfuncs[libAmount];

fun instructhandlers[libAmount];
fun argumenthandlers[libAmount];

fun executehandlers[libAmount];

fun updatefuncs[libAmount];
fun freefuncs[libAmount];

int strlook(char string[], char source[][maxKeywordLen], int* readhead){
	int item = 0;
	int j;
	bool isthis;
	while (source[item][0] != '\0'){
		isthis = true;
		j = 0;
		while (source[item][j] != '\0'){
			if(string[j + *readhead] != source[item][j]) isthis = false;
			j++;
		}
		if(isthis){
			*readhead += --j;
			return item;
		}
		item++;
	}
	return -1;
}

char* mfgets(char* string, int size, file_t* file){
	int64_t i = 0;
	if(file->pos == file->len) return NULL;
	do {
		string[i] = file->mfp[file->pos + i];
		i++;
	} while(file->mfp[file->pos + i - 1] != '\n' && i <= size-1 && file->pos + i <= file->len);

	string[i] = '\0';
	file->pos += i;
//	printf("-------------------------------- %s\n", string);
	return string;
}

int RunScript(GLOBAL* mainptrs, char* name){
	mainptrs->fileNr++;

	mainptrs->flist = realloc(mainptrs->flist, (mainptrs->fileNr + 1)*sizeof(file_t*));

	mainptrs->flist[mainptrs->fileNr] = malloc(sizeof(file_t));

	mainptrs->flist[mainptrs->fileNr]->fp = fopen(name, "r");
	file_t* thefile = mainptrs->flist[mainptrs->fileNr];
	if(thefile->fp == NULL){
		printf("\aCould not open script '%s'.\n", name);
		free(mainptrs->flist[mainptrs->fileNr]);
		mainptrs->flist = realloc(mainptrs->flist, (mainptrs->fileNr)*sizeof(file_t*));
		mainptrs->fileNr--;
		return 1;
	}
	mainptrs->inputMode = 'f';
	thefile->pos = 0;
	thefile->lineNr = 0;
	thefile->begin_time = time(&thefile->begin_time);

	fseek(thefile->fp, 0, SEEK_END);
	thefile->len = ftell(thefile->fp);
	fseek(thefile->fp, 0, SEEK_SET);

	thefile->labelposs = NULL;

	thefile->mfp = (char*) malloc(sizeof(char)*(thefile->len + maxKeywordLen));
	fread(thefile->mfp, 1, thefile->len, thefile->fp);
	int labelam = 0;
	for(int i = 0; i < thefile->len;i++){
		if(thefile->mfp[i] == '\n' && thefile->mfp[i+1] == ':'){
			i += 2;
			thefile->labels = (char(*)[maxKeywordLen]) realloc(thefile->labels, sizeof(char[labelam+1][maxKeywordLen]));
			thefile->labelposs = (int64_t*) realloc(thefile->labelposs, sizeof(int64_t[labelam+1]));
			int j = 0;

			for(j = 0;
			j < maxKeywordLen && ((thefile->mfp[i+j] >= 'a' && thefile->mfp[i+j] <= 'z') || (thefile->mfp[i+j] >= 'A' && thefile->mfp[i+j] <= 'Z') || (thefile->mfp[i+j] >= '0' && thefile->mfp[i+j] <= '9'));
			j++)
			{
				thefile->labels[labelam][j] = thefile->mfp[i+j];
			}
			thefile->labels[labelam][j] = '\0';
			thefile->labelposs[labelam] = i + j;
//			printf("%s, %ld\n", thefile->labels[labelam], thefile->labelposs[labelam]);
			labelam++;
		}
	}

	thefile->labels = (char(*)[maxKeywordLen]) realloc(thefile->labels, sizeof(char[labelam+1][maxKeywordLen]));
	thefile->labels[labelam][0] = '\0';

	fseek(thefile->fp, 0, SEEK_SET);
	return 0;
}

void EndScript(GLOBAL* mainptrs){
	file_t* thefile = mainptrs->flist[mainptrs->fileNr];
	thefile->labels = (char(*)[maxKeywordLen]) realloc(thefile->labels, 1);

	free(thefile->labelposs);
	thefile->labelposs = NULL;

	thefile->lineNr = 0;
	mainptrs->userInput[0] = '\0';
	fclose(thefile->fp);
	free(thefile->mfp);

	free(mainptrs->flist[mainptrs->fileNr]);
	mainptrs->flist[mainptrs->fileNr] = NULL;
	mainptrs->flist = realloc(mainptrs->flist, (mainptrs->fileNr)*sizeof(file_t*));

	mainptrs->fileNr--;
	if(mainptrs->fileNr == 0) mainptrs->inputMode = 'i';
}

#endif
