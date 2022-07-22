#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <set>
#include "myException.hpp"

using namespace std;

class GParser {
public:
	GParser(const string &gName, const bool _infoKey);
	void GFill();

	multimap<string, vector<pair<string, bool>>> grammar;
	map<vector<string>, string> rgrammar;
	map<pair<string, string>, set<size_t>> rTable;

private:
	multimap<string, pair<string, bool>> rETable;
	map<string, map<string, bool>> rFTable;
	map<string, map<string, bool>> rLTable;

	bool infoKey;

	void RFFill(multimap<string, vector<pair<string, bool>>>::iterator it, multimap<string, vector<pair<string, bool>>>::iterator jt);
	void RLFill(multimap<string, vector<pair<string, bool>>>::iterator it, multimap<string, vector<pair<string, bool>>>::iterator jt);
	map<vector<string>, string> rgFill(multimap<string, vector<pair<string, bool>>> grammar, map<vector<string>, string> rgrammar);

	void outputGrammar();
	void outputRelations();
};