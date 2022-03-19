#ifndef QUATLIB
#define QUATLIB
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include "aasm_global.h"
#include "arbnum.h"
#include "arbnum_stdlib.h"
#include "arbquats.h"

//############################################### <Quats globals>
qua_t* quargs[maxArgumentAmount];
qua_t quatmps[maxArgumentAmount];
int dimNr;
int prevargNr;

enum quatregisters {
	qr1, qr2, qr3, qr4, qans,
	quatregAmount
};
qua_t quatregs[quatregAmount];

enum instructquatlib {
	qset, qprint, qadd, qsub, qmul, qdiv
};

char quatregisterstring[][maxKeywordLen] = {
	"qr1", "qr2", "qr3", "qr4", "qns",
	"\0end"
};

char quatinstructstring[][maxKeywordLen] = {
	"qset", "qprint", "qadd", "qsub", "qmul", "qdiv",
	"\0end"
};
//############################################### </Quats globals>

//############################################### <Quats surroundings>
int init_quats(GLOBAL* mainptrs){
	dimNr = 0;
	prevargNr = 0;
	initquatarray(quatregAmount, quatregs);
	for(int i = 0; i < maxArgumentAmount; i++){
		initquat(&quatregs[i]);
		initquat(&quatmps[i]);
		quargs[i] = &quatmps[i];
	}

	if(mainptrs->debug == 'v') printf("quats initted\n");
	return 0;
}

int update_quats(GLOBAL* mainptrs){
	dimNr = 0;
	prevargNr = 0;
	for(int i = 0; i < maxArgumentAmount; i++){
		freequat(&quatmps[i]);
		initquat(&quatmps[i]);
		quargs[i] = &quatmps[i];
	}

	if(mainptrs->debug == 'v') printf("quats updated\n");
	return 0;
}

int free_quats(GLOBAL* mainptrs){
	freequatarray(quatregAmount, quatregs);
	for(int i = 0; i < maxArgumentAmount; i++){
		freequat(&quatmps[i]);
	}

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
	int qregisterNr = -1;
	int customreadhead = mainptrs->readhead;
	mainptrs->lookingMode = 'a';

	if(prevargNr != mainptrs->argumentNr){ dimNr = 0; prevargNr = mainptrs->argumentNr; }
	if(mainptrs->userInput[mainptrs->readhead] == '+') dimNr++;

	else {
		qregisterNr = strlook(mainptrs->userInput, quatregisterstring, &customreadhead);
		if(qregisterNr == -1){
			argumenthandler_std(mainptrs);
			copynum(&quargs[mainptrs->argumentNr]->q[dimNr % 4], args[mainptrs->argumentNr], 0);
		} else {
			mainptrs->readhead = customreadhead;
			quargs[dimNr] = &quatregs[qregisterNr];
		}
	}

	if(mainptrs->debug == 'v') printf("quats argumented\n");
	return 0;
}

int executehandler_quats(GLOBAL* mainptrs){
	time_t begin_time = time(&begin_time);
	bool doprint = true;
	if(mainptrs->inputMode != 'i') doprint = false;
	qua_t* printptr = quargs[0];

	switch(mainptrs->instructNr){
		case qset:
			copyquat(quargs[0], quargs[1], 0);
			break;
		case qprint:
			doprint = true;
			break;
		case qadd:
			sumquat(quargs[0], quargs[0], quargs[1], false);
			break;
		case qsub:
			sumquat(quargs[0], quargs[0], quargs[1], true);
			break;
		case qmul:
			multquat(quargs[0], quargs[0], quargs[1]);
			break;
		case qdiv:
			printf("qdiv not yet implented\n");
			doprint = false;
			break;
	}

	if(doprint){
		printquat(printptr, mainptrs->bigEndian);
		copyquat(&quatregs[qans], printptr, 0);
	}

	time_t end_time = time(&end_time);
	inttonum(&regs[ptme], (int64_t) end_time - begin_time);

	if(mainptrs->debug == 'v') printf("quats executed\n");
	return 0;
}
//############################################### </Quats surroundings>

#endif
