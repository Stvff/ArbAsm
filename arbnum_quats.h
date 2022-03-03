#ifndef ARBQUATERNIONS_H
#define ARBQUATERNIONS_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "arbnum.h"

enum basequats {re=0, im, jm, km};

typedef struct Quaternion {
	num_t q[4];
} qua_t;

void initquat(qua_t* quat){
	for(int p = 0; p < 4; p++){
		initnum(&quat->q[p], 1, 0, p);
		quat->q[p].nump[0] = 0;
	}
}

void initquatarray(int size, qua_t* quatarray){
	for(int i = 0; i < size; i++)
		initquat(&quatarray[i]);
}

void freequat(qua_t* quat){
	for(int p = 0; p < 4; p++)
		free(quat->q[p].nump);
}

void freequatarray(int size, qua_t* quatarray){
	for(int i = 0; i < size; i++)
		freequat(&quatarray[i]);
}

void setquat(qua_t* quat, num_t* real, num_t* imag, num_t* jmag, num_t* kmag){
	copynum(&quat->q[re], real, false);
	copynum(&quat->q[im], imag, false);
	copynum(&quat->q[jm], jmag, false);
	copynum(&quat->q[km], kmag, false);
}

void copyquat(qua_t* des, qua_t* src, bool keeplen){
	copynum(&des->q[re], &src->q[re], keeplen);
	copynum(&des->q[im], &src->q[im], keeplen);
	copynum(&des->q[jm], &src->q[jm], keeplen);
	copynum(&des->q[km], &src->q[km], keeplen);
}

void printquat(qua_t* quat, int isbigend){
	printnum(&quat->q[re], isbigend + 2);
	printf(" + ");
	printnum(&quat->q[im], isbigend + 2);
	printf(" + ");
	printnum(&quat->q[jm], isbigend + 2);
	printf(" + ");
	printnum(&quat->q[km], isbigend + 2);
	printf("\n");
}

void sumquat(qua_t* res, qua_t* arg1, qua_t* arg2, bool subtract){
	qua_t dummy;
	initquat(&dummy);
	for(int p = 0; p < 4; p++){
		sumnum(&dummy.q[arg1->q[p].dim], &dummy.q[arg1->q[p].dim], &arg1->q[p], subtract);
		sumnum(&dummy.q[arg2->q[p].dim], &dummy.q[arg2->q[p].dim], &arg2->q[p], subtract);
	}

	copyquat(res, &dummy, false);
	freequat(&dummy);
}

void multquat(qua_t* res, qua_t* arg1, qua_t* arg2){
	qua_t dummy1;
	initquat(&dummy1);
	qua_t dummy2;
	initquat(&dummy2);
	for(int p1 = 0; p1 < 4; p1++){
		for(int p2 = 0; p2 < 4; p2++){
			multnum(&dummy2.q[p2], &arg1->q[p1], &arg2->q[p2]);
		}
		sumquat(&dummy1, &dummy1, &dummy2, false);
	}

	copyquat(res, &dummy1, false);
	freequat(&dummy1);
	freequat(&dummy2);
}
#endif
