#pragma once
#include "LAnalyzer.h"
#include "GParser.h"

using namespace std;
using nlohmann::json;

class SAnalyzer {
public:
	SAnalyzer(const json lexems, const string _grammarFile, const bool _infoKey);
	void startSAnalyzing();

private:
	vector<string> base;
	vector<string> rbuff;
	vector<string> buff;

	json lexems;
	string grammarFile;
	bool infoKey;
	bool end_of_lex = false;
};