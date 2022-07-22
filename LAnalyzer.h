#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <map> 
#include <filesystem>

#include <json.hpp>

#include "myException.hpp"

using namespace std;
using nlohmann::json;

#define IDN_ID 35
#define IDN_CON 36
#define IDN_LABEL 37


class LAnalyzer {
public:
	LAnalyzer(const string &_codeFile, const string _lexemFile);

	void startAnalyzing(); 

	json getLexems() { return lexems; };
	json getIdn() { return idn; };

private:
	json lexems, idn, con, labels;
	vector<pair<size_t, string>> lexem;

	const string alphabet = "abcdefghijklmnopqrstuvwxyz_";
	const string sign = "+-*/()=><!.";

	string codeFile, lexemFile;

	void writeLexTab();
	void writeIdnTab();
	void writeConTab();
	void writeLabTab();
	void writeAll();
};