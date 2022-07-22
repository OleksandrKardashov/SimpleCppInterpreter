#pragma once
#include <iostream>
#include <string>
#include <exception>
#include <vector>
#include <json.hpp>

using namespace std;
using nlohmann::json;

//FILE EXCEPTIONS
class codeFileDoesNotExists : public exception {
public:
	codeFileDoesNotExists() : exception(string("Exception 001: Invalid argument! Code file does not exists!").c_str()) {}
};

class grammarFileDoesNotExists : public exception {
public:
	grammarFileDoesNotExists() : exception(string("Exception 002: Invalid argument! Grammar file does not exists!").c_str()) {}
};

class lexemFileDoesNotExists : public exception {
public:
	lexemFileDoesNotExists() : exception(string("Exception 003: Invalid argument! Lexem file does not exists!").c_str()) {}
};



class codeFileKeyIsRepeating : public exception {
public:
	codeFileKeyIsRepeating(size_t position) : exception(string("Exception 004: Invalid argument! Code file key is repeating! Position: " + to_string(position) + ".").c_str()) {}
};

class grammarFileKeyIsRepeating : public exception {
public:
	grammarFileKeyIsRepeating(size_t position) : exception(string("Exception 005: Invalid argument! Grammar file key is repeating! Position: " + to_string(position) + ".").c_str()) {}
};

class lexemFileKeyIsRepeating : public exception {
public:
	lexemFileKeyIsRepeating(size_t position) : exception(string("Exception 006: Invalid argument! Lexem file key is repeating! Position: " + to_string(position) + ".").c_str()) {}
};



class codeFileKeyIsMissing : public exception {
public:
	codeFileKeyIsMissing() : exception(string("Exception 007: Invalid argument! Code file key is missing!").c_str()) {}
};

class grammarFileKeyIsMissing : public exception {
public:
	grammarFileKeyIsMissing() : exception(string("Exception 008: Invalid argument! Grammar file is missing!").c_str()) {}
};

class lexemFileKeyIsMissing : public exception {
public:
	lexemFileKeyIsMissing() : exception(string("Exception 009: Invalid argument! Lexem file is missing!").c_str()) {}
};




//LEXICAL ANALYZER
class unexpected_dot : public exception {
public:
	unexpected_dot(size_t lineCount) : exception(string("Exception 101: Unexpected dot! Line: " + to_string(lineCount) + ".").c_str()) {}
};

class dot_not_one : public exception {
public:
	dot_not_one(size_t lineCount) : exception(string("Exception 102: Number has more than one dot! Line: " + to_string(lineCount) + ".").c_str()) {}
};

class unexpected_symbol : public exception {
public:
	unexpected_symbol(string sym, size_t lineCount) : exception(string("Exception 103: Unexpected symbol! Symbol: " + sym + ". Line: " + to_string(lineCount) + ".").c_str()) {}
};

class incorrect_label : public exception {
public:
	incorrect_label(size_t lineCount) : exception(string("Exception 104: Incorrect label! Line: " + to_string(lineCount) + ".").c_str()) {}
};

class unknown_symbol : public exception {
public:
	unknown_symbol(size_t lineCount) : exception(string("Exception 105: Unknown symbol! Line: " + to_string(lineCount) + ".").c_str()) {}
};




//SYNTAX ANALYZER
class no_relation : public exception {
public:
	no_relation(string leftStr, string rightStr, size_t lineCount) : exception("Exception 201: No relationship!") {
		this->leftStr = leftStr;
		this->rightStr = rightStr;
		this->lineCount = lineCount;
	}

	string getLeftStr() { return leftStr; }
	string getRightStr() { return rightStr; }
	size_t getLineCount() { return lineCount; }
	
private:
	string leftStr, rightStr;
	size_t lineCount;
};

class no_match : public exception {
public:
	no_match(vector<string> rule) : exception("Exception 202: No match in grammar for the found rule!") {
		this->rule = rule;
	}

	string getRule() {
		string str;
		for (auto i : rule) {
			for (auto j : i) {
				str.push_back(j);
			}
			str.push_back(' ');
		}
		return str;
	}

private:
	vector<string> rule;
};




//GRAMMAR PARSER
class conflict : public exception {
public:
	conflict(string leftStr, string rightStr) : exception("Exception 301: Conflict detected!") {
		this->leftStr = leftStr;
		this->rightStr = rightStr;
	}

	string getLeftStr() { return leftStr; }
	string getRightStr() { return rightStr; }

private:
	string leftStr, rightStr;
};




//Reverse Polish Notation
class id_not_declared : public exception {
public:
	id_not_declared(json::iterator it) : exception("Exception 401: id is not declared!") {
		name = (*it)["Name"].get<string>();
		line = (*it)["Line"].get<int>();
	}

	string getName() { return name; }
	int getLineCount() { return line; }

private:
	string name;
	int line;
};

class id_not_initialized : public exception {
public:
	id_not_initialized(json::iterator it) : exception("Exception 402: id is not initialized!") {
		name = (*it)["Name"].get<string>();
		line = (*it)["Line"].get<int>();
	}

	string getName() { return name; }
	int getLineCount() { return line; }

private:
	string name;
	int line;
};

class incorrect_data_entry : public exception {
public:
	incorrect_data_entry(json::iterator it) : exception("Exception 403: Incorrect data entry!") {
		line = (*it)["Line"].get<int>();
	}

	int getLineCount() { return line; }

private:
	int line;
};

class nested_loop_var_repeat : public exception {
public:
	nested_loop_var_repeat(json::iterator it) : exception("Exception 404: Nested loop variable cannot be repeated!") {
		line = (*it)["Line"].get<int>();
	}

	int getLineCount() { return line; }

private:
	int line;
};

class no_label : public exception {
public:
	no_label(json::iterator it) : exception("Exception 405: Label cannot be found!") {
		line = (*it)["Line"].get<int>();
	}

	int getLineCount() { return line; }

private:
	int line;
};