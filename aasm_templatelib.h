// This is the template for everything a library needs
#ifndef AASM_TEMPLATELIB
#define AASM_TEMPLATELIB 3
#include <stdio.h>
#include "aasm_global.h"

//############################################### <template globals>
//############################################### </template globals>
//############################################### <template internal functions>
//############################################### </template internal functions>
//############################################### <template essentials>
int init_template(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("templatelib initted\n");
	return 0;
}

int update_template(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("templatelib updated\n");
	return 0;
}

int free_template(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("templatelib freed\n");
	return 0;
}

int instructhandler_template(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("templatelib instructed\n");
	return 0;
}

int argumenthandler_template(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("templatelib argumented\n");
	return 0;
}

int executehandler_template(GLOBAL* mainptrs){

	if(mainptrs->debug == 'v') printf("templatelib executed\n");
	return 0;
}

int libFuncPtrs_AASM_TEMPLATELIB(){
	initfuncs[AASM_TEMPLATELIB] = &init_template;
	instructhandlers[AASM_TEMPLATELIB] = &instructhandler_template;
	argumenthandlers[AASM_TEMPLATELIB] = &argumenthandler_template;
	executehandlers[AASM_TEMPLATELIB] = &executehandler_template;
	updatefuncs[AASM_TEMPLATELIB] = &update_template;
	freefuncs[AASM_TEMPLATELIB] = &free_template;

	return 0;
}
//############################################### </example essentials>

#endif
