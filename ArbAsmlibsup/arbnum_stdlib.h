#ifndef ARBNUM_STDLIB
#define ARBNUM_STDLIB
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include "defglobal.h"
#include "arbnum.h"

enum CompiletimeConstantsStdlib {
	stackSize = 20,
	regAmount = 14,
};

num_t* stack;
int stackptr;
num_t* retstack;
int retstackptr;

int regamount = 14;
num_t regs[14];
enum registers {
	gr1, gr2, gr3, gr4, ir, flag,
	inplen, endian, stacsz, mstptr,
	rstptr, tme, ptme, loop
};

enum instructs {
	set, dset, dget, rev, sel, cton, ntoc,
	inc, dec, add, sub, mul, divi, modu,
	cmp, ucmp, rot, shf, app, len, 
	print, sprint, nprint, draw,
	firead, fiwrite, input, sinput,
	push, pop, peek, flip, ret,
	SCR, SAVE, LOAD, Ce, Cg, Cs
};

char registerstring[][10] = { 
	"gr1", "gr2", "gr3", "gr4", "ir",
	"flag", "inplen", "endian", "stacsz", "mstptr", "rstptr",
	"time", "ptime", "loop",
	"\0end"
};

char instructstring[][10] = {
	"set", "dset", "dget", "rev", "sel", "cton", "ntoc",
	"inc", "dec", "add", "sub", "mul", "div", "mod",
	"cmp", "ucmp", "rot", "shf", "app", "len",
	"print", "sprint", "nprint", "draw",
	"fread", "fwrite", "input", "sinput",
	"push", "pop", "peek", "flip", "ret", 
	"SCR", "SAVE", "LOAD", "Ce", "Cg", "Cs",
	"\0end"
};


char(*)[][] returnfuncstrstd(){
	return &instructstring;
}

char* returnargstrstd

int initstd(GLOBAL* mainptrs){
	return 0;
}

int updatestd(GLOBAL* mainptrs){
	return 0;
}

int freestd(GLOBAL* mainptrs){
	return 0;
}

int instructionsstd(GLOBAL* mainptrs){
	return 0;
}

int argumentsstd(GLOBAL* mainptrs){
	return 0;
}

#endif
