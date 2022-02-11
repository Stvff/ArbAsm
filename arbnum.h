#ifndef ARBNUM_H
#define ARBNUM_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct Number {
	int dim;
	unsigned int len;
	uint32_t* nump;
	int overflow;
} num_t;

int psi(int g,int h,int n){
	return (n % (int)pow((float)g, (float)(h+1)))/(int)pow(g,h);
}

uint32_t* initnum(num_t* number, unsigned int length, int sign){
	number->dim = sign;
	number->len = length;
	number->nump = (uint32_t*) malloc(number->len*sizeof(uint32_t));
	if(number->nump == NULL){
		printf("Malloc error on initnum: %lu\n", (unsigned long int)number->nump);
		number->overflow = 1;
	}
	number->overflow = 0;
	return number->nump;
}

void copynum(num_t* des, num_t* src, int keeplen){
	unsigned int len = src->len*(1-keeplen) + des->len*keeplen;
	unsigned int otherlen = src->len*keeplen + des->len*(1-keeplen);
	free(des->nump);
	initnum(des, len, src->dim);
	des->overflow = src->overflow;
	for(unsigned int i = 0; i < len; i++){
		if(keeplen == 0) des->nump[i] = src->nump[i];
		else if (i < otherlen) des->nump[i] = src->nump[i];
		else des->nump[i] = 0;
	}
}

void printnum(num_t* number, int isbigend){
	for(int i = isbigend*((int)number->len-1); i != (1-isbigend)*(int)number->len - isbigend; i+= (1 - 2*isbigend))
		printf("%u ", number->nump[i]);
	/*switch (number->dim - (number->dim % 2)){
		case 2: printf("i"); break;
		case 4: printf("j"); break;
		case 6: printf("k"); break;
	} if (number->dim % 2 == 1) printf("-"); else printf("+");*/
	printf("\n");
}

int numtoint(num_t* num){
	int cum = 0;
	int pow = 1;
	for(unsigned int i = 0; i < num->len; i++){
		cum += num->nump[i]*pow;
		pow *= 10;
	}
	return cum;
}

void inttonum(num_t* num, int inte){
	free(num->nump);
	if(inte == 0) initnum(num, 1, 0);
	else initnum(num, 1+(uint32_t)(log((double)inte)/log(10.0)), 0);
	for(unsigned int i = 0; i < num->len; i++){
		num->nump[i] = psi(10, i, inte);
	}
}

int inpstrtonum(num_t* number, char str[], int offset, int isbigend){
	int i = offset;
	int j = 0;
	while(str[i] >= '0' && str[i] <= '9'){
		j++;i++;
	}
	free(number->nump);
	number->len = j;
	number->nump = (uint32_t*) malloc(j*sizeof(uint32_t));
	if(number->nump == NULL) printf("There is a malloc problem on inpstrtonum: %lu\n", (unsigned long int)number->nump);

	int signage = 0;
	switch(str[i]){
		case 'i':
			number->dim = 2;
			i++; signage++;
			break;
		case 'j':
			number->dim = 4;
			i++; signage++;
			break;
		case 'k':
			number->dim = 6;
			i++; signage++;
			break;
	}
	if(str[i] == '-'){ number->dim++; signage++;}
	else if(str[i] == '+') signage++;	

	int dirio = 1 - 2*isbigend;
	i = offset + (j-1)*(isbigend);
	j = 0;

	while(str[i] >= '0' && str[i] <= '9'){
		number->nump[j] = (int) str[i] - 0x30;
		j++; i+= dirio;
	}
	return j + signage;
}

void incnum(num_t* num){
	num_t dummy; initnum(&dummy, num->len + 1, 0);
	copynum(&dummy, num, 1);
	
	dummy.nump[0]++;
	unsigned int i = 0;
	while (psi(10, 1, dummy.nump[i]) != 0 && i < num->len){
		dummy.nump[i+1] += psi(10, 1, dummy.nump[i]);
		dummy.nump[i] = psi(10, 0, dummy.nump[i]);
		i++;
	}
	if(i == num->len) copynum(num, &dummy, 0);
	else {
		copynum(num, &dummy, 1);
	}
	free(dummy.nump);
}

/*void decnum(num_t* num){
	num_t dummy; initnum(&dummy, num->len + 1, 0);
	copynum(&dummy, num, 1);
	
}*/

unsigned int cmpnum(num_t* arg1, num_t* arg2, bool considersign){//returns 0 for arg1 = arg2, 1 for arg1 > arg2, 2 for arg1 < arg2
	unsigned int result = 0;
	uint32_t num1 = 0;
	uint32_t num2 = 0;
	unsigned int maxlen = arg1->len;
	if(arg2->len > maxlen) maxlen = arg2->len;
	for(int i = (int)maxlen - 1; i >= 0; i--){
		if(i < (int)arg1->len) num1 = arg1->nump[i];
		if(i < (int)arg2->len) num2 = arg2->nump[i];
		if(num1 > num2){ result = 1; break;}
		if(num1 < num2){ result = 2; break;}
	}
	if(considersign){
		if(arg1->dim % 2 < arg2->dim % 2)
			result = 1;
		else if(arg1->dim % 2 > arg2->dim % 2)
			result = 2;
		else if(arg1->dim % 2 == 1 && result != 0)
			result = 3 - result;
	}
	return result;
}

void addnum(num_t* res, num_t* arg1, num_t* arg2){
	num_t* tempformax = arg2;
	if(arg1->len < arg2->len){ arg2 = arg1; arg1 = tempformax;}//arg1 is now always the longest (not nessecarily the largest)
	num_t dummy; initnum(&dummy, arg1->len + 1, 0);
	copynum(&dummy, arg1, 1);

	for(unsigned int i = 0; i < arg1->len; i++){
		if(i < arg2->len) dummy.nump[i] += arg2->nump[i];
		dummy.nump[i+1] += psi(10, 1, dummy.nump[i]);
		dummy.nump[i] = psi(10, 0, dummy.nump[i]);
	}

	if(dummy.nump[dummy.len-1] != 0) copynum(res, &dummy, 0);
	else {
		copynum(res, &dummy, 1);
	}
	free(dummy.nump);
}

void multnum(num_t* res, num_t* arg1, num_t* arg2){
	num_t* tempformax = arg2;
	if(arg1->len < arg2->len){ arg2 = arg1; arg1 = tempformax;}//arg1 is now always the longest (not nessecarily the largest)
	num_t dummy; initnum(&dummy, arg1->len + arg2->len, 0);
	for(unsigned int i = 0; i < dummy.len; i++)
		dummy.nump[i] = 0;

	unsigned int j = 0;
	unsigned int maxj = arg1->len;
	for(unsigned int z = 0; z < arg2->len; z++){
		for(unsigned int i = 0; i < arg1->len; i++){
			dummy.nump[i + z] += arg1->nump[i]*arg2->nump[z];
			j = i + z;
			while(psi(10, 1, dummy.nump[j]) != 0 && j < dummy.len){
				dummy.nump[j+1] += psi(10, 1, dummy.nump[j]);
				dummy.nump[j] = psi(10, 0, dummy.nump[j]);
				j++;
			}
			if(j + 1 > maxj) maxj = j + 1;
		}
	}

	dummy.len = maxj;
	copynum(res, &dummy, 0);

	free(dummy.nump);
}

#endif
