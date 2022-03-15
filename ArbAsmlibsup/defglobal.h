#ifndef GLOBAL_DEF
#define GLOBAL_DEF
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#define maxKeywordLen 10

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
	int scriptLoops;
	char mode;
	// 'i' is from stdin
	// 'f' is from file
	// 'F' is from a file in deep

	int bigEndian;
	
	int recDep;
	int fileNr;
	file_t* flist;

	char debug;
	// 'o' is off
	// 'v' is verbose
} GLOBAL;

typedef int (*fun)(GLOBAL*);

int strlook(char string[], char source[][maxKeywordLen], int offset, int* lengthoflocated){
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

#endif
