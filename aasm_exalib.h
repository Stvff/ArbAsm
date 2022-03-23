// This is the template for everything a library needs
#ifndef AASM_EXALIB
#define AASM_EXALIB 3
#include <stdio.h>
#include "aasm_global.h"

//############################################### <example globals>
//############################################### </example globals>
//############################################### <example internal functions>
//############################################### </example internal functions>
//############################################### <example surroundings>
int init_example(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("exalib initted\n");
	return 0;
}

int update_example(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("exalib updated\n");
	return 0;
}

int free_example(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("exalib freed\n");
	return 0;
}

int instructhandler_example(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("exalib instructed\n");
	return 0;
}

int argumenthandler_example(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("exalib argumented\n");
	return 0;
}

int executehandler_example(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("exalib executed\n");
	return 0;
}

int libFuncPtrs_AASM_EXALIB(){
	initfuncs[AASM_EXALIB] = &init_example;
	instructhandlers[AASM_EXALIB] = &instructhandler_example;
	argumenthandlers[AASM_EXALIB] = &argumenthandler_example;
	executehandlers[AASM_EXALIB] = &executehandler_example;
	updatefuncs[AASM_EXALIB] = &update_example;
	freefuncs[AASM_EXALIB] = &free_example;

	return 0;
}
//############################################### </example surroundings>

#endif
