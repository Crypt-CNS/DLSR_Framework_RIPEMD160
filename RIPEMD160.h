#ifndef _RIPEMD160_H
#define _RIPEMD160_H
#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
using namespace std;

typedef unsigned int UINT32;
#define WORD_NUM 16
#define COLLISION_ATTACK 0
#define NEAR_COLLISION_ATTACK 1
#define NEAR_COLLISION_ATTACK_PADDING 2
#define COLLISION_ATTACK_LEFT 3


const unsigned int EXP[32] = { 0x1, 0x2, 0x4, 0x8,
0x10, 0x20, 0x40, 0x80,
0x100, 0x200, 0x400, 0x800,
0x1000, 0x2000, 0x4000, 0x8000,
0x10000, 0x20000, 0x40000, 0x80000,
0x100000, 0x200000, 0x400000, 0x800000,
0x1000000, 0x2000000, 0x4000000, 0x8000000,
0x10000000, 0x20000000, 0x40000000, 0x80000000 };

struct Condition{
	UINT32 andMask;
	UINT32 orMask;
	UINT32 checkValue;
	int index2;
	int type=-1;
	vector<int> pos1;
	vector<int> pos2;

	int moreType = -1;
	vector<int> pos3;
	vector<int> pos4;
};

class RIPEMD160{
private:
	UINT32 word[WORD_NUM];//message word
	UINT32 wordDiff;
	UINT32 deltaWord[WORD_NUM];
	bool isFixed[16];
	int differenceIndex;

	UINT32 LIV[5];//IV in the left branch
	UINT32 RIV[5];//IV in the right branch
	UINT32 X[80];//Internal state in the left branch
	UINT32 Y[80];//Internal state in the right branch
	UINT32 LQ[80];
	UINT32 RQ[80];
	UINT32 IN[80];
	UINT32 OUT[80];

	UINT32 LDiff[80];//The difference of the internal state in the left branch
	UINT32 RDiff[80];//The difference of the internal state in the right branch

	UINT32 LC[5];//constants in the left branch
	UINT32 RC[5];//constants values in the right branch
	UINT32 LS[5][16];//rotation values in the left branch
	UINT32 RS[5][16];//rotation values in the right branch
	UINT32 LI[5][16];//word permutation in the left branch
	UINT32 RI[5][16];//word permutation in the right branch

	Condition condition[80];

	UINT32 accurateEXP[32][10];
	UINT32 preRandomValue;

	//vector<vector<UINT32> > solution;//(X9, X8)
	UINT32 **solution;
	UINT32 *SIZE_S;
	//vector<UINT32> M3;

	int solutionSize;
	int leftBound;
	int rightBound;

	int attackSteps;
public:
	RIPEMD160();
	~RIPEMD160();
	void setSolutionSize(int size);
	void setBound(int left, int right);
	void setAttackSteps(int steps);
	string toBinary(unsigned int num);

	void getOrigialAndNewValueByPattern(string str, UINT32 &originalValue, UINT32 &newValue);

	UINT32 getValueByPattern(string str);
	void initializeModularDifference(string str, int steps,int branchNumber);
	UINT32 computeProbability(UINT32 in, UINT32 out, int shift);
	UINT32 computeProbabilityCheck(UINT32 in, UINT32 out, int shift);
	void generateAccurateEXP();
	double getRange(UINT32 number);

	//Formalize the output
	void formalizeOutputToText(string str[], int steps, int branchNumber);
	void outputPattern(string str);

	void getPossibleInDifference(UINT32 out, int shift, UINT32 possibleValue[], int &len);

	//Output the modular difference of the boolean function
	void outputBooleanFunctionDifference(string x,string y,string z,int steps, int branchNumber);

	//Solution
	void computeSolutionSet(UINT32 c0, UINT32 c1, int shift);

	//readDifferentialPath
	void readDifferentialPath(string filename,int branchNume,int isGeneratingStartingPoint);
	string inverse(string str);
	UINT32 getAndMask(string str);
	UINT32 getOrMask(string str);
	UINT32 getDifference(string str);
	UINT32 getCheckValue(string str);

	//modify
	void multiConditionCorrect(int type, vector<int> pos1, vector<int> pos2, int index1, int index2, int size, int branchNum);
	bool multiConditionCheck(int type, vector<int> pos1, vector<int> pos2, int index1, int index2, int size, int branchNum);
	inline bool checkQ(UINT32 q, int index, int shift);
	void generateStartingPoint(int start, int end,int branchNum);
	inline unsigned int getRand();
	void getW(int steps, int branchNum);
	void getState(int steps, int branchNum);
	void modify(int steps, int branchNum);
	bool checkState(int steps,int branchNum);
	void updateWord(int steps, int branchNum);
	bool isUsed(int steps, int branchNum);
	bool checkCondition(int steps, int branchNum);

	//verify
	bool verify(int start, int end, int branchNum);
	void outputOneLineForWindow(unsigned int original, unsigned int newValue);
	bool checkFinalResult(int end, int branchNum);
	void computeHash(int start, int steps);

	//outputLatex
	void read(string filename);
	void outputOneLine(string str);

	//collision attack
	void initializeDensePart(int attackType);
	bool computeFromIVCollision();
	void computeFromMiddleCollision();
	bool checkLatterConditionsCollision(int end);

	//near collision attack
	void computeFromIVCollisionLeft_33Steps(int end);
	void computeFromIVCollisionLeft_34Steps(int end);
	void computeFromIVCollisionLeft_30Steps(int end);
	void computeFromIVCollisionLeft_31Steps(int end, int outputParam);
	int checkRightBranch();
	int checkRightBranch_31();

	//test probabilistic part on the right branch for collision attack
	bool testProbabilisticRightBranchCollision(int start, int end);

	//test probabilistic part on the right branch for near collision attack
	bool testSparseRightBranch(int roundNum);

	//convert pattern to value
	void outputWordFromPattern(string filename);

	//compute final output
	void computeFinalOutput();
};
#endif