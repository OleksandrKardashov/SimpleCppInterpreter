#include "SAnalyzer.h"

using namespace std;

SAnalyzer::SAnalyzer(const json lexems, const string _grammarFile, const bool _infoKey) {
	this->lexems = lexems;
	grammarFile = _grammarFile;
	infoKey = _infoKey;
}

void SAnalyzer::startSAnalyzing() {

	GParser objB(grammarFile, infoKey);
	objB.GFill();

	for (auto it = lexems.begin(); it != lexems.end(); ++it) {

		switch ((*it)["ID"].get<int>()) {
		case 35:
			(*it)["Name"] = "id";
			break;
		case 36:
			(*it)["Name"] = "fixConst";
			break;
		case 37:
			(*it)["Name"] = "lab";
			break;
		case 25:
			(*it)["Name"] = "comparisonSign";
			break;
		case 26:
			(*it)["Name"] = "comparisonSign";
			break;
		case 27:
			(*it)["Name"] = "comparisonSign";
			break;
		case 28:
			(*it)["Name"] = "comparisonSign";
			break;
		case 29:
			(*it)["Name"] = "comparisonSign";
			break;
		case 30:
			(*it)["Name"] = "comparisonSign";
			break;
		}
	}
	
	base.push_back((*lexems.begin())["Name"].get<string>());

	try {
		for (auto it = lexems.begin() + 1; it != lexems.end(); ++it) {

			if (!end_of_lex && objB.rTable[{ *(base.end() - 1), (*it)["Name"].get<string>() }].size() == 1 && *(objB.rTable[{ *(base.end() - 1), (*it)["Name"].get<string>() }].begin()) != 3) {
				//cout << "Base: ";
				//for (auto i : base) {
				//	cout << i << ' ';
				//}

				//cout << "  <==  " << (*it)["Name"].get<string>();
				//cout << endl;
				base.push_back((*it)["Name"].get<string>());

				if (it + 1 == lexems.end()) {
					it--;
					end_of_lex = true;
				}
			}
			else if (it + 1 == lexems.end() || objB.rTable[{ *(base.end() - 1), (*it)["Name"].get<string>() }].size() == 1 && *(objB.rTable[{ *(base.end() - 1), (*it)["Name"].get<string>() }].begin()) == 3) {

				for (auto jt = base.rbegin(); jt != base.rend(); ++jt) {
					if ((jt + 1) != base.rend() && *(objB.rTable[{ *(jt + 1), *jt }].begin()) == 1) {
						rbuff.push_back(*jt);
					}
					else if ((jt + 1) != base.rend() && *(objB.rTable[{ *(jt + 1), *jt }].begin()) == 2 || (jt + 1) == base.rend()) {
						rbuff.push_back(*jt);

						for (auto kt = rbuff.rbegin(); kt != rbuff.rend(); ++kt) {
							buff.push_back(*kt);
						}
						break;
					}
				}

				for (auto lt : rbuff) {
					base.pop_back();
				}

				/*for (auto i : buff) {
					cout << i;
				}
				cout << endl;*/

				if (objB.rgrammar.find(buff) != objB.rgrammar.end()) {
					if (objB.rgrammar[buff] != "Program") {
						--it;
					}

					base.push_back(objB.rgrammar[buff]);

					rbuff.clear();
					buff.clear();
				}
				else {
					throw no_match(buff);
				}
			}
			else if (objB.rTable[{ *(base.end() - 1), (*it)["Name"].get<string>() }].size() == 0) {
				throw no_relation(*(base.end() - 1), (*it)["Name"].get<string>(), (*it)["Line"].get<size_t>());
			}
		}
	}
	catch (no_relation &ex) {
		cout << ex.what() << ex.getLeftStr() << " and " << ex.getRightStr() << ". Line: " << ex.getLineCount() << endl;
		system("pause");
		exit(1);
	}
	catch (no_match &ex) {
		cout << ex.what() << " Rule: " << ex.getRule() << endl;
		system("pause");
		exit(1);
	}
}