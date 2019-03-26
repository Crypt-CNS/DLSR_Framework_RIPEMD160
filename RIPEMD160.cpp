#include "RIPEMD160.h"
#include <fstream>
#include <cstdlib>
#include <string>
#include <vector>
#include <ctime>
#include <unordered_map>
using namespace std;

unsigned int LL(unsigned int number, int r){
	number = (number >> (32 - r)) | (number << r);
	return number;
}

/*rotate right*/
unsigned int RR(unsigned int number, int r){
	number = (number << (32 - r)) | (number >> r);
	return number;
}

/*Get the certain bit*/
inline unsigned int getBit(unsigned int number, int position){
	return (number >> position) & 0x1;
}

/*round function XOR*/
unsigned int XOR(unsigned int x, unsigned int y, unsigned int z){
	return x^y^z;
}

/*round function IFX*/
unsigned int IFX(unsigned int x, unsigned int y, unsigned int z){
	return (x&y) ^ ((~x)&z);
}

/*round function XOR*/
unsigned int ONZ(unsigned int x, unsigned int y, unsigned int z){
	return (x | (~y)) ^ z;
}

/*round function XOR*/
unsigned int IFZ(unsigned int x, unsigned int y, unsigned int z){
	return (x&z) ^ (y&(~z));
}

/*round function ONX*/
unsigned int ONX(unsigned int x, unsigned int y, unsigned int z){
	return x ^ (y | (~z));
}

//convert to binary
string RIPEMD160::toBinary(unsigned int num){
	string str = "";
	unsigned int left = 0;
	int array[32];
	for (int i = 0; i < 32; i++){
		array[i] = 0;
	}
	int count = 0;
	while (num){
		left = num % 2;
		array[count++] = left;
		num = num / 2;
	}

	for (int i = 31; i >= 0; i--){
		cout << array[i];
		if (array[i] == 1){
			str += '1';
		}
		else{
			str += '0';
		}
		if (i % 4 == 0){
			cout << " ";
			str += ' ';
		}
	}
	cout << endl;
	return str;
}

RIPEMD160::RIPEMD160(){
	//Initialization of IV
	LIV[0] = RIV[0] = 0XC059D148;
	LIV[1] = RIV[1] = 0x7c30f4b8;
	LIV[2] = RIV[2] = 0X1D840C95;
	LIV[3] = RIV[3] = 0X98BADCFE;
	LIV[4] = RIV[4] = 0xefcdab89;

	//Initialization of constants in the left branch.
	LC[0] = 0X0;
	LC[1] = 0X5A827999;
	LC[2] = 0X6ED9EBA1;
	LC[3] = 0X8F1BBCDC;
	LC[4] = 0XA953FD4E;

	//Initialization of constants in the right branch.
	RC[0] = 0X50A28BE6;
	RC[1] = 0X5C4DD124;
	RC[2] = 0X6D703EF3;
	RC[3] = 0X7A6D76E9;
	RC[4] = 0X0;

	unsigned char LST[5][16] = { 11, 14, 15, 12, 5, 8, 7, 9, 11, 13, 14, 15, 6, 7, 9, 8,
		7, 6, 8, 13, 11, 9, 7, 15, 7, 12, 15, 9, 11, 7, 13, 12,
		11, 13, 6, 7, 14, 9, 13, 15, 14, 8, 13, 6, 5, 12, 7, 5,
		11, 12, 14, 15, 14, 15, 9, 8, 9, 14, 5, 6, 8, 6, 5, 12,
		9, 15, 5, 11, 6, 8, 13, 12, 5, 12, 13, 14, 11, 8, 5, 6 };

	unsigned RST[5][16] = { 8, 9, 9, 11, 13, 15, 15, 5, 7, 7, 8, 11, 14, 14, 12, 6,
		9, 13, 15, 7, 12, 8, 9, 11, 7, 7, 12, 7, 6, 15, 13, 11,
		9, 7, 15, 11, 8, 6, 6, 14, 12, 13, 5, 14, 13, 13, 7, 5,
		15, 5, 8, 11, 14, 14, 6, 14, 6, 9, 12, 9, 12, 5, 15, 8,
		8, 5, 12, 9, 12, 5, 14, 6, 8, 13, 6, 5, 15, 13, 11, 11 };

	unsigned int LIT[5][16] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
		7, 4, 13, 1, 10, 6, 15, 3, 12, 0, 9, 5, 2, 14, 11, 8,
		3, 10, 14, 4, 9, 15, 8, 1, 2, 7, 0, 6, 13, 11, 5, 12,
		1, 9, 11, 10, 0, 8, 12, 4, 13, 3, 7, 15, 14, 5, 6, 2,
		4, 0, 5, 9, 7, 12, 2, 10, 14, 1, 3, 8, 11, 6, 15, 13 };

	unsigned int RIT[5][16] = { 5, 14, 7, 0, 9, 2, 11, 4, 13, 6, 15, 8, 1, 10, 3, 12,
		6, 11, 3, 7, 0, 13, 5, 10, 14, 15, 8, 12, 4, 9, 1, 2,
		15, 5, 1, 3, 7, 14, 6, 9, 11, 8, 12, 2, 10, 0, 4, 13,
		8, 6, 4, 1, 3, 11, 15, 0, 5, 12, 2, 13, 9, 7, 10, 14,
		12, 15, 10, 4, 1, 5, 8, 7, 6, 2, 13, 14, 0, 3, 9, 11 };

	for (int i = 0; i != 5; i++){
		for (int j = 0; j != 16; j++){
			LS[i][j] = LST[i][j];//Initialization of rotation values in the left branch.
			RS[i][j] = RST[i][j];//Initialization of rotation values in the right branch.
			LI[i][j] = LIT[i][j];//Initialization of word permutation in the left branch.
			RI[i][j] = RIT[i][j];//Initialization of word permutation in the right branch.
		}
	}

	for (int i = 0; i < WORD_NUM; i++){
		deltaWord[i] = 0;
		isFixed[i] = false;
	}
	//deltaWord[15] = 0 - EXP[24];
	deltaWord[12] = EXP[15];

	for (int i = 0; i < 80; i++){
		condition[i].pos1.clear();
		condition[i].pos2.clear();
		condition[i].pos3.clear();
		condition[i].pos4.clear();
		LDiff[i] = 0;
		RDiff[i] = 0;
		IN[i] = 0;
		OUT[i] = 0;
	}

	generateAccurateEXP();
	preRandomValue = 0;
}

void RIPEMD160::setSolutionSize(int size){
	solutionSize = size;
}

void RIPEMD160::setBound(int left, int right){
	leftBound = left;
	rightBound = right;
}

void RIPEMD160::setAttackSteps(int steps){
	attackSteps = steps;
}

RIPEMD160::~RIPEMD160(){
	for (int i = 0; i < solutionSize; i++){
		delete[]solution[i];
	}
	delete[]solution;
	delete[]SIZE_S;
}

//Get random value
inline unsigned int RIPEMD160::getRand(){
	unsigned int num1 = 0, num2 = 0, result;
	num1 = rand() & 0xffff;
	num2 = rand() & 0xffff;
	result = (num1 << 16) | num2;
	result = result + preRandomValue;
	preRandomValue = result;
	return result;
}


void RIPEMD160::getOrigialAndNewValueByPattern(string str, UINT32 &originalValue, UINT32 &newValue){
	originalValue = 0, newValue = 0;
	for (int i = 0; i < 32; i++){
		if (str[i] == 'u'){
			originalValue = originalValue + EXP[i];
		}
		if (str[i] == 'n'){
			newValue = newValue + EXP[i];
		}
		if (str[i] == '1'){
			originalValue += EXP[i];
			newValue += EXP[i];
		}
	}
}

UINT32 RIPEMD160::getValueByPattern(string str){
	UINT32 result = 0;
	for (int i = 0; i < 32; i++){
		if (str[i] == 'u'){
			result = result - EXP[i];
		}
		if (str[i] == 'n'){
			result = result + EXP[i];
		}
	}
	return result;
}

void RIPEMD160::initializeModularDifference(string str, int steps, int branchNumber){
	if (branchNumber == 0){//Left
		LDiff[steps] = getValueByPattern(str);
	}
	else if (branchNumber == 1){//Right
		RDiff[steps] = getValueByPattern(str);
	}
}

void RIPEMD160::outputPattern(string str){
	for (int i = 31; i >= 0; i--){
		cout << str[i];
		if (i % 4 == 0){
			cout << " ";
		}
	}
	cout << endl;
}

void RIPEMD160::formalizeOutputToText(string str[], int steps, int branchNumber){
	//str[0] : X[i]
	//str[1]:  X[i+1]
	//str[2] : X[i+2]
	//str[3]:  X[i+3]
	//str[4]:  X[i+4]
	//str[5]:  X[i+5]

	string tempStr[6];
	for (int i = 0; i < 6; i++){
		tempStr[i] = str[i];
	}
	for (int i = 0; i < 32; i++){
		tempStr[0][i] = str[0][(i - 10 + 32) % 32];
		tempStr[1][i] = str[1][(i - 10 + 32) % 32];
		tempStr[2][i] = str[2][(i - 10 + 32) % 32];
	}

	UINT32 result[6];
	result[1] = getValueByPattern(tempStr[1]);
	result[5] = getValueByPattern(tempStr[5]);	
	result[0] = getValueByPattern(tempStr[0]);

	cout << dec << steps + 1 << endl;
	cout << "STEPS " << dec<<steps << ":  ";
	outputPattern(tempStr[4]);

	cout << "STEPS " << dec << steps - 1 << ":  ";
	outputPattern(tempStr[3]);

	cout << "STEPS " << dec << steps - 2 << ":  ";
	outputPattern(tempStr[2]);
	cout << endl;

	int shift;
	if (branchNumber == 0){
		shift = LS[steps / 16][steps % 16];
	}
	else{
		shift = RS[steps / 16][steps % 16];
	}
	int index;
	if (branchNumber == 0){
		index = LI[steps / 16][steps % 16];
	}
	else{
		index = RI[steps / 16][steps % 16];
	}
	
	UINT32 f, out;

	out = result[5] - result[1];

	UINT32 possibleIn[4];
	int len = 0;
	getPossibleInDifference(out, shift, possibleIn, len);

	for (int i = 0; i < len; i++){
		f = possibleIn[i] - getValueByPattern(tempStr[0])-deltaWord[index];
		cout << "Case " << i << ": "<<hex<<f<<"  ";
		toBinary(f);
	}

	cout << endl;
	cout << "Shift: " << dec<<shift << endl;
	for (int i = 0; i < len; i++){
		cout << "IN[" << dec << steps << "]=0x" << hex << possibleIn[i] << ";"<< endl; 
		cout << " OUT[" << dec << steps << "] = 0x" << hex << out <<";" << endl;
		cout << dec << possibleIn[i] << endl;
		cout << dec << out << endl;
		cout << "IN:   ";
		toBinary(possibleIn[i]);
		cout << "OUT:  ";
		toBinary(out);
		//cout << "Pr: " << computeProbability(possibleIn[i], out, shift) << endl;
		cout << "Pr: " << getRange(computeProbability(possibleIn[i], out, shift))-32 << endl;
		//cout << "Pr: " << computeProbabilityCheck(possibleIn[i], out, shift) << endl;
		cout << endl;
	}
	cout << "=========================================================================" << endl;
}

//根据外部差分得到内部的所有可能差分,并计算概率
void RIPEMD160::getPossibleInDifference(UINT32 out, int shift, UINT32 possibleValue[], int &len){
	len = 0;
	UINT32 in, s = RR(out, shift);
	//case 1: in = out>>>shift + 1, and out[shift-1 ~ 0] is not euqal to 0
	in = s + 1;
	if (out%EXP[shift] != 0){
		possibleValue[len] = in;
		len++;
	}

	//case 2: in = out>>>shift - EXP[32-shift], and in[31-shift ~ 0] is not euqal to 0
	in = s - EXP[32 - shift];
	if (in % EXP[32 - shift] != 0){
		possibleValue[len] = in;
		len++;
	}

	//case 3: in = out>>>shift - EXP[32-shift] + 1, and in[31-shift ~ 0] is not euqal to 0, and out[shift-1 ~ 0] is not euqal to 0
	in = s - EXP[32 - shift] + 1;
	if (in / EXP[32 - shift] != 0 && out%EXP[shift] != 0){
		possibleValue[len] = in;
		len++;
	}

	//case 4: in=out>>>shift
	in = s;
	possibleValue[len] = in;
	len++;
}

void RIPEMD160::generateAccurateEXP(){
	double index[10] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };
	double t;
	for (int k = 0; k<32; k++){
		for (int i = 0; i<10; i++){
			t = pow(2, index[i]);
			accurateEXP[k][i] = EXP[k] * t;
		}
	}
}

double RIPEMD160::getRange(UINT32 number){
	if (number == 0){
		return 0;
	}

	int index = 31;
	for (int i = 0; i<31; i++){
		if (number >= EXP[i] && number<EXP[i + 1]){
			index = i;
			break;
		}
	}

	double result = 0.9;
	double t[10] = { 0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9 };
	for (int i = 0; i<9; i++){
		if (number == accurateEXP[index][i] || (number>accurateEXP[index][i] && number<accurateEXP[index][i + 1])){
			result = t[i];
			break;
		}
	}

	return result + index;
}

UINT32 RIPEMD160::computeProbability(UINT32 in, UINT32 out, int shift){
	UINT32 lowIn, highIn, lowOut, highOut;
	lowIn = in % EXP[32 - shift];
	highIn = in / EXP[32 - shift];
	lowOut = out%EXP[shift];
	highOut = out / EXP[shift];
	
	UINT32 inP, outP,finalP;
	inP = lowIn;
	outP = lowOut;

	if (lowIn == highOut && lowOut == highIn){
		finalP = (EXP[32 - shift] - inP)*(EXP[shift] - outP);
	}
	else if (lowIn == (highOut + 1) % EXP[32 - shift] && lowOut == highIn){
		finalP = (EXP[32 - shift] - inP)*outP;
	}
	else if (lowIn == highOut && lowOut == (highIn+1)%EXP[shift]){
		finalP = inP*(EXP[shift] - outP);
	}
	else if (lowIn == (highOut + 1) % EXP[32 - shift] && lowOut == (highIn + 1) % EXP[shift]){
		finalP = inP* outP;
	}
	else{
		finalP = 0;
	}
	return finalP;
}

UINT32 RIPEMD160::computeProbabilityCheck(UINT32 in, UINT32 out, int shift){
	int count = 0;
	UINT32 left, right;

	left = LL(in + 0xffffffff,shift);
	right = out + 0xffffffff;
	if (left == right){
		count = 1;
	}

	for (UINT32 i = 0; i < 0xffffffff; i++){
		left = LL(in + i, shift);
		right = out + LL(i,shift);
		if (left == right){
			count++;
		}
	}
	return count;
}

void RIPEMD160::outputBooleanFunctionDifference(string x, string y, string z, int steps, int branchNumber){
	string tempZ = z;
	for (int i = 0; i < 32; i++){
		tempZ[i] = z[(i - 10 + 32) % 32];
	}
	z = tempZ;

	UINT32 original[3], newValue[3];
	getOrigialAndNewValueByPattern(x, original[0], newValue[0]);
	getOrigialAndNewValueByPattern(y, original[1], newValue[1]);
	getOrigialAndNewValueByPattern(z, original[2], newValue[2]);

	UINT32 result[3];

	if (steps < 16){
		if (branchNumber == 0){//left
			result[0] = XOR(original[0], original[1], original[2]);
			result[1] = XOR(newValue[0], newValue[1], newValue[2]);
		}
		else{
			result[0] = ONX(original[0], original[1], original[2]);
			result[1] = ONX(newValue[0], newValue[1], newValue[2]);
		}
	}

	else if (steps < 32){
		if (branchNumber == 0){//left
			result[0] = IFX(original[0], original[1], original[2]);
			result[1] = IFX(newValue[0], newValue[1], newValue[2]);
		}
		else{
			result[0] = IFZ(original[0], original[1], original[2]);
			result[1] = IFZ(newValue[0], newValue[1], newValue[2]);
		}
	}

	cout << "Modular Difference: "<<hex<<result[1]-result[0]<<"  ";
	toBinary(result[1] - result[0]);
}
void RIPEMD160::computeSolutionSet(UINT32 in, UINT32 out, int shift){
	//use array
	SIZE_S = new UINT32[solutionSize];

	UINT32 RIGHT = solutionSize*rightBound - 1;
	UINT32 LEFT = solutionSize*leftBound;

	for (int i = 0; i < solutionSize; i++){
		SIZE_S[i] = 0;
	}

	//compute Size
	UINT32 x, y, z;
	for (UINT32 x = 0; x < 0xffffffff; x++){
		y = RR(out - LL(x, 10), shift) - in;
		y = RR(y, 10);
		z = x^y;

		if (LEFT <= z && z <= RIGHT){
			//solution_shared[z - length*leftBound].push_back(x);
			SIZE_S[z - LEFT]++;
		}
	}

	//malloc solutionArray
	solution = new UINT32*[solutionSize];
	for (int i = 0; i < solutionSize; i++){
		solution[i] = NULL;
		if (SIZE_S[i]>0){
			solution[i] = new UINT32[SIZE_S[i]];
		}
	}

	//insert value to solutionArray
	for (UINT32 x = 0; x < 0xffffffff; x++){
		y = RR(out - LL(x, 10), shift) - in;
		y = RR(y, 10);
		z = x^y;

		if (LEFT <= z && z <= RIGHT){
			//solution_shared[z - length*leftBound].push_back(x);
			solution[z - LEFT][SIZE_S[z - LEFT] - 1] = x;
			SIZE_S[z - LEFT]--;
		}
	}

	//recompute SIZE_S
	for (int i = 0; i < solutionSize; i++){
		SIZE_S[i] = 0;
	}
	for (UINT32 x = 0; x < 0xffffffff; x++){
		y = RR(out - LL(x, 10), shift) - in;
		y = RR(y, 10);
		z = x^y;

		if (LEFT <= z && z <= RIGHT){
			SIZE_S[z - LEFT]++;
		}
	}

	//use vector
	// X<<<10 + (Y<<<10 + in)<<<shift = out
	/*UINT32 length = solutionSize;
	solution.clear();
	solution.resize(length);
	for (int i = 0; i<length; i++){
		solution[i].clear();
	}

	UINT32 x, y, z;
	for (UINT32 x = 0; x < 0xffffffff; x++){
		y = RR(out - LL(x, 10), shift) -in;
		y = RR(y, 10);
		z = x^y;

		if (length*leftBound <=z && z< length*rightBound){
			solution[z - length*leftBound].push_back(x);
			//cout << hex << x << " " << z << endl;
		}
	}*/
	cout << "Over!" << endl;
}

//compute difference based on the pattern
string RIPEMD160::inverse(string str){
	string temp = "";
	for (int i = 31; i >= 0; i--){
		temp += str[i];
	}
	return temp;
}

UINT32 RIPEMD160::getAndMask(string str){
	UINT32 result = 0;
	for (int i = 0; i < 32; i++){
		if (str[i] == '-'){
			result += EXP[i];
		}
	}
	return result;
}
UINT32 RIPEMD160::getOrMask(string str){
	UINT32 result = 0;
	for (int i = 0; i < 32; i++){
		if (str[i] == 'u' || str[i]=='1'){
			result += EXP[i];
		}
	}
	return result;
}

UINT32 RIPEMD160::getDifference(string str){
	UINT32 result = 0;
	for (int i = 0; i < 32; i++){
		if (str[i] == 'u'){
			result -= EXP[i];
		}
		else if (str[i] == 'n'){
			result += EXP[i];
		}
	}
	return result;
}

UINT32 RIPEMD160::getCheckValue(string str){
	UINT32 result = 0;
	for (int i = 0; i < 32; i++){
		if (str[i] == 'u' || str[i]=='1'){
			result += EXP[i];
		}
	}
	return result;
}

void RIPEMD160::readDifferentialPath(string filename, int branchNum, int isGeneratingStartingPoint){
	ifstream inFile(filename);
	int start, end;
	inFile >> start >> end;
	start--;
	end--;
	string str,temp;
	for (int i = start; i <= end; i++){
		inFile >> str;
		temp = inverse(str);
		cout << str << endl;

		if (branchNum == 0){
			LDiff[i] = getDifference(temp);
		}
		else if (branchNum == 1){
			RDiff[i] = getDifference(temp);
		}

		condition[i].andMask = getAndMask(temp);
		condition[i].orMask = getOrMask(temp);
		condition[i].checkValue = getCheckValue(temp);
	}

	//read multi-step condition
	int count = 0;
	inFile >> count;
	int index1, index2,type,pos,bitCount;
	for (int i = 0; i < count; i++){
		inFile >> index1;
		inFile >> index2;

		inFile >> type;
		inFile >> bitCount;
		index1--;

		condition[index1].index2 = index2 - 1;
		if (condition[index1].type != -1 && condition[index1].moreType == -1){
			condition[index1].moreType = type;
			for (int j = 0; j < bitCount; j++){
				inFile >> pos;
				condition[index1].pos3.push_back(pos);
			}
			for (int j = 0; j < bitCount; j++){
				inFile >> pos;
				condition[index1].pos4.push_back(pos);
			}
		}

		else{
			condition[index1].type = type;
			for (int j = 0; j < bitCount; j++){
				inFile >> pos;
				condition[index1].pos1.push_back(pos);
			}
			for (int j = 0; j < bitCount; j++){
				inFile >> pos;
				condition[index1].pos2.push_back(pos);
			}
		}
	}

	//read Q
	int qStart, qEnd;
	inFile >> qStart >> qEnd;
	//cout << qStart <<" "<< qEnd << endl;
	//system("pause");
	qStart--;
	qEnd--;
	for (int i = qStart; i <= qEnd; i++){
		inFile >> IN[i] >> OUT[i];
	}

	/*//test the correctness of reading
	for (int i = start; i <= end; i++){
		cout << dec << i + 1 << endl;
		cout << hex << "And: " << condition[i].andMask << endl;
		cout << hex << "OR : " << condition[i].orMask << endl;
		if (condition[i].type != -1){
			cout << "Type " << condition[i].type << endl;
			cout << "Condition pos:" << endl;
			for (int j = 0; j < condition[i].pos1.size(); j++){
				cout << dec << condition[i].pos1[j] << " ";
			}
			cout << endl;
			for (int j = 0; j < condition[i].pos1.size(); j++){
				cout << dec << condition[i].pos2[j] << " ";
			}
			cout << endl << endl;
		}
		if (condition[i].moreType != -1){
			cout << "Type " << condition[i].moreType << endl;
			cout << "Condition pos:" << endl;
			for (int j = 0; j < condition[i].pos3.size(); j++){
				cout << dec << condition[i].pos3[j] << " ";
			}
			cout << endl;
			for (int j = 0; j < condition[i].pos4.size(); j++){
				cout << dec << condition[i].pos4[j] << " ";
			}
			cout << endl << endl;
		}
	}

	for (int i = qStart; i <= qEnd; i++){
		cout << dec << IN[i] << endl;
		cout << dec << OUT[i] << endl << endl;
	}
	system("pause");*/

	if (isGeneratingStartingPoint == 1){
		generateStartingPoint(start, 22, branchNum);
	}
}

void RIPEMD160::multiConditionCorrect(int type, vector<int> pos1, vector<int> pos2, int index1, int index2, int size, int branchNum){
	UINT32 T1, T2;
	if (branchNum == 0){
		T1 = X[index1];
		T2 = X[index2];
	}
	else if (branchNum == 1){
		T1 = Y[index1];
		T2 = Y[index2];
	}

	if (type == 0){// T[index1] V ~T[index2] = 1
		for (int i = 0; i < size; i++){
			if (getBit(T1, pos1[i]) == 0 && getBit(T2, pos2[i]) == 1){
				T1 = T1 ^ EXP[pos1[i]];
			}
		}
	}
	else if (type == 1){//T[index1] = T[index2]
		for (int i = 0; i < size; i++){
			if (getBit(T1, pos1[i]) != getBit(T2, pos2[i])){
				T1 = T1 ^ EXP[pos1[i]];
			}
		}
	}

	else if (type == 2){//T[index1] ^ T[index2] = 0
		for (int i = 0; i < size; i++){
			if (getBit(T1, pos1[i]) != getBit(T2, pos2[i])){
				T1 = T1 ^ EXP[pos1[i]];
			}
		}
	}

	else if (type == 3){//T[index1] ^ T[index2] = 1
		for (int i = 0; i < size; i++){
			if (getBit(T1, pos1[i]) == getBit(T2, pos2[i])){
				T1 = T1 ^ EXP[pos1[i]];
			}
		}
	}

	if (branchNum == 0){
		X[index1]=T1;
	}
	else if (branchNum == 1){
		Y[index1]=T1;
	}
}

bool RIPEMD160::multiConditionCheck(int type, vector<int> pos1, vector<int> pos2, int index1, int index2, int size, int branchNum){
	UINT32 T1, T2;
	if (branchNum == 0){
		T1 = X[index1];
		T2 = X[index2];
	}
	else if (branchNum == 1){
		T1 = Y[index1];
		T2 = Y[index2];
	}

	if (type == 0){// T[index1] V ~T[index2] = 1
		for (int i = 0; i < size; i++){
			if (getBit(T1, pos1[i]) == 0 && getBit(T2, pos2[i]) == 1){
				return false;
			}
		}
	}
	else if (type == 1){//T[index1] = T[index2]
		for (int i = 0; i < size; i++){
			if (getBit(T1, pos1[i]) != getBit(T2, pos2[i])){
				return false;
			}
		}
	}
	else if (type == 2){//T[index1] ^ T[index2] = 0
		for (int i = 0; i < size; i++){
			if (getBit(T1, pos1[i]) != getBit(T2, pos2[i])){
				return false;
			}
		}
	}
	else if (type == 3){//T[index1] ^ T[index2] = 1
		for (int i = 0; i < size; i++){
			if (getBit(T1, pos1[i]) == getBit(T2, pos2[i])){
				return false;
			}
		}
	}
	return true;
}

inline bool RIPEMD160::checkQ(UINT32 q, int index, int shift){
	if (LL(q + IN[index], shift) != OUT[index] + LL(q, shift)){
		return false;
	}
	return true;
}

void RIPEMD160::getW(int steps, int branchNum){
	int shift, wIndex;
	if (branchNum == 0){//left
		shift = LS[steps / 16][steps % 16];
		wIndex = LI[steps / 16][steps % 16];

		if (steps < 16){
			word[wIndex] = RR(X[steps] - LL(X[steps - 4], 10), shift) - (XOR(X[steps - 1], X[steps - 2], LL(X[steps - 3], 10)) + LL(X[steps - 5], 10) + LC[0]);
		}
		else if (steps < 32){
			word[wIndex] = RR(X[steps] - LL(X[steps - 4], 10), shift) - (IFX(X[steps - 1], X[steps - 2], LL(X[steps - 3], 10)) + LL(X[steps - 5], 10) + LC[1]);
		}
	}
	else{//right
		shift = RS[steps / 16][steps % 16];
		wIndex = RI[steps / 16][steps % 16];

		if (steps < 16){
			word[wIndex] = RR(Y[steps] - LL(Y[steps - 4], 10), shift) - (ONX(Y[steps - 1], Y[steps - 2], LL(Y[steps - 3], 10)) + LL(Y[steps - 5], 10) + RC[0]);
		}
		else if (steps < 32){
			word[wIndex] = RR(Y[steps] - LL(Y[steps - 4], 10), shift) - (IFZ(Y[steps - 1], Y[steps - 2], LL(Y[steps - 3], 10)) + LL(Y[steps - 5], 10) + RC[1]);
		}
	}
	//cout << dec << wIndex << endl;
	isFixed[wIndex] = true;
}

void RIPEMD160::getState(int steps, int branchNum){
	int shift, wIndex;
	if (branchNum == 0){//left
		shift = LS[steps / 16][steps % 16];
		wIndex = LI[steps / 16][steps % 16];

		if (steps < 16){
			X[steps] = LL(X[steps - 4], 10) + LL(XOR(X[steps - 1], X[steps - 2], LL(X[steps - 3], 10)) + LL(X[steps - 5], 10) + LC[0] + word[wIndex], shift);
		}
		else if (steps < 32){
			X[steps] = LL(X[steps - 4], 10) + LL(IFX(X[steps - 1], X[steps - 2], LL(X[steps - 3], 10)) + LL(X[steps - 5], 10) + LC[1] + word[wIndex], shift);
		}
	}
	else{//right
		shift = RS[steps / 16][steps % 16];
		wIndex = RI[steps / 16][steps % 16];

		if (steps < 16){
			Y[steps] = LL(Y[steps - 4], 10) + LL(ONX(Y[steps - 1], Y[steps - 2], LL(Y[steps - 3], 10)) + LL(Y[steps - 5], 10) + RC[0] + word[wIndex], shift);
		}
		else if (steps < 32){
			Y[steps] = LL(Y[steps - 4], 10) + LL(IFZ(Y[steps - 1], Y[steps - 2], LL(Y[steps - 3], 10)) + LL(Y[steps - 5], 10) + RC[1] + word[wIndex], shift);
		}
	}
}

void RIPEMD160::modify(int i, int branchNum){
	UINT32 T;
	T = getRand();
	T = T & condition[i].andMask;
	T = T | condition[i].orMask;
	if (branchNum == 0){
		X[i] = T;
	}
	else if (branchNum == 1){
		Y[i] = T;
	}
	if (condition[i].type != -1){
		multiConditionCorrect(condition[i].type, condition[i].pos1, condition[i].pos2, i, condition[i].index2, condition[i].pos1.size(), branchNum);
	}
	if (condition[i].moreType != -1){//moreType
		multiConditionCorrect(condition[i].moreType, condition[i].pos3, condition[i].pos4, i, condition[i].index2, condition[i].pos3.size(), branchNum);
	}
}

bool RIPEMD160::checkState(int steps, int branchNum){
	UINT32 Q;
	int shift;
	modify(steps, branchNum);
	if (branchNum == 0){
		shift = LS[steps / 16][steps % 16];
		Q = RR(X[steps] - LL(X[steps - 4], 10),shift);
	}
	else{
		shift = RS[steps / 16][steps % 16];
		Q = RR(Y[steps] - LL(Y[steps - 4], 10),shift);
		
	}
	return checkQ(Q, steps, shift);
}

bool RIPEMD160::isUsed(int steps, int branchNum){
	if (branchNum == 0){
		if (isFixed[LI[steps / 16][steps % 16]] == false){
			return false;
		}
	}
	else if (branchNum == 1){
		if (isFixed[RI[steps / 16][steps % 16]] == false){
			return false;
		}
	}
	return true;
}

bool RIPEMD160::checkCondition(int steps, int branchNum){
	UINT32 T,Q,shift;
	if (branchNum == 0){
		shift = LS[steps / 16][steps % 16];
		T = X[steps];
		Q =RR(X[steps] - LL(X[steps-4],10),shift);
	}
	else{
		shift = RS[steps / 16][steps % 16];
		T = Y[steps];
		Q = RR(Y[steps] - LL(Y[steps - 4], 10),shift);
	}

	//Single Condition
	T = T&(~condition[steps].andMask);
	if (T != condition[steps].checkValue){
		//cout << "condition" << endl;
		//system("pause");
		return false;
	}

	//multi-condition check
	if (condition[steps].type != -1 && multiConditionCheck(condition[steps].type, condition[steps].pos1, condition[steps].pos2, steps, condition[steps].index2, condition[steps].pos1.size(), branchNum) == false){
		//cout << steps<<" Multi type condition" << endl;
		//system("pause");
		return false;
	}
	//More Type
	if (condition[steps].moreType != -1 && multiConditionCheck(condition[steps].moreType, condition[steps].pos3, condition[steps].pos4, steps, condition[steps].index2, condition[steps].pos3.size(), branchNum) == false){
		//cout << steps<<" Multi type condition-2" << endl;
		//system("pause");
		return false;
	}

	//Check Q
	if (checkQ(Q, steps, shift) == false){
		return false;
	}

	return true;
}

void RIPEMD160::generateStartingPoint(int start, int end,int branchNum){
	int steps,shift;
	bool flag = true, isWordFixed = false;
	bool find = false;
	UINT32 Q;

	while (!find){
		for (steps = start + 1; steps <= end; steps++){
			if (steps < start + 6){
				modify(steps, branchNum);
			}

			else{
				//cout << "steps " << dec << steps << ": ";
				modify(steps, branchNum);
				getW(steps, branchNum);
			}
		}
		flag = true;
		for (steps = start + 5; steps <= end; steps++){
			if (checkCondition(steps, branchNum) == false){
				/*cout << hex << IN[steps] << " " << OUT[steps] << endl;
				toBinary(RR(X[steps] - LL(X[steps - 4], 10), LS[steps / 16][steps % 16]));
				checkCondition(steps, branchNum);
				toBinary(X[steps]);
				toBinary(X[steps - 1]);
				toBinary(X[steps - 2]);
				toBinary(X[steps - 3]);
				toBinary(LL(X[steps - 3], 10));
				toBinary(LL(X[steps - 4], 10));
				cout << steps<< " is WRONG!" << endl;
				flag = false;
				system("pause");*/
				cout << steps << " is WRONG!" << endl;
				break;
			}
		}
		if (flag == false){
			continue;
		}
		else{
			cout << "right "<<end << endl;
		}

		//check
		//compute X11
		steps = start + 5;
		X[steps - 5] = RR(X[steps] - LL(X[steps - 4], 10), LS[steps / 16][steps % 16]) -(XOR(X[steps - 1], X[steps - 2], LL(X[steps - 3], 10)) + LC[0] + word[LI[steps / 16][steps % 16]]);
		X[steps - 5] = RR(X[steps - 5], 10);

		steps = start + 4;
		if (branchNum == 0){
			shift = LS[steps / 16][steps % 16];
			Q = RR(X[steps] - LL(X[steps - 4], 10), shift);
		}
		else{
			shift = RS[steps / 16][steps % 16];
			Q = RR(Y[steps] - LL(Y[steps - 4], 10), shift);
		}

		if (attackSteps==33 && checkQ(Q, steps, shift) && getBit(X[start], 11) != getBit(X[start + 1], 21)){//33 steps
			find = true;
		}
		if (attackSteps==34 && checkQ(Q, steps, shift) && getBit(X[start], 11) == getBit(X[start + 1], 21) && getBit(X[start], 12) == getBit(X[start + 1], 22) && getBit(X[start], 13) == getBit(X[start + 1], 23)){//33 steps
			find = true;
		}
		if (attackSteps == 30 && checkQ(Q, steps, shift) && getBit(X[start], 11) != getBit(X[start + 1], 21)){
			find = true;
		}
		if (attackSteps == 31 && checkQ(Q, steps, shift) && getBit(X[start], 17) != getBit(X[start + 1], 27) && getBit(X[start], 16) != getBit(X[start + 1], 26) && getBit(X[start], 15) != getBit(X[start + 1], 25) && getBit(X[start], 14) != getBit(X[start + 1], 24) && getBit(X[start], 13) != getBit(X[start + 1], 23) && getBit(X[start], 12) != getBit(X[start + 1], 22) && getBit(X[start], 11) != getBit(X[start + 1], 21)){
			find = true;
		}
	}

	/////output
	steps = end+1;
	if (branchNum == 0){
		for (int i = start; i < steps; i++){
			cout << "X" << dec << i+1 << " : ";
			toBinary(X[i]);
		}
	}

	else if (branchNum == 1){
		for (int i = start; i < steps; i++){
			cout << "Y" << dec << i+1 << " : ";
			toBinary(Y[i]);
		}
	}

	
	if (branchNum == 0){
		for (int i = start; i < steps; i++){
			cout << "X[" << dec << i << "]=0x" << hex << X[i] << ";" << endl;
		}
	}

	else if (branchNum == 1){
		for (int i = start; i < steps; i++){
			cout << "Y[" << dec << i << "]=0x" << hex << Y[i] << ";" << endl;
		}
	}

	for (int i = 0; i < 16; i++){
		if (isFixed[i] == true){
			cout << "word[" << dec << i << "]=0x" << hex << word[i] << ";" << endl;
		}
	}
	
	
	verify(start, end,branchNum);
}

bool RIPEMD160::verify(int start, int end,int branchNum){
	UINT32 T1[80], T2[80];
	
	word[differenceIndex] = word[differenceIndex] + wordDiff;

	if (branchNum == 0){//left
		for (int i = start; i <= end; i++){
			T1[i] = X[i];
			if (i < start + 5){
				T2[i] = X[i] + LDiff[i];
				X[i] = T2[i];
			}
		}
		for (int i = start + 5; i <= end; i++){
			getState(i, 0);
			T2[i] = X[i];
		}

		for (int i = start; i <= end; i++){
			cout << "X" << dec << i + 1 << ": ";
			outputOneLineForWindow(T1[i], T2[i]);
		}

		if (T2[end] - T1[end] == LDiff[end])
			return true;
	}
	else{
		for (int i = start; i <= end; i++){
			T1[i] = Y[i];
			if (i < start + 5){
				T2[i] = Y[i] + RDiff[i];
				Y[i] = T2[i];
			}
		}
		for (int i = start + 5; i <= end; i++){
			getState(i, 1);
			T2[i] = Y[i];
		}

		for (int i = start; i <= end; i++){
			cout << "Y" << dec << i + 1 << ": ";
			outputOneLineForWindow(T1[i], T2[i]);
		}
		cout << endl;

		if (T2[end] - T1[end] == RDiff[end])
			return true;
	}

	return false;
}

void RIPEMD160::outputOneLineForWindow(unsigned int original, unsigned int newValue){
	string str;
	str = "";
	unsigned int result;
	for (int i = 31; i >= 0; i--){
		if (getBit(original, i) == getBit(newValue, i)){
			result = getBit(original, i);
			if (result == 1){
				str += "1";
			}
			else{
				str += "0";
			}
		}
		else{
			if (getBit(original, i) == 0 && getBit(newValue, i) == 1){
				str += "n";
			}
			else if (getBit(original, i) == 1 && getBit(newValue, i) == 0){
				str += "u";
			}
		}
	}
	cout << str << endl;
}

void RIPEMD160::outputOneLine(string str){
	for (int i = 0; i < 32; i++){
		if (i != 0 && i % 8 == 0){
			cout << " & ";
		}

		cout << str[i] << "&";
	}
}

void RIPEMD160::read(string filename){
	ifstream inFile(filename);
	int start, end;
	string str[7];
	int count = 0;
	inFile >> start >> end;
	for (int i = start; i <= end; i++){
		if (count < 16){
			inFile >> str[0] >> str[1] >> str[2] >> str[3] >> str[4] >> str[5] >> str[6];
			//cout << str[0] << str[1] << str[2] << str[3] << str[4] << str[5] << str[6];
			count++;
		}
		else{
			inFile >> str[0] >> str[1] >> str[2] >> str[3] >> str[4];
		}
		cout << setw(2) << i + 1 << "& ";
		outputOneLine(str[2]);
		cout << " " << setw(2) <<(int)LI[i / 16][i % 16] << "& ";
		outputOneLine(str[4]);
		cout << " " << setw(2) << (int)RI[i / 16][i % 16];
		cout << endl;
		cout << "\\" << "\\ " << endl;
	}
}

void RIPEMD160::initializeDensePart(int attackType){
	if (attackType==COLLISION_ATTACK_LEFT){
		if (attackSteps == 33){
			X[10] = 0x909f3c1f;
			X[11] = 0x2cc56b94;
			X[12] = 0x59c26f57;
			X[13] = 0xd6c5bd87;
			X[14] = 0xc267ee7d;
			X[15] = 0x7e0113b1;
			X[16] = 0x17ee15a3;
			X[17] = 0x13177f0f;
			X[18] = 0x8bed5915;
			X[19] = 0x6ef209eb;
			X[20] = 0x587d1046;
			X[21] = 0x48af1e8a;
			X[22] = 0x37fad421;

			word[1] = 0xf2470729;
			word[4] = 0xd19ebad5;
			word[6] = 0x1f2c0d0e;
			word[7] = 0xc4f488a9;
			word[10] = 0x236883a;
			word[13] = 0x8425047b;
			word[15] = 0x6458c5e3;

			differenceIndex = 12;
			wordDiff = EXP[15];
		}
		else if (attackSteps == 34){
			X[10] = 0xbd842688;
			X[11] = 0x5296d502;
			X[12] = 0xc37a4515;
			X[13] = 0xc0168920;
			X[14] = 0xbc6dc20;
			X[15] = 0x3244f36;
			X[16] = 0x91687a8a;
			X[17] = 0x5a3b7677;
			X[18] = 0xd9459a2e;
			X[19] = 0x983f1691;
			X[20] = 0xd2c486c6;
			X[21] = 0x49239b6d;
			X[22] = 0x15ccff5f;

			word[1] = 0x58a0be2;
			word[4] = 0x8d38c100;
			word[6] = 0x7214c160;
			word[7] = 0xea755943;
			word[10] = 0xa6a0ee3e;
			word[13] = 0xb9e9de76;
			word[15] = 0xb949ab42;

			differenceIndex = 12;
			wordDiff = EXP[15];

			/*/test
			word[0] = 0xc2056cdf;
			word[1] = 0x58a0be2;
			word[2] = 0xe114b874;
			word[3] = 0xb7f045ff;
			word[4] = 0x8d38c100;
			word[5] = 0x4e926b96;
			word[6] = 0x7214c160;
			word[7] = 0xea755943;
			word[8] = 0x496a5788;
			word[9] = 0x857f0518;
			word[10] = 0xa6a0ee3e;
			word[11] = 0xcd1f88a9;
			word[12] = 0x14a4951c;
			word[13] = 0xb9e9de76;
			word[14] = 0x65df3f3a;
			word[15] = 0xb949ab42;

			checkFinalResult(34, 0);
			/*/
		}
		else if (attackSteps == 30){
			X[10] = 0xfc60c171;
			X[11] = 0x6aefc645;
			X[12] = 0x8ef00ad6;
			X[13] = 0x4eae6408;
			X[14] = 0x84f09d7d;
			X[15] = 0xafc0db46;
			X[16] = 0x4b6c55d;
			X[17] = 0x60cc2545;
			X[18] = 0xc3164812;
			X[19] = 0xe2e6dab8;
			X[20] = 0x9d94174b;
			X[21] = 0x97f83c88;
			X[22] = 0xe8f1a1a7;

			word[1] = 0x8ad15821;
			word[4] = 0x4d2d28f6;
			word[6] = 0x8c7769dc;
			word[7] = 0x961cce16;
			word[10] = 0x37316e69;
			word[13] = 0xe748e57f;
			word[15] = 0xe753c86;

			differenceIndex = 12;
			wordDiff = 0 - EXP[15];

			/*//test found collision
			word[0] = 0x1fbb5316;
			word[1] = 0x8ad15821;
			word[2] = 0xbf04a498;
			word[3] = 0xb85ed58f;
			word[4] = 0x4d2d28f6;
			word[5] = 0x977b64cd;
			word[6] = 0x8c7769dc;
			word[7] = 0x961cce16;
			word[8] = 0x9d7a5bc6;
			word[9] = 0xf6519d38;
			word[10] = 0x37316e69;
			word[11] = 0x206d429;
			word[12] = 0x2f451be9;
			word[13] = 0xe748e57f;
			word[14] = 0x5c73a141;
			word[15] = 0xe753c86;

			computeHash(0, 30);
			UINT32 TX[80], TY[80];
			for (int i = 0; i < 30; i++){
				TX[i] = X[i];
				TY[i] = Y[i];
			}

			//Add difference
			word[12] = word[12] + wordDiff;
			computeHash(0, 30);

			for (int i = 0; i < 30; i++){
				cout << "X " << dec << i + 1 << ": ";
				outputOneLineForWindow(TX[i], X[i]);

				cout << "\t";
				cout << "Y " << dec << i + 1 << ": ";
				outputOneLineForWindow(TY[i], Y[i]);

				cout << endl;
			}
			cout << endl;
			system("pause");*/
		}
		else if (attackSteps == 31){
			X[10] = 0x5ceea8aa;
			X[11] = 0x555cadf5;
			X[12] = 0xd7e0f1f6;
			X[13] = 0xb5445ae5;
			X[14] = 0x62a659f5;
			X[15] = 0x20465949;
			X[16] = 0x9558f4d5;
			X[17] = 0x35b2cab9;
			X[18] = 0x395eba0f;
			X[19] = 0x5bf93153;
			X[20] = 0x5b9a9d48;
			X[21] = 0x67800fc;
			X[22] = 0x92c47adc;

			word[1] = 0xff13f724;
			word[4] = 0xa9df768c;
			word[6] = 0xd8cfa4bb;
			word[7] = 0xedb8f36f;
			word[10] = 0xd13c6647;
			word[13] = 0xfc5c01e5;
			word[15] = 0xe5e50cc1;

			differenceIndex = 12;
			wordDiff = EXP[15];
		}

		//compute solution
		UINT32 in = XOR(X[12], X[11], LL(X[10], 10)) + word[13] + LC[0];
		UINT32 out = X[13];

		cout << "Pre-compute solutions of (X9, X10)... Due to the memory limitation, we only choose a part from all the solutions." << endl;
		computeSolutionSet(in, out, 7);//(xor, X9);
	}
	srand(time(NULL));
}

bool RIPEMD160::computeFromIVCollision(){
	Y[0] = LL(RIV[1], 10) + LL(LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[5] + RC[0], RS[0][0]);//word[5] is known

	word[14] = getRand();
	Y[1] = LL(RIV[2], 10) + LL(LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[14] + RC[0], RS[0][1]);//word[14] is unknown

	Y[2] = LL(RIV[3], 10) + LL(LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[7] + RC[0], RS[0][2]);//word[7] is known

	Y[3] = LL(RIV[4], 10) + LL(LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[0] + RC[0], RS[0][3]);//word[0] is known

	word[9] = getRand();
	Y[4] = LL(Y[0], 10) + LL(LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[9] + RC[0], RS[0][4]);// word[9] is unknown


	//Let Y6=0x0, then compute Y[5]
	Y[6] = 0;
	UINT32 tmp = RR(Y[6] - LL(Y[2], 10), RS[0][6]) - (word[11] + RC[0] + LL(Y[1], 10));//word[11] is known
	Y[5] = ONX(tmp, Y[4], LL(Y[3], 10));
	word[2] = RR(Y[5] - LL(Y[1], 10), RS[0][5]) - (ONX(Y[4], Y[3], LL(Y[2], 10)) + LL(Y[0], 10) + RC[0]);//compute word[2]

	//compute Y[8]
	//Y[9] = Y[5]<<<10 + (ONX(Y[8],Y[7],Y[6]<<<10) + Y[4]<<<10 + word[6] +RC[0])<<<RS[0][9]
	//Since Y[6] = 0 ----> ONX(Y[8],Y[7],Y[6]<<<10) = Y[8]^0xffffffff;
	Y[8] = RR(Y[9] - LL(Y[5], 10), RS[0][9]) - (LL(Y[4], 10) + word[6] + RC[0]);//word[6] is known
	Y[8] = Y[8] ^ 0xffffffff;

	tmp = RR(Y[8] - LL(Y[4], 10), RS[0][8]) - (LL(Y[3], 10) + word[13] + RC[0]);//word[13] is known
	Y[7] = ONX(tmp, Y[6], LL(Y[5], 10));
	word[4] = RR(Y[7] - LL(Y[3], 10), RS[0][7]) - (ONX(Y[6], Y[5], LL(Y[4], 10)) + LL(Y[2], 10) + RC[0]);//compute word[4]

	//compute word[10], word[1], word[8], word[15] to link two parts
	for (int i = 10; i < 14; i++){
		word[RI[0][i]] = RR(Y[i] - LL(Y[i - 4], 10), RS[0][i]) - (ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + LL(Y[i - 5], 10) + RC[0]);
	}

	//Check the modular difference
	for (int i = 10; i < 13; i++){
		RQ[i] = RR(Y[i] - LL(Y[i - 4], 10), RS[0][i]);
		if (checkQ(RQ[i], i, RS[0][i]) == false){
			//cout << "WRONG Q " << dec << i << endl;
			return false;
		}
	}
	return true;
}

void RIPEMD160::computeFromMiddleCollision(){
	int branchNum = 1,steps,shift;//right
	UINT32 Q;

	for (steps = 20; steps <= 22; steps++){
		modify(steps, branchNum);
		if (steps == 21){
			shift = RS[steps / 16][steps % 16];
			Q = RR(Y[steps] - LL(Y[steps - 4], 10), shift);
			if (getBit(Q, 22) == 1){
				Y[21] = Y[21] ^ EXP[30];
			}
		}
		getW(steps, branchNum);
	}
}

bool RIPEMD160::checkLatterConditionsCollision(int end){
	for (int steps = 23; steps < end; steps++){
		//getState(i, 1);
		int shift = RS[steps / 16][steps % 16];
		int wIndex = RI[steps / 16][steps % 16];
		Y[steps] = LL(Y[steps - 4], 10) + LL(IFZ(Y[steps - 1], Y[steps - 2], LL(Y[steps - 3], 10)) + LL(Y[steps - 5], 10) + RC[1] + word[wIndex], shift);
		if (checkCondition(steps, 1) == false){
			return false;
		}
	}
	
	return true;
}

//30-step collision attack
void RIPEMD160::computeFromIVCollisionLeft_30Steps(int end){
	bool find = false, flag = false;
	UINT32 runningTimes = 0;
	unsigned long long outRunningTimes=0;
	UINT32 outputCount = 0;
	UINT32 LEFT = solutionSize*leftBound;
	UINT32 RIGHT = solutionSize*rightBound - 1;
	while (find == false){
		outRunningTimes++;
		checkState(23, 0);
		getW(23, 0);//word[3]

		//randomly choose values for other bits of m0
		word[0] = getRand();
		word[2] = getRand();
		word[5] = getRand();

		X[0] = LL(LIV[1], 10) + LL(LL(LIV[0], 10) + XOR(LIV[4], LIV[3], LL(LIV[2], 10)) + word[LI[0][0]] + LC[0], LS[0][0]);
		X[1] = LL(LIV[2], 10) + LL(LL(LIV[1], 10) + XOR(X[0], LIV[4], LL(LIV[3], 10)) + word[LI[0][1]] + LC[0], LS[0][1]);
		X[2] = LL(LIV[3], 10) + LL(LL(LIV[2], 10) + XOR(X[1], X[0], LL(LIV[4], 10)) + word[LI[0][2]] + LC[0], LS[0][2]);
		X[3] = LL(LIV[4], 10) + LL(LL(LIV[3], 10) + XOR(X[2], X[1], LL(X[0], 10)) + word[LI[0][3]] + LC[0], LS[0][3]);
		X[4] = LL(X[0], 10) + LL(LL(LIV[4], 10) + XOR(X[3], X[2], LL(X[1], 10)) + word[LI[0][4]] + LC[0], LS[0][4]);

		for (int i = 5; i < 8; i++){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + XOR(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[0][i]] + LC[0], LS[0][i]);
		}

		UINT32 var = RR(X[10] - LL(X[6], 10), LS[0][10]) - (LL(X[5], 10) + word[10] + LC[0]);
		var = var^LL(X[7], 10);
		flag = false;
		if (LEFT <= var && var <= RIGHT){
			for (int j = 0; j< SIZE_S[var - LEFT]; j++){
				X[9] = solution[var - LEFT][j];
				X[8] = X[9] ^ var;

				//check Q12 and Q13
				LQ[12] = RR(X[12] - LL(X[8], 10), LS[0][12]);
				LQ[13] = RR(X[13] - LL(X[9], 10), LS[0][13]);
				if (checkQ(LQ[12], 12, LS[0][12]) && checkQ(LQ[13], 13, LS[0][13])){
					getW(8, 0);
					getW(9, 0);
					getW(11, 0);
					getW(12, 0);
					getW(14, 0);
					runningTimes++;

					flag = false;
					//Check uncontrolled part (X[24]-X[29])
					find = true;
					for (int i = 24; i < 30; i++){
						if (i < 26){
							X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFX(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[1][i - 16]] + LC[1], LS[1][i - 16]);
							if (checkCondition(i, 0) == false){
								find = false;
							}
						}
						else{//Only need to check modular difference
							X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFX(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[1][i - 16]] + LC[1], LS[1][i - 16]);
							if (checkQ(RR(X[i] - LL(X[i - 4], 10), LS[i / 16][i % 16]), i, LS[i / 16][i % 16]) == false){
								find = false;
							}

							//Further check when i=29
							if (i == 29){
								//Compute a new X[29]
								UINT32 new29, new28;
								new28 = X[28] + LDiff[28];
								new29 = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFX(new28, X[i - 2], LL(X[i - 3], 10)) + word[LI[1][i - 16]] + LC[1], LS[1][i - 16]);
								if (new29 != X[29]){
									find = false;
								}
							}
						}
						if (find == false){
							break;
						}
					}
					if (find){
						int reachedSteps = checkRightBranch();
						if (reachedSteps <= end){//Only if end=29, the collision is found.
							find = false;
							if (reachedSteps > 25){
								cout << "No." << outputCount << endl;
								cout <<"X "<< dec<< reachedSteps << " failed!" << endl;
								cout << "Running times:" << hex << runningTimes << endl;
								cout << "Miss: " << dec << outRunningTimes / runningTimes << endl << endl;
								outputCount++;
							}
						}

						else{
							for (int i = 0; i < 16; i++){
								cout << "word[" << dec << i << "] = 0x" << hex << word[i] << endl;
							}

							UINT32 TMPX[80],TMPY[80];
							computeHash(0, 30);
							for (int i = 0; i < reachedSteps; i++){
								TMPY[i] = Y[i];
							}
							for (int i = 0; i < 30; i++){
								TMPX[i] = X[i];
							}
							word[12] = word[12] + wordDiff;
							computeHash(0, 30);
							for (int i = 0; i < 30; i++){
								cout << "X " << dec << i + 1 << ": ";
								outputOneLineForWindow(TMPX[i], X[i]);
							}
							cout << endl;
							for (int i = 0; i < reachedSteps; i++){
								cout << "Y " << dec << i+1 << ": ";
								outputOneLineForWindow(TMPY[i], Y[i]);
							}

							word[12] = word[12] - wordDiff;
							cout << "Total runningTimes:" << hex << runningTimes << endl;
							cout << "Solution size:" << hex << solutionSize << endl;
							cout << "Miss: " << dec << outRunningTimes / runningTimes << endl << endl;
						}
					}
				}
				if (find){
					break;
				}
			}
		}
	}
}

//31-step collision attack
//The param outputParam is used to output the step that is not smaller than outputParam failed
void RIPEMD160::computeFromIVCollisionLeft_31Steps(int end, int outputParam){
	//Modify X[23]
	UINT32 f_12, out_12, in_12, m0_12, res_12 = 0;

	//Calculate res_12
	//in: 1100 0100
	//m : 0100 000-
	//sum 0000 01?
	//f:  10?? 00?
	//sum 10?? ?
	//out 0000 01
	//fin 1??? ??

	//in: 1100 0100
	//m : 0-11 110-
	//sum 0-00 00?
	//f:  1-?? 00?
	//sum 10?? 0
	//out 0000 01
	//fin 10?? ??

	//the carry from 20-th bit is 1 by properly choosing m_0
	/*for (int i = 0; i < 1000; i++){
	checkState(23, 0);
	getW(23, 0);

	modify(24, 0);

	word[0] = getRand();//(0100 000- ---- ---- ---- ---- ---- ----) or (0-11 110- ---- ---- ---- ---- ---- ----)
	word[0] = word[0] & 0x1ffffff;
	word[0] = word[0] | 0x40000000;

	//word[0] = word[0] & 0x41ffffff;
	//word[0] = word[0] | 0x3c000000;

	X[25] = LL(X[21], 10) + LL(LL(X[20], 10) + IFX(X[24], X[23], LL(X[22], 10)) + word[0] + LC[1], LS[1][25 - 16]);

	cout << getBit(X[25], 11) << endl;
	if (getBit(X[25], 11) == 0){
	toBinary(IFX(X[24], X[23], LL(X[22], 10)));
	toBinary(LL(X[20], 10) + LC[1]);
	toBinary(word[0]);
	toBinary(LL(X[21], 10));
	cout << endl;
	system("pause");
	}

	/*f_12 = IFX(X[24], X[23], LL(X[22], 10)) & 0xfff00000;
	in_12 = LL(X[20], 10) + LC[1];
	cout << "in:";
	toBinary(in_12);
	cout << "f_12";
	toBinary(f_12);
	cout << "out:";
	toBinary(LL(X[21], 10));
	cout << endl;

	in_12 = in_12 & 0xfff00000;
	}*/

	bool flag = false;
	bool find = false;
	unsigned long long runningTimes = 0;
	unsigned long long outRunningTimes = 0;
	UINT32 outputCount = 0;
	int reachedSteps = 0;
	UINT32 LEFT = solutionSize*leftBound;
	UINT32 RIGHT = solutionSize*rightBound - 1;
	while (find == false){
		outRunningTimes++;
		checkState(23, 0);
		getW(23, 0);//word[3]

		//randomly choose values for other bits of m0 to ensure X25,11=1
		word[0] = getRand();//(0100 000- ---- ---- ---- ---- ---- ----) or (0-11 110- ---- ---- ---- ---- ---- ----)
		if (getRand() == 0){
			word[0] = word[0] & 0x1ffffff;
			word[0] = word[0] | 0x40000000;
		}
		else{
			word[0] = word[0] & 0x41ffffff;
			word[0] = word[0] | 0x3c000000;
		}

		word[2] = getRand();
		word[5] = getRand();

		X[0] = LL(LIV[1], 10) + LL(LL(LIV[0], 10) + XOR(LIV[4], LIV[3], LL(LIV[2], 10)) + word[LI[0][0]] + LC[0], LS[0][0]);
		X[1] = LL(LIV[2], 10) + LL(LL(LIV[1], 10) + XOR(X[0], LIV[4], LL(LIV[3], 10)) + word[LI[0][1]] + LC[0], LS[0][1]);
		X[2] = LL(LIV[3], 10) + LL(LL(LIV[2], 10) + XOR(X[1], X[0], LL(LIV[4], 10)) + word[LI[0][2]] + LC[0], LS[0][2]);
		X[3] = LL(LIV[4], 10) + LL(LL(LIV[3], 10) + XOR(X[2], X[1], LL(X[0], 10)) + word[LI[0][3]] + LC[0], LS[0][3]);
		X[4] = LL(X[0], 10) + LL(LL(LIV[4], 10) + XOR(X[3], X[2], LL(X[1], 10)) + word[LI[0][4]] + LC[0], LS[0][4]);

		for (int i = 5; i < 8; i++){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + XOR(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[0][i]] + LC[0], LS[0][i]);
		}

		UINT32 var = RR(X[10] - LL(X[6], 10), LS[0][10]) - (LL(X[5], 10) + word[10] + LC[0]);
		var = var^LL(X[7], 10);
		flag = false;
		if (LEFT <= var && var <= RIGHT){
			for (int j = 0; j< SIZE_S[var - LEFT]; j++){
				X[9] = solution[var - LEFT][j];
				X[8] = X[9] ^ var;

				//check Q12 and Q13
				LQ[12] = RR(X[12] - LL(X[8], 10), LS[0][12]);
				LQ[13] = RR(X[13] - LL(X[9], 10), LS[0][13]);
				if (checkQ(LQ[12], 12, LS[0][12]) && checkQ(LQ[13], 13, LS[0][13])){
					getW(8, 0);
					getW(9, 0);
					getW(11, 0);
					getW(12, 0);
					getW(14, 0);
					runningTimes++;

					//Check uncontrolled part (X[24]-X[32])
					//Check uncontrolled part (X[24]-X[32])
					find = true;
					for (int i = 24; i < 30; i++){
						X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFX(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[1][i - 16]] + LC[1], LS[1][i - 16]);
						if (checkCondition(i, 0) == false){
							find = false;
							break;
						}
					}
					if (find){
						reachedSteps = checkRightBranch_31();
						if (reachedSteps <= end){//only if end=30, a collision is found
							//output information
							if (reachedSteps >= outputParam){
								cout << "No." << hex << outputCount << endl;
								cout << "X " << dec << reachedSteps << " failed!" << endl;

								cout << "Running times:" << hex << runningTimes << endl;
								cout << "Miss: " << dec << outRunningTimes / runningTimes << endl << endl;
								outputCount++;
							}
							find = false;

							if (reachedSteps == end){
								for (int i = 0; i < 16; i++){
									cout << hex << word[i] << " ";
								}
								cout << endl << endl;
							}
						}

						else{
							UINT32 TMPX[80],TMPY[80];
							computeHash(0, 31);
							for (int i = 0; i < reachedSteps; i++){
								TMPY[i] = Y[i];
							}
							for (int i = 0; i < 31; i++){
								TMPX[i] = X[i];
							}

							word[12] = word[12] + wordDiff;
							computeHash(0, 31);
							for (int i = 0; i < 31; i++){
								cout << "X " << dec << i + 1 << ": ";
								outputOneLineForWindow(TMPX[i], X[i]);
							}
							cout << endl;
							for (int i = 0; i < reachedSteps; i++){
								cout << "Y " << dec << i + 1 << ": ";
								outputOneLineForWindow(TMPY[i], Y[i]);
							}

							for (int i = 0; i < 16; i++){
								cout << "word[" << dec << i << "] = 0x" << hex << word[i] << "; " << endl;
							}

							cout << "Total runningTimes:" << hex << runningTimes << endl;
							cout << "Miss: " << dec << outRunningTimes / runningTimes << endl;
							cout << "Solution size:" << hex << solutionSize << endl;
						}
					}
				}
				if (find){
					break;
				}
			}
		}
	}
}

//33-step collision attack
void RIPEMD160::computeFromIVCollisionLeft_33Steps(int end){
	UINT32 f_12, out_12, in_12, x25_12, m0_12, res_12;
	//Calculate res_12
	//in:  0100 1110 1100 0011 1001 0010 1111 1010
	//m0:  ---- ---- ---- 101- ---- ---- ---- ----
	//sum: ---- ---- ---- 11-- ---- ---- ---- ----
	//F:   cccc cccc cccc ?1-- ---- ---- ---- ----
	//sum: cccc cccc cccc'(There is carry)
	//the carry from 20-th bit is 1 by properly choosing m_0
	checkState(23, 0);
	getW(23, 0);
	modify(24, 0);
	f_12 = IFX(X[24], X[23], LL(X[22], 10)) & 0xfff00000;
	in_12 = LL(X[20], 10) + LC[1];

	in_12 = in_12 & 0xfff00000;

	res_12 = (in_12 + f_12 + 0x100000) & 0xfff00000;//There is carry
	//res_12 = 0011 1010 0010 0000 0000 0000 0000 0000
	//res_12 = 0x3a200000

	bool flag = false;
	bool find = false;
	unsigned long long runningTimes = 0;
	unsigned long long outRunningTimes = 0;
	UINT32 outputCount = 0;
	UINT32 LEFT = solutionSize*leftBound;
	UINT32 RIGHT = solutionSize*rightBound - 1;
	while (find == false){
		outRunningTimes++;
		checkState(23, 0);
		getW(23, 0);//word[3]

		//Randomly choose the values for the low 12 bits of X[25] (---- ---- ---- ---- ---- 1010 1--- ----)
		X[25] = getRand();
		X[25] = X[25] & 0x7f;
		X[25] = X[25] | 0xa80;
		out_12 = RR(X[25] - LL(X[21], 10), 12);
		out_12 = out_12 & 0xfff00000;
		m0_12 = out_12 - res_12;//compute m0_12 (high 12 bits)

		//randomly choose values for other bits of m0
		word[0] = getRand();
		word[0] = word[0] & 0x0001ffff;
		word[0] = word[0] | 0x000a0000;
		word[0] = word[0] | m0_12;

		word[2] = getRand();
		word[5] = getRand();

		X[0] = LL(LIV[1], 10) + LL(LL(LIV[0], 10) + XOR(LIV[4], LIV[3], LL(LIV[2], 10)) + word[LI[0][0]] + LC[0], LS[0][0]);
		X[1] = LL(LIV[2], 10) + LL(LL(LIV[1], 10) + XOR(X[0], LIV[4], LL(LIV[3], 10)) + word[LI[0][1]] + LC[0], LS[0][1]);
		X[2] = LL(LIV[3], 10) + LL(LL(LIV[2], 10) + XOR(X[1], X[0], LL(LIV[4], 10)) + word[LI[0][2]] + LC[0], LS[0][2]);
		X[3] = LL(LIV[4], 10) + LL(LL(LIV[3], 10) + XOR(X[2], X[1], LL(X[0], 10)) + word[LI[0][3]] + LC[0], LS[0][3]);
		X[4] = LL(X[0], 10) + LL(LL(LIV[4], 10) + XOR(X[3], X[2], LL(X[1], 10)) + word[LI[0][4]] + LC[0], LS[0][4]);

		for (int i = 5; i < 8; i++){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + XOR(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[0][i]] + LC[0], LS[0][i]);
		}

		UINT32 var = RR(X[10] - LL(X[6], 10), LS[0][10]) - (LL(X[5], 10) + word[10] + LC[0]);
		var = var^LL(X[7], 10);
		flag = false;
		if (LEFT <= var && var <= RIGHT){
			for (int j = 0; j< SIZE_S[var - LEFT]; j++){
				X[9] = solution[var - LEFT][j];
				X[8] = X[9] ^ var;

				//check Q12 and Q13
				LQ[12] = RR(X[12] - LL(X[8], 10), LS[0][12]);
				LQ[13] = RR(X[13] - LL(X[9], 10), LS[0][13]);
				if (checkQ(LQ[12], 12, LS[0][12]) && checkQ(LQ[13], 13, LS[0][13])){
					getW(8, 0);
					getW(9, 0);
					getW(11, 0);
					getW(12, 0);
					getW(14, 0);
					runningTimes++;

					flag = false;
					//Check uncontrolled part (X[24]-X[32])
					find = true;
					for (int i = 24; i < end; i++){
						if (i < 32){
							X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFX(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[1][i - 16]] + LC[1], LS[1][i - 16]);
							if (checkCondition(i, 0) == false){
								find = false;
								//output step information
								if (i >= 28){
									cout << endl;
									cout << "No." << hex << outputCount << endl;
									cout << "Steps " << dec << i << " failed!" << endl;
									cout << "Running times:" << hex << runningTimes << endl;
									cout << "Miss: " << dec << outRunningTimes / runningTimes << endl << endl;
									outputCount++;
								}
							}
						}
						if (i == 32){//Only need to check modular difference
							//int shift = LS[i / 16][i % 16];
							//UINT32 Q = RR(X[i] - LL(X[i - 4], 10), shift);
							X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + ONZ(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[2][i - 32]] + LC[2], LS[2][i - 32]);
							if (checkQ(RR(X[i] - LL(X[i - 4], 10), LS[i / 16][i % 16]), i, LS[i / 16][i % 16]) == false){
								find = false;
								//output step information
								cout << endl;
								cout << "No." << hex << outputCount << endl;
								cout << "Steps " << dec << 32 << " failed!" << endl;
								cout << "Running times:" << hex << runningTimes << endl;
								cout << "Miss: " << dec << outRunningTimes / runningTimes << endl << endl;
								outputCount++;
							}
						}
						if (find == false){
							break;
						}
					}
					if (find){
						cout << "Found" << endl;
						if (checkFinalResult(end, 0)){
							cout << "right!" << endl;
						}
						for (int i = 0; i < 16; i++){
							if (i == 12){
								word[i] = word[i] - wordDiff;
							}
							cout << "word[" << dec << i << "] = 0x" << hex << word[i] << endl;
						}
						cout << "Running times:" << hex << runningTimes << endl;
						cout << "Miss: " << dec << outRunningTimes / runningTimes << endl << endl;
						cout << "Solution size:" << hex << solutionSize << endl;
					}
				}
				if (find){
					break;
				}
			}
		}
	}
}

//34-step collision attack
void RIPEMD160::computeFromIVCollisionLeft_34Steps(int end){
	//Modify X[23]
	UINT32 f_20, out_20, in_20, x25_20, m0_20, res_20;

	//Calculate res_12
	//in:  0100 1110 1100 0011 1001 0010 1111 1010
	//m0:  ---- ---- ---- 101- ---- ---- ---- ----
	//sum: ---- ---- ---- 11-- ---- ---- ---- ----
	//F:   cccc cccc cccc ?1-- ---- ---- ---- ----
	//sum: cccc cccc cccc'(There is carry)
	//the carry from 20-th bit is 1 by properly choosing m_0

	/*cout << "X21: ";
	toBinary(LL(X[21], 10));
	cout << "X20:";
	toBinary(LL(X[20], 10) + LC[1]);
	UINT32 carry;
	for (int i = 0; i < 10; i++){
	checkState(23, 0);
	getW(23, 0);
	modify(24, 0);

	f_20 = IFX(X[24], X[23], LL(X[22], 10)) & 0xfffff;
	in_20 = LL(X[20], 10) + LC[1];
	in_20 = in_20 & 0xfffff;
	res_20 = (in_20 + f_20) & 0xfffff;//There is always no carry
	toBinary(res_20);

	//Randomly choose the values for the high 20 bits of X[25] (100u 0-n- --00 -11- ---- ---- ---- ----)
	X[25] = getRand();
	X[25] = X[25] & 0x05c9ffff;
	X[25] = X[25] | 0x90060000;

	X[25] = X[25] >> 12;
	out_20 = LL(X[21], 10) >> 12;
	//assume carry=0
	carry = 0;
	out_20 = X[25] - out_20;//out_20 = res_20 + m_20

	if (getRand() % 0x1 == 0){//since res_20 has tow possible values
	res_20 = 0xaf13b;
	}
	else{
	res_20 = 0xb113b;
	}

	m0_20 = out_20 - res_20;//compute m0_20 (higher 20 bits)
	}
	system("pause");*/
	//There are 2 possible values for res_20 based on the value of X24[13]
	//res_20 = 0000 0000 0000 1010 1111 0001 0011 1011(0xaf13b)
	//res_20 = 0000 0000 0000 1011 0001 0001 0011 1011(0xb113b)

	bool flag = false;
	bool find = false;
	unsigned long long runningTimes = 0;
	unsigned long long outRunningTimes = 0;
	UINT32 outputCount = 0;
	UINT32 LEFT = solutionSize*leftBound;
	UINT32 RIGHT = solutionSize*rightBound - 1;
	while (find == false){
		outRunningTimes++;
		checkState(23, 0);
		getW(23, 0);//word[3]

		//Randomly choose the values for the low 12 bits of X[25] (100u 0-n- --00 -11- ---- ---- ---- ----)
		X[25] = getRand();
		X[25] = X[25] & 0x05c9ffff;
		X[25] = X[25] | 0x90060000;

		X[25] = X[25] >> 12;
		out_20 = LL(X[21], 10) >> 12;
		//assume carry=0
		out_20 = X[25] - out_20;//out_20 = res_20 + m_20
	
		/*if (getRand() % 2 == 0){//carry=0
			out_20 = X[25] - out_20;//out_20 = res_20 + m_20
		}
		else{//carry=1
			out_20 = X[25] - out_20 - 1;//out_20 = res_20 + m_20
		}*/

		if (getRand() % 2 == 0){//since res_20 has tow possible values
			res_20 = 0xaf13b;
		}
		else{
			res_20 = 0xb113b;
		}
		m0_20 = (out_20 - res_20) & 0xfffff;//compute m0_20 (higher 20 bits)

		//randomly choose values for other bits of m0
		word[0] = getRand();
		word[0] = word[0] & 0xfff00000;
		word[0] = word[0] | m0_20;

		word[2] = getRand();
		word[5] = getRand();

		X[0] = LL(LIV[1], 10) + LL(LL(LIV[0], 10) + XOR(LIV[4], LIV[3], LL(LIV[2], 10)) + word[LI[0][0]] + LC[0], LS[0][0]);
		X[1] = LL(LIV[2], 10) + LL(LL(LIV[1], 10) + XOR(X[0], LIV[4], LL(LIV[3], 10)) + word[LI[0][1]] + LC[0], LS[0][1]);
		X[2] = LL(LIV[3], 10) + LL(LL(LIV[2], 10) + XOR(X[1], X[0], LL(LIV[4], 10)) + word[LI[0][2]] + LC[0], LS[0][2]);
		X[3] = LL(LIV[4], 10) + LL(LL(LIV[3], 10) + XOR(X[2], X[1], LL(X[0], 10)) + word[LI[0][3]] + LC[0], LS[0][3]);
		X[4] = LL(X[0], 10) + LL(LL(LIV[4], 10) + XOR(X[3], X[2], LL(X[1], 10)) + word[LI[0][4]] + LC[0], LS[0][4]);

		for (int i = 5; i < 8; i++){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + XOR(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[0][i]] + LC[0], LS[0][i]);
		}

		UINT32 var = RR(X[10] - LL(X[6], 10), LS[0][10]) - (LL(X[5], 10) + word[10] + LC[0]);
		var = var^LL(X[7], 10);
		flag = false;
		if (LEFT <= var && var <= RIGHT){
			for (int j = 0; j< SIZE_S[var - LEFT]; j++){
				X[9] = solution[var - LEFT][j];
				X[8] = X[9] ^ var;

				//check Q12 and Q13
				LQ[12] = RR(X[12] - LL(X[8], 10), LS[0][12]);
				LQ[13] = RR(X[13] - LL(X[9], 10), LS[0][13]);
				if (checkQ(LQ[12], 12, LS[0][12]) && checkQ(LQ[13], 13, LS[0][13])){
					getW(8, 0);
					getW(9, 0);
					getW(11, 0);
					getW(12, 0);
					getW(14, 0);
					runningTimes++;

					flag = false;
					//Check uncontrolled part (X[24]-X[32])
					find = true;
					for (int i = 24; i < end; i++){
						if (i < 32){
							X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFX(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[1][i - 16]] + LC[1], LS[1][i - 16]);
						}
						else{
							X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + ONZ(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[2][i - 32]] + LC[2], LS[2][i - 32]);
						}
						if (checkCondition(i, 0) == false){
							find = false;
							if (i >= 28){//output step information
								cout << endl;
								cout << "No." << hex << outputCount << endl;
								cout << "Steps " << dec << i << " failed!" << endl;
								cout << "Running times:" << hex << runningTimes << endl;
								cout << "Miss: " << dec << outRunningTimes / runningTimes << endl << endl;
								outputCount++;
							}
							break;
						}
					}
					if (find){
						cout << "Found" << endl;
						if (checkFinalResult(end, 0)){
							cout << "right!" << endl;

							for (int i = 0; i < 16; i++){
								if (i == 12){
									word[i] = word[i] - wordDiff;
								}
								cout << "word[" << dec << i << "] = 0x" << hex << word[i] << endl;
							}
						}
						cout << "Running times:" << hex << runningTimes << endl;
						cout << "Miss: " << dec << outRunningTimes / runningTimes << endl;
						cout << "Solution size:" << hex << solutionSize << endl;
					}
				}
				if (find){
					break;
				}
			}
		}
	}
}

//Compute the hash value according to the start step and the steps
void RIPEMD160::computeHash(int start, int steps){
	for (int i = start; i < steps; i++){
		if (i < 5){
			switch (i){
			case 0:
				X[0] = LL(LIV[1], 10) + LL(LL(LIV[0], 10) + XOR(LIV[4], LIV[3], LL(LIV[2], 10)) + word[LI[0][0]] + LC[0], LS[0][0]);

				RQ[0] = LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0];
				Y[0] = LL(RIV[1], 10) + LL(LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0], RS[0][0]);
				break;
			case 1:
				X[1] = LL(LIV[2], 10) + LL(LL(LIV[1], 10) + XOR(X[0], LIV[4], LL(LIV[3], 10)) + word[LI[0][1]] + LC[0], LS[0][1]);

				RQ[1] = LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0];
				Y[1] = LL(RIV[2], 10) + LL(LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0], RS[0][1]);
				break;
			case 2:
				X[2] = LL(LIV[3], 10) + LL(LL(LIV[2], 10) + XOR(X[1], X[0], LL(LIV[4], 10)) + word[LI[0][2]] + LC[0], LS[0][2]);

				RQ[2] = LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0];
				Y[2] = LL(RIV[3], 10) + LL(LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0], RS[0][2]);
				break;
			case 3:
				X[3] = LL(LIV[4], 10) + LL(LL(LIV[3], 10) + XOR(X[2], X[1], LL(X[0], 10)) + word[LI[0][3]] + LC[0], LS[0][3]);

				RQ[3] = LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0];
				Y[3] = LL(RIV[4], 10) + LL(LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0], RS[0][3]);
				break;
			case 4:
				X[4] = LL(X[0], 10) + LL(LL(LIV[4], 10) + XOR(X[3], X[2], LL(X[1], 10)) + word[LI[0][4]] + LC[0], LS[0][4]);

				RQ[4] = LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0];
				Y[4] = LL(Y[0], 10) + LL(LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0], RS[0][4]);
				break;
			}
		}
		else if (i < 16){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + XOR(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[0][i]] + LC[0], LS[0][i]);

			RQ[i] = LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0];
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0], RS[0][i]);
		}
		else if (i < 32){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFX(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[1][i - 16]] + LC[1], LS[1][i - 16]);

			RQ[i] = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1], RS[1][i - 16]);
		}
		else if (i < 48){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + ONZ(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[2][i - 32]] + LC[2], LS[2][i - 32]);
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + ONZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[2][i - 32]] + RC[2], RS[2][i - 32]);
		}
		else if (i < 64){
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + IFZ(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[3][i - 48]] + LC[3], LS[2][i - 48]);
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + IFX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[3][i - 48]] + RC[3], RS[2][i - 48]);
		}
		else{
			X[i] = LL(X[i - 4], 10) + LL(LL(X[i - 5], 10) + ONX(X[i - 1], X[i - 2], LL(X[i - 3], 10)) + word[LI[4][i - 64]] + LC[4], LS[2][i - 64]);
			Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + XOR(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[4][i - 64]] + RC[4], RS[2][i - 64]);
		}
	}
}

//check result
bool RIPEMD160::checkFinalResult(int end,int branchNum){
	UINT32 T1[80], T2[80];

	computeHash(0, end);

	if (branchNum == 0){//left
		for (int i = 0; i < end; i++){
			T1[i] = X[i];
		}
	}
	else{//right
		for (int i = 0; i < end; i++){
			T1[i] = Y[i];
		}
	}

	if (branchNum == 0){//left
		word[12] = word[12] + wordDiff;
		computeHash(0, end);
	}
	else{//right
		word[15] = word[15] + wordDiff;
		computeHash(0, end);
	}

	if (branchNum == 0){//left
		for (int i = 0; i < end; i++){
			T2[i] = X[i];
		}

		if (T2[end-1] - T1[end-1] != LDiff[end-1]){
			cout << "wonrg" << endl;
			return false;
		}
	}
	else{//right
		for (int i = 0; i < end; i++){
			T2[i] = Y[i];
		}

		for (int i = 0; i < end; i++){
			cout << "Y " << dec << i+1 << ": ";
			outputOneLineForWindow(T1[i], T2[i]);
		}

		/*if (T2[end-1] - T1[end-1] != RDiff[end-1]){
			return false;
		}*/
	}

	for (int i = 0; i < end; i++){
		cout << "X " << dec << i+1 << ": ";
		outputOneLineForWindow(T1[i], T2[i]);
	}

	return true;
}

//30-step differential path
int RIPEMD160::checkRightBranch(){
	//compute right branch until X[15]
	Y[0] = LL(RIV[1], 10) + LL(LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0], RS[0][0]);
	Y[1] = LL(RIV[2], 10) + LL(LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0], RS[0][1]);
	Y[2] = LL(RIV[3], 10) + LL(LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0], RS[0][2]);
	Y[3] = LL(RIV[4], 10) + LL(LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0], RS[0][3]);
	Y[4] = LL(Y[0], 10) + LL(LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0], RS[0][4]);
	
	for (int i = 5; i < 16; i++){
		Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0], RS[0][i]);
	}

	//check conditions on right branch
	UINT32 Q,in=0,out=0;
	int i = 16;
	//Y_13
	if (getBit(Y[13], 11) != 0){
		return 13;
	}

	//Y_14
	if (getBit(Y[14], 11) != 1){
		return 14;
	}

	//Y_15 (need to know in and out)
	Q = RR(Y[15]  - LL(Y[11], 10),RS[0][15]);
	in = wordDiff;
	out = 0xffe00000;
	if (LL(Q + in, RS[0][15]) != out + LL(Q, RS[0][15])){
		return 15;
	}
	if (getBit(Y[15], 21) != 1){
		return 15;
	}

	//Y_16
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);

	//Y_17
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 31) != getBit(Y[i - 1], 31) || getBit(Y[i], 21) != 1){
		return 17;
	}

	//Y_18
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 21) != 1){
		return 18;
	}

	//Y_19
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 31) != 1){
		return 19;
	}

	//Y_20
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);

	//Y_21
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 0) != 0 || getBit(Y[i], 31) != 1 || getBit(Y[i], 9) != getBit(Y[i - 1], 9)){
		return 21;
	}

	//Y_22
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 0) != 1 || getBit(Y[i], 31) != 1){
		return 22;
	}

	//Y_23
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 10) != 1 || getBit(Y[i], 9) != 0){
		return 23;
	}

	//Y_24
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);

	//Y_25
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 9) != 1 || getBit(Y[i], 10) != 0 || getBit(Y[i], 12) != 0 || getBit(Y[i], 19) != getBit(Y[i - 1], 19) || getBit(Y[i], 20) != getBit(Y[i - 1], 20)){
		return 25;
	}

	//Y_26
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 9) != 1 || getBit(Y[i], 10) != 1 || getBit(Y[i], 12) != 1){
		return 26;
	}

	//Y_27 (need to know in and out)
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 19) != 0 || getBit(Y[i], 20) != 1 || getBit(Y[i], 22) != 1){
		return 27;
	}
	in = wordDiff;
	out = 0xffc00000;
	if (LL(Q + in, RS[1][i - 16]) != out + LL(Q, RS[1][i - 16])){
		return 27;
	}

	return 29;
}

//31-step diffrential path
int RIPEMD160::checkRightBranch_31(){
	//compute right branch until Y[15]
	Y[0] = LL(RIV[1], 10) + LL(LL(RIV[0], 10) + ONX(RIV[4], RIV[3], LL(RIV[2], 10)) + word[RI[0][0]] + RC[0], RS[0][0]);
	Y[1] = LL(RIV[2], 10) + LL(LL(RIV[1], 10) + ONX(Y[0], RIV[4], LL(RIV[3], 10)) + word[RI[0][1]] + RC[0], RS[0][1]);
	Y[2] = LL(RIV[3], 10) + LL(LL(RIV[2], 10) + ONX(Y[1], Y[0], LL(RIV[4], 10)) + word[RI[0][2]] + RC[0], RS[0][2]);
	Y[3] = LL(RIV[4], 10) + LL(LL(RIV[3], 10) + ONX(Y[2], Y[1], LL(Y[0], 10)) + word[RI[0][3]] + RC[0], RS[0][3]);
	Y[4] = LL(Y[0], 10) + LL(LL(RIV[4], 10) + ONX(Y[3], Y[2], LL(Y[1], 10)) + word[RI[0][4]] + RC[0], RS[0][4]);

	for (int i = 5; i < 16; i++){
		Y[i] = LL(Y[i - 4], 10) + LL(LL(Y[i - 5], 10) + ONX(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[0][i]] + RC[0], RS[0][i]);
	}

	//check conditions on right branch
	UINT32 Q, in = 0, out = 0;
	int i = 16;
	//Y_13
	if (getBit(Y[13], 11) != 0){
		return 13;
	}

	//Y_14
	if (getBit(Y[14], 11) != 1){
		return 14;
	}

	//Y_15 (need to know in and out)
	Q = RR(Y[15] - LL(Y[11], 10), RS[0][15]);
	in = wordDiff;
	out = 0x200000;
	if (LL(Q + in, RS[0][15]) != out + LL(Q, RS[0][15])){
		return 15;
	}
	if (getBit(Y[15], 21) != 0){
		return 15;
	}

	//Y_16
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);

	//Y_17
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 31) != getBit(Y[i - 1], 31) || getBit(Y[i], 21) != 1){
		return 17;
	}

	//Y_18
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 21) != 1){
		return 18;
	}

	//Y_19
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 31) != 0){
		return 19;
	}

	//Y_20
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);

	//Y_21
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 0) != 0 || getBit(Y[i], 31) != 1 || getBit(Y[i], 9) != getBit(Y[i - 1], 9)){
		return 21;
	}

	//Y_22
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 0) != 1 || getBit(Y[i], 31) != 1){
		return 22;
	}

	//Y_23
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 10) != 0 || getBit(Y[i], 9) != 1){
		return 23;
	}

	//Y_24
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);

	//Y_25
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 9) != 1 || getBit(Y[i], 10) != 0 || getBit(Y[i], 12) != 0 || getBit(Y[i], 19) != getBit(Y[i - 1], 19) || getBit(Y[i], 20) != getBit(Y[i - 1], 20)){
		return 25;
	}

	//Y_26
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 9) != 1 || getBit(Y[i], 10) != 1 || getBit(Y[i], 12) != 1){
		return 26;
	}

	//Y_27 (need to know in and out)
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 19) != 1 || getBit(Y[i], 20) != 0 || getBit(Y[i], 22) != 0){
		return 27;
	}
	in = wordDiff;
	out = 0x400000;
	if (LL(Q + in, RS[1][i - 16]) != out + LL(Q, RS[1][i - 16])){
		return 27;
	}

	//Y_28
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);

	//Y_29
	i++;
	Q = LL(Y[i - 5], 10) + IFZ(Y[i - 1], Y[i - 2], LL(Y[i - 3], 10)) + word[RI[1][i - 16]] + RC[1];
	Y[i] = LL(Y[i - 4], 10) + LL(Q, RS[1][i - 16]);
	if (getBit(Y[i], 30) != getBit(Y[i - 1], 30) || getBit(Y[i], 29) != getBit(Y[i - 1], 29) || getBit(Y[i], 0) != getBit(Y[i - 1], 0)){
		return 29;
	}

	return 30;
}

void RIPEMD160::outputWordFromPattern(string filename){
	ifstream inFile(filename);
	int start, end;
	inFile >> start >> end;
	start--;
	end--;
	string str, temp;
	UINT32 newValue[80];
	for (int i = start; i <= end; i++){
		inFile >> str;
		temp = inverse(str);
		LDiff[i] = getDifference(temp);
		getOrigialAndNewValueByPattern(temp, X[i], newValue[i]);
	}

	for (int i = 16; i < 32; i++){
		getW(i, 0);
	}

	for (int i = 0; i < 16; i++){
		cout << "word[" << i << "] = " << hex << word[i] << ";" << endl;
	}

	checkFinalResult(33, 0);
}

//compute final output
void RIPEMD160::computeFinalOutput(){
	LIV[0] = RIV[0] = 0XC059D148;//Y-4
	LIV[1] = RIV[1] = 0x7c30f4b8;//Y-3
	LIV[2] = RIV[2] = 0X1D840C95;//Y-2
	LIV[3] = RIV[3] = 0X98BADCFE;//Y-1
	LIV[4] = RIV[4] = 0xefcdab89;//Y0

	UINT32 h[5],fi[5];
	h[0] = RR(LIV[0], 10);
	h[4] = RR(LIV[1], 10);
	h[3] = RR(LIV[2], 10);
	h[2] = LIV[3];
	h[1] = LIV[4];

	///test found collision（30-step）
	//int steps = 29;
	/*wordDiff = 0 - EXP[15];
	word[0] = 0x1fbb5316;
	word[1] = 0x8ad15821;
	word[2] = 0xbf04a498;
	word[3] = 0xb85ed58f;
	word[4] = 0x4d2d28f6;
	word[5] = 0x977b64cd;
	word[6] = 0x8c7769dc;
	word[7] = 0x961cce16;
	word[8] = 0x9d7a5bc6;
	word[9] = 0xf6519d38;
	word[10] = 0x37316e69;
	word[11] = 0x206d429;
	word[12] = 0x2f451be9;
	word[13] = 0xe748e57f;
	word[14] = 0x5c73a141;
	word[15] = 0xe753c86;*/

	//31-step 
	int steps = 30;
	wordDiff = EXP[15];
	word[0] = 0x3d604874;
	word[1] = 0xff13f724;
	word[2] = 0xd60f43b4;
	word[3] = 0xc02645eb;
	word[4] = 0xa9df768c;
	word[5] = 0x172f15dc;
	word[6] = 0xd8cfa4bb;
	word[7] = 0xedb8f36f;
	word[8] = 0xc898dd5e;
	word[9] = 0x71c62ade;
	word[10] = 0xd13c6647;
	word[11] = 0xbfa932ef;
	word[12] = 0xfc2b5325;
	word[13] = 0xfc5c01e5;
	word[14] = 0x5f7658c8;
	word[15] = 0xe5e50cc1;

	computeHash(0, steps+1);
	fi[0] = h[1] + X[steps - 1] + LL(Y[steps - 2],10);
	fi[1] = h[2] + LL(X[steps - 2],10) + LL(Y[steps - 3],10);
	fi[2] = h[3] + LL(X[steps - 3],10) + LL(Y[steps - 4],10);
	fi[3] = h[4] + LL(X[steps - 4],10) + Y[steps];
	fi[4] = h[0] + X[steps] + Y[steps-1];
	cout << hex << fi[0] << " " << fi[1] << " " << fi[2] << " " << fi[3] << " " << fi[4] << endl;

	word[12] = word[12] + wordDiff;
	cout << "word[12]: " << hex << word[12] << endl;
	computeHash(0, steps + 1);
	fi[0] = h[1] + X[steps - 1] + LL(Y[steps - 2], 10);
	fi[1] = h[2] + LL(X[steps - 2], 10) + LL(Y[steps - 3], 10);
	fi[2] = h[3] + LL(X[steps - 3], 10) + LL(Y[steps - 4], 10);
	fi[3] = h[4] + LL(X[steps - 4], 10) + Y[steps];
	fi[4] = h[0] + X[steps] + Y[steps - 1];
	cout << hex << fi[0] << " " << fi[1] << " " << fi[2] << " " << fi[3] << " " << fi[4] << endl;
}