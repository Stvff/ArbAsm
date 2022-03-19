#ifndef QUATLIB
#define QUATLIB
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "aasm_global.h"
#include "arbnum.h"
#include "arbnum_stdlib.h"
#include "arbquats.h"

//############################################### <Quats globals>
qua_t* quargs[maxArgumentAmount];
qua_t quatmps[maxArgumentAmount];

enum quatregisters {
	qr1, qr2, qr3, qr4,
	quatregAmount
};
qua_t quatregs[quatregAmount];

enum instructquatlib {
	qset, qprint, qadd, qsub, qmul, qdiv
};

char quatregisterstring[][maxKeywordLen] = {
	"qr1", "qr2", "qr3", "qr4",
	"\0end"
};

char quatinstructstring[][maxKeywordLen] = {
	"qset", "qprint", "qadd", "qsub", "qmul", "qdiv",
	"\0end"
};
//############################################### </Quats globals>

//############################################### <Quats surroundings>
int init_quats(GLOBAL* mainptrs){
	initquatarray(quatregAmount, quatregs);
	initquatarray(maxArgumentAmount, quatmps);

	if(mainptrs->debug == 'v') printf("quats initted\n");
	return 0;
}

int update_quats(GLOBAL* mainptrs){
	freequatarray(maxArgumentAmount, quatmps);
	initquatarray(maxArgumentAmount, quatmps);

	if(mainptrs->debug == 'v') printf("quats updated\n");
	return 0;
}

int free_quats(GLOBAL* mainptrs){
	freequatarray(maxArgumentAmount, quatmps);
	freequatarray(quatregAmount, quatregs);

	if(mainptrs->debug == 'v') printf("quats freed\n");
	return 0;
}

int instructhandler_quats(GLOBAL* mainptrs){
	mainptrs->instructNr = strlook(mainptrs->userInput, quatinstructstring, &mainptrs->readhead);
	mainptrs->lookingMode = 'a';

	if(mainptrs->debug == 'v') printf("quats instructed\n");
	return 0;
}

int argumenthandler_quats(GLOBAL* mainptrs){
	mainptrs->lookingMode = 'e';

	if(mainptrs->debug == 'v') printf("quats argumented\n");
	return 0;
}

int executehandler_quats(GLOBAL* mainptrs){
	switch(mainptrs->instructNr){
		case qset:
			break;
		case qprint:
			break;
		case qadd:
			break;
		case qsub:
			break;
		case qmul:
			break;
		case qdiv:
			break;
	}
	printf("%s\n", quatinstructstring[mainptrs->instructNr]);

	if(mainptrs->debug == 'v') printf("quats executed\n");
	return 0;
}
//############################################### </Quats surroundings>

#endif
