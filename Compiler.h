#pragma once
#include "LAnalyzer.h"
#include "SAnalyzer.h"
#include "RPN.h"

#include <filesystem>

class Compiler {
public:
	Compiler();

	void Begin();

private:
	string codeFile, grammarFile, lexemFile;
	map<int, pair<string, string>> keys;

	void lAnalyzer(json &lexems, json &idn);
	void sAnalyzer(json lexems);
	void rpn(json &lexems, json &idn);

	void fileFind(int keyid);
	bool isInfoActive();
	bool isHelpActive();
	void helpOutput();
};