#pragma once
#include "LAnalyzer.h"
#include <set>

using namespace std;
using nlohmann::json;

class RPN {
public:
	RPN(json lexems, json idn, const bool _infoKey);
	~RPN();
	
	json::iterator cycleVar;
	set<string> cycleVarHistory;

	void ReversePolishNotation();

private:
	map<string, size_t> priority;
	map<string, int> labelMap;
	map<string, pair<double*, bool>> idnMap;
	json stack;
	json rpn;
	json lexems, idn;

	string r1, r2;
	size_t labCounter = 0, rCounter = 0, cycleVarCounter = 0;

	bool infoKey;


	void RPNFill();
	void ListOper(json::iterator &it);
	void MathAndLogic(json::iterator &it);
	void Declaration(json::iterator &it); 
	void CondTrans(json::iterator &it);
	void Cycle(json::iterator &it);
	void Input(json::iterator &it);
	void Output(json::iterator &it);
	void Goto(json::iterator &it);
	void Label(json::iterator &it);
	void StackPush(json::iterator it);

	void RPNProcess();
	void DeclarationOperator(json::iterator &it);
	void AssignmentOperator(const json::iterator left, const json::iterator right, const json::iterator end);
	void BinaryArithmeticOperator(const json::iterator it, const json::iterator left, const json::iterator right, const json::iterator end);
	void BinaryLogicalOperator(const json::iterator it, const json::iterator left, const json::iterator right, const json::iterator end);
	void InputOperator(json::iterator it);
	void OutputOperator(json::iterator it);
	void ConditionalTransition(json::iterator &it);
	void UnconditionalTransition(json::iterator &it);

	string getLabel(size_t labCounter) { return string("m" + to_string(labCounter)); }
	string getR(size_t rCounter) { return string("r" + to_string(rCounter)); }
};