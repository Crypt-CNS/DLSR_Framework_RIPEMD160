#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include "RIPEMD160.h"
using namespace std;

//compute difference based on the pattern
string inverse(string str){
	string temp="";
	for (int i = 31; i >=0; i--){
		temp += str[i];
	}
	return temp;
}

void collisionLeft(int attackedStep){
	int solutionSize = 0x4000000,left,right,choice,end;
	cout << "Choose solution size: 0-> 0x4000000; 1-> 0x8000000; 2-> 0x10000000" << endl;
	cout << "Please input your choice:";
	cin >> choice;
	if (choice == 0){
		solutionSize = 0x4000000;
	}
	else if (choice == 1){
		solutionSize = 0x8000000;
	}
	else if (choice == 2){
		solutionSize = 0x10000000;
	}
	else{
		cout << "Please input {0,1,2}!" << endl;
		return;
	}

	cout << "Preset solution size:" <<hex<< solutionSize << endl;
	cout << "Please input left bound and right bound:";
	cin >> left >> right;
	//check input error
	if (left >= right){
		cout << "left must be smaller than right!" << endl;
		return;
	}
	unsigned long long END = (unsigned long long)solutionSize*(unsigned long long)right;
	if (END > 0x100000000){
		cout << "SolutionSize * right must be smalller than 0x100000000!" << endl;
		return;
	}

	RIPEMD160 ripemd;
	ripemd.setAttackSteps(attackedStep);
	ripemd.setSolutionSize(solutionSize);
	ripemd.setBound(left, right);

	cout << "Please input attack steps:";
	cin >> end;

	if (attackedStep == 30){//To make the program find a collision, end should be 27
		ripemd.readDifferentialPath("30DiffChara.txt", 0, 0);
		ripemd.initializeDensePart(COLLISION_ATTACK_LEFT);
		ripemd.computeFromIVCollisionLeft_30Steps(end);
	}
	else if (attackedStep == 31){//To make the program find a collision, end should be 29. In this case, we suggest to set outputParam=27
		int outputParam;
		cout << "Please input the step to output when it fails: ";
		cin >> outputParam;
		ripemd.readDifferentialPath("31DiffChara.txt", 0, 0);
		ripemd.initializeDensePart(COLLISION_ATTACK_LEFT);
		ripemd.computeFromIVCollisionLeft_31Steps(end,outputParam);
	}
	else if (attackedStep == 33){//To find a instance of the left branch, end should be 33
		ripemd.readDifferentialPath("33DiffChara.txt", 0, 0);
		ripemd.initializeDensePart(COLLISION_ATTACK_LEFT);
		ripemd.computeFromIVCollisionLeft_33Steps(end);
	}
	else if (attackedStep == 34){
		//To find a instance of the left branch, end should be 34
		//To test the partial calculation, end should be set 27
		ripemd.readDifferentialPath("34DiffChara.txt", 0, 0);
		ripemd.initializeDensePart(COLLISION_ATTACK_LEFT);
		ripemd.computeFromIVCollisionLeft_34Steps(end);
	}
}

int main(){
	cout << "30-> Test 30-step collision attack!" << endl;
	cout << "31-> Test 31-step collision attack!" << endl;
	cout << "33-> Test 33-step collision attack!" << endl;
	cout << "34-> Test 34-step collision attack!" << endl;
	cout << "Else-> Exist" << endl << endl;
	cout << "Please input the steps: ";

	int attackedSteps;
	while (cin >> attackedSteps){
		if (attackedSteps == 30 || attackedSteps == 31 || attackedSteps == 33 || attackedSteps == 34){
			collisionLeft(attackedSteps);
		}
		else{
			break;
		}
		cout << endl;
		cout << "Please input the steps: ";
	}
	system("pause");
	return 0;
}