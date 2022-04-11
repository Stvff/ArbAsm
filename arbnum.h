#ifndef ARBNUM_H
#define ARBNUM_H
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>

typedef struct Number {
	int32_t sign;
	int32_t dim;
	uint64_t len;
	uint8_t* nump;
} num_t;

int64_t psi(int64_t g,int64_t h,int64_t n){
	return (n % (int64_t)pow((float)g, (float)(h+1)))/(int64_t)pow((float)g,(float)h);
}

uint8_t* initnum(num_t* number, uint64_t length, int sign, int dim){
	number->sign = sign;
	number->dim = dim;
	number->len = length;
	number->nump = (uint8_t*) malloc((1 + number->len)*sizeof(uint8_t));
	number->nump[length] = 0;
	if(number->nump == NULL){
		printf("Malloc error on initnum.\n");
	}
	number->nump[number->len] = 0;
	return number->nump;
}

void initnumarray(int size, num_t* numarray, uint64_t length, int sign, int dim){
	for(int i = 0; i < size; i++)
		initnum(&numarray[i], length, sign, dim);
}

void freenumarray(int size, num_t* numarray){
	for(int i = 0; i < size; i++)
		free(numarray[i].nump);
}

void copynum(num_t* des, num_t* src, int keeplen){
	if(des == src) return;
	uint64_t len = src->len*(1-keeplen) + des->len*keeplen;
	uint64_t otherlen = src->len*keeplen + des->len*(1-keeplen);
	free(des->nump);
	initnum(des, len, src->sign, src->dim);
	for(uint64_t i = 0; i < len; i++){
		if(keeplen == 0) des->nump[i] = src->nump[i];
		else if (i < otherlen) des->nump[i] = src->nump[i];
		else des->nump[i] = 0;
	}
}

void printnum(num_t* number, int isbigend){//add 2 to isbigend to make it not newline
	bool donewline = true;
	if(isbigend > 1){ donewline = false; isbigend -= 2;}

	char* buffer = (char*) malloc((4 + number->len*2)*sizeof(char));
	int j = 0;
	for(int64_t i = isbigend*((int64_t)number->len-1); i != (1-isbigend)*(int64_t)number->len - isbigend; i+= (1 - 2*isbigend)){
		buffer[j] = 0x30 + number->nump[i];
		buffer[j + 1] = ' ';
		j += 2 ;
	}

	switch (number->dim){
		case 1: buffer[j] = 'i'; j++; break;
		case 2: buffer[j] = 'j'; j++; break;
		case 3: buffer[j] = 'k'; j++; break;
	} if (number->sign == 1){ buffer[j] = '-'; j++;}

	if(donewline){ buffer[j] = '\n'; j++;}
	buffer[j] = '\0';
	printf("%s", buffer);
	free(buffer);
}

int64_t numtoint(num_t* num, bool considersign){
	int64_t cum = 0;
	int64_t pow = 1;
	for(uint64_t i = 0; i < num->len; i++){
		cum += num->nump[i]*pow;
		pow *= 10;
	}
	if(considersign) cum *= 1 - 2*num->sign;
	return cum;
}

void inttonum(num_t* num, int64_t inte){
	free(num->nump);
	if(inte == 0) initnum(num, 1, 0, 0);
	else initnum(num, 1+(uint64_t)(log((double)inte)/log(10.0)), 0, 0);
	for(uint64_t i = 0; i < num->len; i++)
		num->nump[i] = psi(10, i, inte);
}

uint8_t numtouint8(num_t* num){
	uint8_t cum = 0;
	uint32_t pow = 1;
	num_t dummy; initnum(&dummy, 3, 0, 0);
	copynum(&dummy, num, true);
	for(uint64_t i = 0; i < 3; i++){
		cum += dummy.nump[i]*pow;
		pow *= 10;
	}
	free(dummy.nump);
	return cum;
}

void uint8tonum(num_t* num, uint8_t inte8){
	free(num->nump);
	initnum(num, 3, 0, 0);
	for(uint64_t i = 0; i < 3; i++)
		num->nump[i] = (uint8_t) psi(10, i, inte8);
}

int inpstrtonum(num_t* number, char str[], int offset, int isbigend){
	int i = offset;
	int j = 0;
	while((str[i] >= '0' && str[i] <= '9') || str[i] == ' '){
		if(str[i] != ' ') j++;
		i++;
	}
	free(number->nump);
	number->len = j;
	number->nump = (uint8_t*) malloc((1 + j)*sizeof(uint8_t));
	if(number->nump == NULL) printf("There is a malloc problem on inpstrtonum.\n");
	number->nump[j] = 0;

	int signage = 0;
	switch(str[i]){
		case 'i':
			number->dim = 1;
			signage++;
			break;
		case 'j':
			number->dim = 2;
			signage++;
			break;
		case 'k':
			number->dim = 3;
			signage++;
			break;
		default:
			number->dim = 0;
			break;
	}
	number->sign = 0;
	if(str[i + signage] == '-'){ number->sign = 1; signage++;}
//	else if(str[i + signage] == '+'){ number->sign = 0; signage++;}

	int dirio = 1 - 2*isbigend;
	j = 0;
	for(int inc = offset*(1 - isbigend) + (i-1)*(isbigend); inc != (offset-1)*(isbigend) + i*(1 - isbigend); inc += dirio){
		if(str[inc] != ' '){
			number->nump[j] = (int) str[inc] - 0x30;
			j++;
		}
	}
	return i + signage - offset;
}

int strtostrnum(num_t* number, char str[], int offset){
	int i = offset;
	int j = 0;
	int escapes = 0;
	while(str[i] != '"' && str[i] != '\0'){
		if(str[i] == '\\'){
			i += 2;
			j += 1;
			escapes++;
		} else {
			i += 1;
			j += 1;
		}
	}
	free(number->nump);
	number->len = j;
	number->nump = (uint8_t*) malloc((1 + j)*sizeof(uint8_t));
	if(number->nump == NULL) printf("There is a malloc problem on strtostrnum.\n");
	number->nump[j] = 0;

	number->sign = 0;
	number->dim = 0;

	j = 0;
	for(int n = offset; n < i; n++){
		if(str[n] == '\\'){
			switch (str[n+1]){
				case '0':
					number->nump[j] = '\0';
					break;
				case 'n':
					number->nump[j] = '\n';
					break;
				case 'r':
					number->nump[j] = '\r';
					break;
				case '\\':
					number->nump[j] = '\\';
					break;
				case '\"':
					number->nump[j] = '\"';
					break;
			}
			n++;
		} else number->nump[j] = str[n];
		j++;
	}
	return j + escapes + 1;
}

void printstrnum(num_t* number){
	char* buffer = (char*) malloc((1 + number->len)*sizeof(char));
	for(uint64_t i = 0; i < number->len; i++)
		buffer[i] = (char)number->nump[i];
	buffer[number->len] = '\0';
	printf("%s", buffer);
	free(buffer);
}

void savenum(FILE* fp, num_t* num){//File needs to be opened already!
	fwrite(num, 4, 3, fp);
	fwrite(num->nump, 1, num->len, fp);
}

void loadnum(FILE* fp, num_t* num){//File needs to be opened already!
	fread(num, 4, 3, fp);
	free(num->nump);
	initnum(num, num->len, num->sign, num->dim);
	fread(num->nump, 1, num->len, fp);
}

bool isnumzero(num_t* num){
	for(uint64_t i = num->len - 1; i != (uint64_t)-1; i--)
		if(num->nump[i] != 0) return false;
	return true;
}

void incnum(num_t* num, bool decrement){
	num_t dummy; initnum(&dummy, num->len + 1, 0, 0);
	copynum(&dummy, num, 1);

	bool iszero = isnumzero(&dummy);
	if(iszero && decrement && dummy.sign == 0) dummy.sign = 1;
	if(iszero && !decrement && dummy.sign == 1) dummy.sign = 0;

	uint8_t carry = 3 - 2*dummy.sign;
	if(decrement) carry = 1 + 2*dummy.sign;
	for(uint64_t i = 0; i < dummy.len; i++){
		dummy.nump[i] += 18 + carry;
		carry = psi(10, 1, dummy.nump[i]);
		dummy.nump[i] = psi(10, 0, dummy.nump[i]);
	}

	if(dummy.nump[dummy.len-1] != 0) copynum(num, &dummy, 0);
	else {
		copynum(num, &dummy, 1);
	}
	free(dummy.nump);
}

void rotnum(num_t* num, int64_t amount){
	num_t dummy; initnum(&dummy, 1, 0, 0);
	copynum(&dummy, num, 0);
	int64_t index;
	for(uint64_t i = 0; i < num->len; i++){
		index = amount + (int64_t)i;
		index = index - (int64_t)dummy.len*floor((float)index/(float)dummy.len);
		dummy.nump[index] = num->nump[i];
	}
	copynum(num, &dummy, 0);
	free(dummy.nump);
}

void shiftnum(num_t* num, int64_t amount){
	if(-1*amount >= (int64_t)num->len){
		free(num->nump);
		initnum(num, 1, num->sign, num->dim);
		num->nump[0] = 0;
		return;
	}
	num_t dummy;
	initnum(&dummy, num->len + amount, num->sign, num->dim);
	int64_t j = (int64_t)(num->len) - 1;
	for(int64_t i = (int64_t)dummy.len - 1; i >= 0; i--){
		if(j >= 0) dummy.nump[i] = num->nump[j];
		else dummy.nump[i] = 0;
		j--;
	}
	copynum(num, &dummy, 0);
	free(dummy.nump);
}

void appendnum(num_t* des, num_t* appendage){
	des->nump = (uint8_t*) realloc(des->nump, (des->len + appendage->len + 1)*sizeof(uint8_t));
	if(des->nump == NULL) printf("realloc error on appendnum!\n");
	for(uint64_t i = 0; i < appendage->len; i++)
		des->nump[i + des->len] = appendage->nump[i];
	des->len = des->len + appendage->len;
	des->nump[des->len] = 0;
}

void truncatenum(num_t* num){
	uint64_t lenohne0 = num->len;
	for(uint64_t i = 0; i < num->len; i++)
		if(num->nump[i] != 0) lenohne0 = i + 1;
	num_t dummy; initnum(&dummy, lenohne0, num->sign, num->dim);
	for(uint64_t i = 0; i < dummy.len; i++)
		dummy.nump[i] = num->nump[i];
	copynum(num, &dummy, false);
	free(dummy.nump);
}

void reversenum(num_t* num){
	num_t dummy; initnum(&dummy, num->len, num->sign, num->dim);
	for(uint64_t i = 0; i < dummy.len; i++)
		dummy.nump[i] = num->nump[num->len - 1 - i];
	copynum(num, &dummy, 0);
	free(dummy.nump);
}

void selectsectionnum(num_t* section, num_t* source, uint64_t start, uint64_t end){
	if(section == source){
		printf("Selectsectionnum was used inappropriately\n");
		return;
	}
	free(section->nump);
	uint64_t tempformax = start;
	if(start > end){ start = end; end = tempformax;}

	initnum(section, end - start, source->sign, source->dim);
	for(uint64_t i = 0; i < section->len; i++)
		section->nump[i] = source->nump[(start + i) % source->len];
}

void insertnum(num_t* des, num_t* src, uint64_t pos){
	for(uint64_t i = 0; i < src->len; i++)
		des->nump[(i + pos) % des->len] = src->nump[i];
}

uint8_t cmpnum(num_t* arg1, num_t* arg2, bool considersign){//returns 0 for arg1 = arg2, 1 for arg1 > arg2, 2 for arg1 < arg2
	uint8_t result = 0;
	uint8_t num1 = 0;
	uint8_t num2 = 0;
	uint64_t maxlen = arg1->len;
	if(arg2->len > maxlen) maxlen = arg2->len;
	for(int64_t i = (int64_t)maxlen - 1; i >= 0; i--){
		if(i < (int64_t)arg1->len) num1 = arg1->nump[i];
		if(i < (int64_t)arg2->len) num2 = arg2->nump[i];
		if(num1 > num2){ result = 1; break;}
		if(num1 < num2){ result = 2; break;}
	}
	if(considersign){
		if(arg1->sign < arg2->sign)
			result = 1;
		else if(arg1->sign > arg2->sign)
			result = 2;
		else if(arg1->sign == 1 && result != 0)
			result = 3 - result;
	}
	return result;
}

void sumnum(num_t* res, num_t* arg1, num_t* arg2, bool subtract){
	int suboradd = 0;
	if(subtract) suboradd = 1;
	suboradd = 1 - 2*(( arg1->sign + suboradd + arg2->sign ) % 2);

	num_t* tempformax = arg2;
	if(cmpnum(arg1, arg2, false) == 2){
		arg2 = arg1;
		arg1 = tempformax;
	}// arg1 is now the highest number

	uint64_t maxlen = arg1->len;
	if(maxlen < arg2->len) maxlen = arg1->len;
	num_t dummy; initnum(&dummy, maxlen + 1, 0, 0);
	copynum(&dummy, arg1, 1);

	if(subtract && tempformax == arg1) dummy.sign += 1 - 2*dummy.sign;

	uint8_t carry = 2;
	for(uint64_t i = 0; i < dummy.len; i++){
		//20 + arg1->nump[i] - arg2->nump[i] + (carry - 2)
		dummy.nump[i] += 18 + carry;
		if(i < arg2->len) dummy.nump[i] += (suboradd)*arg2->nump[i];
		carry = psi(10, 1, dummy.nump[i]);
		dummy.nump[i] = psi(10, 0, dummy.nump[i]);
	}

	if(dummy.nump[dummy.len-1] != 0) copynum(res, &dummy, 0);
	else {
		res->len = maxlen;
		copynum(res, &dummy, 1);
	}
	free(dummy.nump);
}

int multquaternionbase(int one, int two){//This returns dim*2 + sign, so account for that!
	const int table[4][4] ={ 
	{0, 2, 4, 6},
	{2, 1, 6, 5},
	{4, 7, 1, 2},
	{6, 4, 3, 1}};
	return table[one][two];
}

void multnum(num_t* res, num_t* arg1, num_t* arg2){
	num_t dummy;
	initnum(&dummy, arg1->len + arg2->len, (arg1->sign + arg2->sign)%2, multquaternionbase(arg1->dim, arg2->dim));
	dummy.sign = (dummy.sign + dummy.dim)%2;
	dummy.dim = dummy.dim/2;
	for(uint64_t i = 0; i < dummy.len; i++)
		dummy.nump[i] = 0;

	num_t* tempformax = arg2;
	if(arg1->len < arg2->len){ arg2 = arg1; arg1 = tempformax;}//arg1 is now always the longest (not nessecarily the largest)

	uint64_t j = 0;
	uint64_t maxj = arg1->len;
	for(uint64_t z = 0; z < arg2->len; z++){
		for(uint64_t i = 0; i < arg1->len; i++){
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

void divnum(num_t* res, num_t* mod, num_t* num, num_t* den){
	int dim = multquaternionbase(num->dim, den->dim);
	int sign = (num->sign + den->sign + dim)%2;
	if(den->dim > 0) sign = 1 - sign;
	dim /= 2;
	if(cmpnum(num, den, false) == 2){
		copynum(mod, num, 0);
		shiftnum(res, -1*(int)res->len);
		mod->dim = dim; mod->sign = sign;
		res->dim = dim; res->sign = sign;
		return;
	} else if(isnumzero(den)){
		printf("Divide by zero, no operation preformed.\n");
		return;
	}
	num_t moddummy; initnum(&moddummy, 1, 0, 0);
	copynum(&moddummy, num, 0);
	moddummy.dim = 0; moddummy.sign = 0;
	
	uint64_t testpower = num->len - den->len;
	num_t resdummy;
	initnum(&resdummy, testpower + 1, 0, 0);
	for(uint64_t i = 0; i < resdummy.len; i++)
		resdummy.nump[i] = 0;

	num_t dummy;
	uint64_t j;

	uint64_t previouscmp;
	uint64_t thiscmp;
	for(testpower = testpower; testpower != (uint64_t)-1; testpower--){
		previouscmp = 1;
		for(uint8_t testdigit = 9; testdigit != 0; testdigit--){ 
			initnum(&dummy, den->len + 1, 0, 0);
			for(uint64_t i = 0; i < dummy.len; i++)
				dummy.nump[i] = 0;

			for(uint64_t i = 0; i < den->len; i++){
				dummy.nump[i] += den->nump[i]*testdigit;
				j = i;
				while(psi(10, 1, dummy.nump[j]) != 0 && j < den->len){
					dummy.nump[j+1] += psi(10, 1, dummy.nump[j]);
					dummy.nump[j] = psi(10, 0, dummy.nump[j]);
					j++;
				}
			}
			
			shiftnum(&dummy, testpower);
			thiscmp = cmpnum(&dummy, &moddummy, false);
			//printf("testpower: %u cmp: %u testdigit: %u\n", testpower, thiscmp, testdigit);
			if(previouscmp < thiscmp || thiscmp == 0){
				//printf("that's it!\n");
				sumnum(&moddummy, &moddummy, &dummy, true);
				//printnum(&moddummy, false);
				resdummy.nump[testpower] = testdigit;
				//printf("tesdigit: %u\n", testdigit);
				testdigit = 1;
				if(isnumzero(&moddummy)) testpower = 0;
			}
			previouscmp = thiscmp;
			//printnum(&dummy, false);
			free(dummy.nump);
		}
	}
	//printnum(&resdummy, false);

	moddummy.dim = dim; moddummy.sign = sign;
	copynum(mod, &moddummy, 0);

	resdummy.dim = dim; resdummy.sign = sign;
	copynum(res, &resdummy, 0);

	free(moddummy.nump);
	free(resdummy.nump);
}

#endif
