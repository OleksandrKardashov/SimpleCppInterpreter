#include "GParser.h"

using namespace std;

GParser::GParser(const string &gName, const bool _infoKey) {

	infoKey = _infoKey;

	//read the grammar from the file "gName"
	ifstream fin(gName);
	if (fin.is_open()) {

		string buff;
		vector<string> reader;
		while (fin.peek() != char_traits<char>::eof()) {
			unsigned char a = fin.get();

			if (a != ' ' && a != '\n' && a != '.') {
				buff.push_back(a);
			}
			else if (!buff.empty()) {
				reader.push_back(buff);
				buff.clear();
			}

			if (a == '.') {
				buff.push_back(a);
				reader.push_back(buff);
				buff.clear();
			}
		}

		vector<pair<string, bool>> right;
		string left;

		bool flag = false;
		for (auto i : reader) {
			if (i == ":=") {
				flag = true;
				continue;
			}
			else if (i == ".") {
				flag = false;
			}

			if (flag) {
				string terminal;
				if (i.front() == '"' && i.back() == '"') {
					for (size_t j = 1; j < i.size() - 1; ++j) {
						
						terminal.push_back(i[j]);
					}
					if (terminal == "¶") {
						terminal = "\n";
					}
					right.push_back(make_pair(terminal, true));
				}
				else {
					right.push_back(make_pair(i, false));
				}
			}
			else if (!flag && i != ".") {
				left = i;
			}

			if (!left.empty() && i == ".") {
				//grammar[left].push_back(mright);
				grammar.insert({ left, right });

				left.clear();
				right.clear();
			}
		}

		if(infoKey)
			this->outputGrammar();
	}
}

void GParser::GFill() {
	//filling of the multimap "Equal" - rETable
	for (auto it = grammar.begin(); it != grammar.end(); ++it) {
		for (auto jt = it->second.begin(); jt != it->second.end(); ++jt) {
			if (jt + 1 != it->second.end()) {
				rETable.insert({ jt->first, { (jt + 1)->first, (jt + 1)->second } });
			}
		}
	}

	//filling of the map "First+" - rFTable
	for (auto it = grammar.begin(); it != grammar.end(); ++it) {
		if (it->second.size() != 0) {
			rFTable[it->first].insert({ it->second.front().first, it->second.front().second });
			if (it->second.front().second == 0) {
				RFFill(it, it);
			}
		}
	}

	//filling of the map "Last+" - rLTable
	for (auto it = grammar.begin(); it != grammar.end(); ++it) {
		if (it->second.size() != 0) {
			rLTable[it->first].insert({ it->second.back().first, it->second.back().second });
			if (it->second.back().second == 0) {
				RLFill(it, it);
			}
		}
	}

	//filling of the map "Relation" - rTable
	for (auto i : rETable) {
		rTable[{i.first, i.second.first}].insert(1); //insert := in rTable

		//insert "<" relation in rTable
		auto jt = rFTable.find(i.second.first);
		if (jt != rFTable.end()) {
			for (auto j : jt->second) {
				rTable[{i.first, j.first}].insert(2);
			}
		}

		//insert ">" relation in rTable
		auto kt = rLTable.find(i.first);
		if (kt != rLTable.end()) {
			for (auto k : kt->second) {
				if (i.second.second == true) {
					rTable[{k.first, i.second.first}].insert(3);
				}
				else {
					auto lt = rFTable.find(i.second.first);
					if (lt != rFTable.end()) {
						for (auto l : lt->second) {
							rTable[{k.first, l.first}].insert(3);
						}
					}
				}
			}
		}
	}

	if(infoKey)
		this->outputRelations();

	try {
		for (auto i : rTable) {
			if (i.second.size() > 1) {
				throw conflict(i.first.first, i.first.second);
			}
		}
	}
	catch (conflict &ex) {
		cout << ex.what() << ex.getLeftStr() << " and " << ex.getRightStr() << endl;
		system("pause");
		exit(1);
	}

	rgrammar = rgFill(grammar, rgrammar);
}

//recursive filling of the "First+" table
void GParser::RFFill(multimap<string, vector<pair<string, bool>>>::iterator it, multimap<string, vector<pair<string, bool>>>::iterator jt) {
	auto range = grammar.equal_range(jt->second.front().first);

	for (auto i = range.first; i != range.second; ++i) {
		rFTable[it->first].insert({ i->second.front().first, i->second.front().second });

		if (i->second.front().second == 0 && i->first != i->second.front().first) {
			RFFill(it, i);
		}
	}
}

//recursive filling of the "Last+" table
void GParser::RLFill(multimap<string, vector<pair<string, bool>>>::iterator it, multimap<string, vector<pair<string, bool>>>::iterator jt) {
	auto range = grammar.equal_range(jt->second.back().first);

	for (auto i = range.first; i != range.second; ++i) {
		rLTable[it->first].insert({ i->second.back().first, i->second.back().second });

		if (i->second.back().second == 0 && i->first != i->second.back().first) {
			RLFill(it, i);
		}
	}
}

//reverse grammar filling: A = i + i => i + i = A
map<vector<string>, string> GParser::rgFill(multimap<string, vector<pair<string, bool>>> grammar, map<vector<string>, string> rgrammar) {
	vector<string> rgbuff;
	for (auto i : grammar) {
		for (auto j : i.second) {
			rgbuff.push_back(j.first);
		}
		rgrammar[rgbuff] = i.first;
		rgbuff.clear();
	}
	return rgrammar;
}



void GParser::outputGrammar() {
	//output of the grammar in console
	cout << "*******************GRAMMAR*******************\n";
	for (auto i : grammar) {
		cout << i.first << " :=";
		for (auto j : i.second) {
			cout << ' ' << j.first;
		}
		cout << '\n';
	}
	cout << "*********************************************\n\n";
}

void GParser::outputRelations() {
	//output Equal table
	cout << "*******************EQUAL TABLE*******************\n";
	for (auto i : rETable) {
		cout << i.first << " = " << i.second.first << endl;
	}
	cout << "*************************************************\n\n";


	//output First+ table
	cout << "******************* FIRST+ *******************\n";
	for (auto i : rFTable) {
		cout << i.first << " => ";
		for (auto j : i.second) {
			cout << j.first << "  ";
		}
		cout << endl << endl;
	}
	cout << "**********************************************\n\n";


	//output Last+ table
	cout << "******************* LAST+ *******************\n";
	for (auto i : rLTable) {
		cout << i.first << " => ";
		for (auto j : i.second) {
			cout << j.first << "  ";
		}
		cout << endl << endl;
	}
	cout << "*********************************************\n\n";

	//output Relation table
	cout << "*******************RELATION TABLE*******************\n";
	for (auto i : rTable) { 
		cout << i.first.first << " | ";
		for (auto j : i.second) {
			if (j == 1)
				cout << "=" << ' ';
			if (j == 2)
				cout << "<" << ' ';
			if (j == 3)
				cout << ">" << ' ';
		}
		cout << "| " << i.first.second << endl;
	}
	cout << "****************************************************\n\n";
}