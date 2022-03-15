#ifndef GLOBAL_DEF
#define GLOBAL_DEF
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

typedef struct FileInfo {
	FILE* fp;
	int rdpos;
	int linenr;
	time_t begin_time;
} file_t;

typedef struct __global__ {
	char* userInput;
	int userInputLen;
	int readhead;

	char lookingMode;
	// 'i' is for instruction mnemonics
	// 'a' is for arguments

	bool running;
	char mode;
	// 'i' is from stdin
	// 'f' is from file
	// 'F' is from a file in deep

	int bigEndian;
	
	int recDep;
	int fileNr;
	file_t* flist;
} GLOBAL;

typedef int (*fun)(GLOBAL*);

#endif
