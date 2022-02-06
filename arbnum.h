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

void copynum(num_t* des, num_t* src){
	free(des->nump);
	initnum(des, src->len, src->dim);
	des->overflow = src->overflow;
	for(unsigned int i = 0; i < src->len; i++){
		des->nump[i] = src->nump[i];
	}
}

void incnum(num_t* num){
	num_t dummy; initnum(&dummy, num->len + 1, 0);
	num->len++;
	copynum(&dummy, num);
	dummy.nump[dummy.len-1] = 0;
	num->len--;
	
	dummy.nump[0]++;
	unsigned int i = 0;
	while (psi(10, 1, dummy.nump[i]) != 0 && i < num->len){
		dummy.nump[i+1] += psi(10, 1, dummy.nump[i]);
		dummy.nump[i] = psi(10, 0, dummy.nump[i]);
		i++;
	}
	if(i == num->len) copynum(num, &dummy);
	else {
		dummy.len--;
		copynum(num, &dummy);
	}
	free(dummy.nump);
}

void addnum(num_t* res, num_t* arg1, num_t* arg2){
	num_t* tempformax = arg2;
	if(arg1->len < arg2->len){ arg2 = arg1; arg1 = tempformax;}//arg1 is now always the longest (not nessecarily the largest)
	num_t dummy; initnum(&dummy, arg1->len + 1, 0);
	arg1->len++;
	copynum(&dummy, arg1);
	dummy.nump[dummy.len-1] = 0;
	arg1->len--;

	for(unsigned int i = 0; i < arg1->len; i++){
		if(i < arg2->len) dummy.nump[i] += arg2->nump[i];
		dummy.nump[i+1] += psi(10, 1, dummy.nump[i]);
		dummy.nump[i] = psi(10, 0, dummy.nump[i]);
	}

	if(dummy.nump[dummy.len-1] != 0) copynum(res, &dummy);
	else {
		dummy.len--;
		copynum(res, &dummy);
	}
	free(dummy.nump);
}

void multnum(num_t* res, num_t* arg1, num_t* arg2){
	printf("multnum is not yet implemented.\n");
}
