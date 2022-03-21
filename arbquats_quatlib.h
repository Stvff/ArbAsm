#ifndef QUATLIB
#define QUATLIB 2
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
enum typesquats { stdType = String, Quater };

enum quatregisters {
	qr1, qr2, qr3, qr4, qns,
	quatregAmount
};
qua_t quatregs[quatregAmount];

enum instructquatlib {
	qset, qprint, qdset, qdget,
	qadd, qsub, scmul, scdiv, scmod,
	qmul, qdiv, qmod,
	conj, pysq,
	qpush, qpop, qflip, qret
};

char quatregisterstring[][maxKeywordLen] = {
	"qr1", "qr2", "qr3", "qr4", "qns",
	"\0end"
};

char quatinstructstring[][maxKeywordLen] = {
	"qset", "qprint", "qdset", "qdget",
	"qadd", "qsub", "scmul", "scdiv", "scmod",
	"qmul", "qdiv", "qmod",
	"conj", "pysq",
	"qpush", "qpop", "qflip", "qret",
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
			quargs[mainptrs->argumentNr] = &quatregs[qregisterNr];
		}
	}

	if(mainptrs->debug == 'v') printf("quats argumented\n");
	return 0;
}

int executehandler_quats(GLOBAL* mainptrs){
	time_t begin_time = time(&begin_time);
	bool doprint = true;
	if(mainptrs->inputMode != 'i') doprint = false;
	qua_t* printqptr = quargs[0];
	num_t* printnptr = args[0];
	int rettype = Quater;

	switch(mainptrs->instructNr){
		case qset:
			copyquat(quargs[0], quargs[1], 0);
			break;
		case qprint:
			doprint = true;
			break;
		case qdset:
			copynum(&quargs[0]->q[numtoint(args[1], 0) % 4], args[2], 0);
			break;
		case qdget:
			copynum(args[2], &quargs[0]->q[numtoint(args[1], 0) % 4], 0);
			printnptr = args[2];
			rettype = Number;
			break;
		case qadd:
			sumquat(quargs[0], quargs[0], quargs[1], false);
			break;
		case qsub:
			sumquat(quargs[0], quargs[0], quargs[1], true);
			break;
		case scmul:
			scalarmultquat(quargs[0], quargs[0], args[1]);
			break;
		case scdiv:
			scalardivquat(quargs[0], quargs[2], quargs[0], args[1]);
			break;
		case scmod:
			scalardivquat(quargs[2], quargs[0], quargs[0], args[1]);
			break;
		case qmul:
			multquat(quargs[0], quargs[0], quargs[1]);
			break;
		case qdiv:
			divquat(quargs[0], quargs[2], quargs[0], quargs[1]);
			break;
		case qmod:
			divquat(quargs[2], quargs[0], quargs[0], quargs[1]);
			break;
		case conj:
			conjugate(quargs[0]);
			break;
		case pysq:
			pythsquared(args[0], quargs[1]);
			rettype = Number;
			break;
		case qpush:
			if(!pushtomst(&quargs[0]->q[3])){ doprint = false; break;}
			if(!pushtomst(&quargs[0]->q[2])){ doprint = false; break;}
			if(!pushtomst(&quargs[0]->q[1])){ doprint = false; break;}
			if(!pushtomst(&quargs[0]->q[0]))  doprint = false;
			break;
		case qpop:
			if(!popfrommst(&quargs[0]->q[0])){ doprint = false; break;}
			if(!popfrommst(&quargs[0]->q[1])){ doprint = false; break;}
			if(!popfrommst(&quargs[0]->q[2])){ doprint = false; break;}
			if(!popfrommst(&quargs[0]->q[3]))  doprint = false;
			break;
		case qflip:
			for(int p = 0; p < 4; p++){
				if(!fliperonie()){ doprint = false; break;}
				if(doprint) copynum(&quatregs[qns].q[p], &retstack[retstackptr], 0);
			}
			printqptr = &quatregs[qns];
			break;
		case qret:
			for(int p = 0; p < 4; p++){
				if(!reteronie()){doprint = false; break;}
				copynum(&quatregs[qns].q[3 - p], &stack[stackptr], 0);
			}
			printqptr = &quatregs[qns];
			break;
	}

	if(doprint) switch(rettype){
		case Quater:
			printquat(printqptr, mainptrs->bigEndian);
			copyquat(&quatregs[qns], printqptr, 0);
			break;
		case Number:
			printnum(printnptr, mainptrs->bigEndian);
			copynum(&regs[ans], printnptr, 0);
			break;
	};

	time_t end_time = time(&end_time);
	inttonum(&regs[ptme], (int64_t) end_time - begin_time);

	if(mainptrs->debug == 'v') printf("quats executed\n");
	return 0;
}

int initLibFuncPtrs_2(){
	initfuncs[QUATLIB] = &init_quats;
	instructhandlers[QUATLIB] = &instructhandler_quats;
	argumenthandlers[QUATLIB] = &argumenthandler_quats;
	executehandlers[QUATLIB] = &executehandler_quats;
	updatefuncs[QUATLIB] = &update_quats;
	freefuncs[QUATLIB] = &free_quats;

	return 0;
}
//############################################### </Quats surroundings>

#endif
