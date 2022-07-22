#include "RPN.h"

using namespace std;

RPN::RPN(json lexems, json idn, const bool _infoKey) {
	this->lexems = lexems;
	this->idn = idn;
	this->infoKey = _infoKey;

	priority = {
		{ "(", 0 }, { "if", 0 }, { "for", 0 },
		{ ")", 1 }, { "then", 1 }, { "by", 1 }, { "to", 1 }, { "do", 1 },
		{ "=", 2 },
		{ "<", 3 }, { ">", 3 }, { "<=", 3 }, { ">=", 3 }, { "<", 3 }, { "==", 3 }, { "!=", 3 },
		{ "+", 4 }, { "-", 4 },
		{ "*", 5 }, { "/", 5 },
	};

	for (auto i : idn) {
		idnMap[i["Name"].get<string>()] = { NULL, false };
	}
}

void RPN::ReversePolishNotation() {
	RPNFill();
	RPNProcess();
}



void RPN::RPNFill() {
	try {
		if (!std::filesystem::exists("./OutRPN")) {
			std::filesystem::create_directory("OutRPN");
		}

		auto it = lexems.begin();
		it += 3;

		ListOper(it);

		++it;

		if (infoKey) {
			cout << "***********************REVERSE POLISH NOTATION**********************" << endl << endl;
			for (auto i : rpn) {
				cout << i["Name"].get<string>() << " ";
			}
			cout << endl << "********************************************************************" << endl << endl << endl;
		}


		ofstream file("./OutRPN/RPNTab.json");
		file << rpn.dump(4);
		file.close();
	}
	catch (nested_loop_var_repeat &ex) {
		cout << ex.what() << " Line: " << ex.getLineCount() << endl;
		system("pause");
		exit(1);
	}
}

void RPN::ListOper(json::iterator &it) {
	while ((*it)["ID"].get<int>() != 3 && (*it)["ID"].get<int>() != 9 && (*it)["ID"].get<int>() != 12) {
		if ((it + 1) != lexems.end() && ((*it)["ID"].get<int>() == 35 || (*it)["ID"].get<int>() == 36 || (*it)["ID"].get<int>() == 16)) {
			MathAndLogic(it);
		}
		else if ((it + 1) != lexems.end() && (*it)["ID"].get<int>() == 10) {
			CondTrans(it);
		}
		else if ((it + 1) != lexems.end() && (*it)["ID"].get<int>() == 5) {
			Cycle(it);
		}
		else if ((it + 1) != lexems.end() && (*it)["ID"].get<int>() == 4) {
			Declaration(it);
		}
		else if ((it + 1) != lexems.end() && (*it)["ID"].get<int>() == 14) {
			Input(it += 2);
		}
		else if ((it + 1) != lexems.end() && (*it)["ID"].get<int>() == 15) {
			Output(it += 2);
		}
		else if ((it + 1) != lexems.end() && (*it)["ID"].get<int>() == 13) {
			Goto(it);
		}
		else if ((it + 1) != lexems.end() && (*it)["ID"].get<int>() == 37) {
			Label(it);
		}
		else if (priority.find((*it)["Name"].get<string>()) == priority.end()) {
			rpn.push_back(*it);
			++it;
		}
	}
}

void RPN::Declaration(json::iterator &it) {
	rpn.push_back(*it);
	++it;
	rpn.push_back(*it);
	++it;
}

void RPN::MathAndLogic(json::iterator &it) {
	while ((*it)["ID"].get<int>() != 34 && (*it)["ID"].get<int>() != 6 && (*it)["ID"].get<int>() != 7 && (*it)["ID"].get<int>() != 8 && (*it)["ID"].get<int>() != 11) {
		if (priority.find((*it)["Name"].get<string>()) == priority.end()) {
			rpn.push_back(*it);
		}
		else if (priority.find((*it)["Name"].get<string>()) != priority.end()) {
			StackPush(it);
		}
		++it;
	}
	if ((*it)["ID"].get<int>() == 34) {
		while (!stack.empty() && stack.back()["Name"].get<string>() != "if" && stack.back()["Name"].get<string>() != "for") {
			rpn.push_back(stack.back());
			stack.erase(stack.end() - 1);
		}
	}
}

void RPN::CondTrans(json::iterator &it) {
	StackPush(it);
	++it;

	MathAndLogic(it);

	StackPush(it);
	++it;

	ListOper(it);

	if ((*it)["ID"].get<int>() == 12) {
		while (stack.back()["Name"].get<string>() != "if") {
			rpn.push_back(stack.back());
			stack.erase(stack.end() - 1);
		}
		stack.erase(stack.end() - 1);

		rpn.push_back({ { "Name", stack.back()["Name"].get<string>() }, {"ID", 37 } });
		labelMap[stack.back()["Name"].get<string>()] = rpn.size() - 1;
		stack.erase(stack.end() - 1);
		rpn.push_back({ { "Name", ":" }, { "ID", 42 } });
		++it;
	}
	++it;
}

void RPN::Cycle(json::iterator &it) {
	//for
	StackPush(it);
	++it;
	string buff_str = (*it)["Name"].get<string>();
	if (cycleVarHistory.empty() || cycleVarHistory.find((*it)["Name"].get<string>()) == cycleVarHistory.end()) {
		cycleVarHistory.insert(buff_str);
		cycleVar = it;
	}
	else {
		throw nested_loop_var_repeat(it);
	}

	MathAndLogic(it);
	//by
	StackPush(it);
	++it;
	MathAndLogic(it);

	//to
	StackPush(it);
	++it;
	MathAndLogic(it);

	//do
	StackPush(it);
	++it;

	ListOper(it);

	if ((*it)["ID"].get<int>() == 9) {
		while (stack.back()["Name"].get<string>() != "for") {
			rpn.push_back(stack.back());
			stack.erase(stack.end() - 1);
		}

		static int temp = 0;
		ofstream file(std::string("./OutRPN/StackStr") + to_string(++temp) + std::string(".json"));
		file << stack.dump(4);
		file.close();

		rpn.insert(rpn.end(), { { { "Name", (*(stack.end() - 4))["Name"].get<string>() }, {"ID", 37 } }, { { "Name", "UCT" }, {"ID", 41 } }, { { "Name", (*(stack.end() - 2))["Name"].get<string>() }, {"ID", 37 } }, { { "Name", ":" }, { "ID", 42 } } });
		labelMap[(*(stack.end() - 2))["Name"].get<string>()] = rpn.size() - 2;

		for (size_t i = 0; i < 4; ++i) {
			stack.erase(stack.end() - 1);
		}
		++it;
	}
	++it;

	cycleVarHistory.erase(buff_str);
}

void RPN::Input(json::iterator &it) {
	rpn.push_back(*it);
	rpn.push_back({ {"Name", "IN"}, {"ID", 38 }, { "Line", (*it)["Line"].get<int>() } });
	++it;
}

void RPN::Output(json::iterator &it) {
	MathAndLogic(it);
	rpn.push_back({ {"Name", "OUT"}, {"ID", 39 } });
}

void RPN::Goto(json::iterator &it) {
	++it;
	rpn.push_back(*it);
	rpn.push_back({ { "Name", "UCT" }, {"ID", 41 } });
	++it;
}

void RPN::Label(json::iterator &it) {
	rpn.push_back(*it);
	labelMap[(*it)["Name"].get<string>()] = rpn.size() - 1;
	++it;

	rpn.push_back({ { "Name", ":" }, { "ID", 42 } });
	++it;
}

void RPN::StackPush(json::iterator it) {
	if ((stack.empty() || (*it)["Name"].get<string>() == "for" || (*it)["Name"].get<string>() == "if" || (*it)["Name"].get<string>() == "(" || priority.find((*it)["Name"].get<string>())->second > priority.find(stack.back()["Name"].get<string>())->second)
		&& (*it)["Name"].get<string>() != ")" && (*it)["Name"].get<string>() != "then" && (*it)["Name"].get<string>() != "by" && (*it)["Name"].get<string>() != "to" && (*it)["Name"].get<string>() != "do") {
		/*static int temp = 0;
		ofstream file(std::string("StackStr") + to_string(++temp) + std::string(".json"));
		file << stack.dump(4);
		file.close();*/
		if ((*it)["Name"].get<string>() == "for") {
			stack.insert(stack.end(), { { { "Name",  getLabel(++labCounter) } }, { { "Name",  getLabel(++labCounter) } }, { { "Name",  getLabel(++labCounter) } } });
		}
		stack.push_back(*it);
	}
	else if ((*it)["Name"].get<string>() == "do" || (*it)["Name"].get<string>() == "to" || (*it)["Name"].get<string>() == "by" || (*it)["Name"].get<string>() == "then" || (*it)["Name"].get<string>() == ")"
		|| priority.find((*it)["Name"].get<string>())->second < priority.find(stack.back()["Name"].get<string>())->second || priority.find((*it)["Name"].get<string>())->second == priority.find(stack.back()["Name"].get<string>())->second) {

		if ((stack.back()["Name"].get<string>() != "(" && (*it)["Name"].get<string>() == ")") || (stack.back()["Name"].get<string>() != "if" && (*it)["Name"].get<string>() == "then")
			|| (stack.back()["Name"].get<string>() != "for" && ((*it)["Name"].get<string>() == "by" || (*it)["Name"].get<string>() == "to" || (*it)["Name"].get<string>() == "do"))
			|| priority.find((*it)["Name"].get<string>())->second < priority.find(stack.back()["Name"].get<string>())->second || priority.find((*it)["Name"].get<string>())->second == priority.find(stack.back()["Name"].get<string>())->second) {

			rpn.push_back(stack.back());
			stack.erase(stack.end() - 1);
			StackPush(it);
		}
		else if (stack.back()["Name"].get<string>() == "(" && (*it)["Name"].get<string>() == ")") {
			stack.erase(stack.end() - 1);
		}
		else if (stack.back()["Name"].get<string>() == "if" && (*it)["Name"].get<string>() == "then") { // THEN (if)
			rpn.insert(rpn.end(), { { { "Name", getLabel(++labCounter) }, { "ID", 37 } }, { { "Name", "CT" }, { "ID", 40 } } });
			stack.insert(stack.end() - 1, { { { "Name", getLabel(labCounter) } } });
		}
		else if (stack.back()["Name"].get<string>() == "for" && (*it)["Name"].get<string>() == "by") { //BY (for)
			r1 = getR(++rCounter);
			idnMap[r1] = { NULL, true };
			r2 = getR(++rCounter);
			idnMap[r2] = { NULL, true };
			rpn.insert(rpn.end(), { { { "Name", r1 }, {"ID", 35 } }, { { "Name", "1" }, { "ID", 36 } }, { { "Name", "=" }, { "ID", 18 }  }, { { "Name", (*(stack.end() - 4))["Name"].get<string>() }, { "ID", 37 }  }, { { "Name", ":" }, { "ID", 42 } }, { { "Name", r2 }, { "ID", 35 }  } });
			labelMap[(*(stack.end() - 4))["Name"].get<string>()] =  rpn.size() - 3;
		}
		else if (stack.back()["Name"].get<string>() == "for" && (*it)["Name"].get<string>() == "to") { //TO (for)
			
			rpn.insert(rpn.end(), { { { "Name", "=" }, { "ID", 18 }  }, { { "Name", r1 }, { "ID", 35 } }, { { "Name", "0" }, { "ID", 36 } }, { { "Name", "==" }, { "ID", 25 } }, { { "Name", (*(stack.end() - 3))["Name"].get<string>() }, { "ID", 37 } }, { { "Name", "CT" }, { "ID", 40 } },
				{ { "Name", (*cycleVar)["Name"].get<string>() }, { "ID", (*cycleVar)["ID"].get<int>() }, { "Line", (*cycleVar)["Line"].get<int>() } }, { { "Name", (*cycleVar)["Name"].get<string>() }, { "ID", (*cycleVar)["ID"].get<int>() }, { "Line", (*cycleVar)["Line"].get<int>() } },
				{ { "Name", r2 }, { "ID", 35 } }, { { "Name", "+" }, { "ID", 19 } }, { { "Name", "=" }, { "ID", 18 } }, { { "Name", (*(stack.end() - 3))["Name"].get<string>() }, { "ID", 37 } }, { { "Name", ":" }, { "ID", 42 } }, { { "Name", r1 }, { "ID", 35 } }, { { "Name", "0" }, { "ID", 36 } },
				{ { "Name", "=" }, { "ID", 18 } }, { { "Name", (*cycleVar)["Name"].get<string>() }, { "ID", (*cycleVar)["ID"].get<int>() }, { "Line", (*cycleVar)["Line"].get<int>() } } });
			
			labelMap[(*(stack.end() - 3))["Name"].get<string>()] = rpn.size() - 6;
		}
		else if (stack.back()["Name"].get<string>() == "for" && (*it)["Name"].get<string>() == "do") { //DO (for)
			rpn.insert(rpn.end(), { { { "Name", "-" }, { "ID", 20 } }, { { "Name", r2 }, { "ID", 35 } }, { { "Name", "*" }, { "ID", 21 } }, { { "Name", "0" }, { "ID", 36 } }, { { "Name", "<=" }, { "ID", 28 } }, { { "Name", (*(stack.end() - 2))["Name"].get<string>() }, { "ID", 37 } }, { { "Name", "CT" }, { "ID", 40 } } });
		}
	}
}




void RPN::RPNProcess() {
	stack.clear();
	try {
		for (auto it = rpn.begin(); it != rpn.end(); ++it) {

			switch ((*it)["ID"].get<int>()) {
			case 35:
				if (idnMap.find((*it)["Name"].get<string>())->second.second == false) {
					
					throw id_not_declared(it);
				}
				else {
					stack.push_back(*it);
				}
				break;

			case 36:
				stack.push_back(*it);
				break;

			case 4:
				DeclarationOperator(++it);
				break;

			case 18:
				AssignmentOperator(stack.end() - 2, stack.end() - 1, stack.end());
				break;

			case 19:
			case 20:
			case 21:
			case 22:
				BinaryArithmeticOperator(it, stack.end() - 2, stack.end() - 1, stack.end());
				break;

			case 25:
			case 26:
			case 27:
			case 28:
			case 29:
			case 30:
				BinaryLogicalOperator(it, stack.end() - 2, stack.end() - 1, stack.end());
				break;

			case 38:
				InputOperator(it);
				break;

			case 39:
				OutputOperator(it);
				break;

			case 40:
				ConditionalTransition(it);
				break;

			case 41:
				UnconditionalTransition(it);
				break;
			}
		}
	}
	catch (id_not_declared &ex) {
		cout << ex.what() << " ID: " << ex.getName() << ". Line: " << ex.getLineCount() << endl;
		system("pause");
		exit(1);
	}
	catch (id_not_initialized &ex) {
		cout << ex.what() << " ID: " << ex.getName() << ". Line: " << ex.getLineCount() << endl;
		system("pause");
		exit(1);
	}
	catch (incorrect_data_entry &ex) {
		cout << ex.what() << " Line: " << ex.getLineCount() << endl;
		system("pause");
		exit(1);
	}
	catch (no_label &ex) {
		cout << ex.what() << " Line: " << ex.getLineCount() << endl;
		system("pause");
		exit(1);
	}
}

void RPN::DeclarationOperator(json::iterator &it) {
	idnMap[(*it)["Name"].get<string>()].second = true;
	++it;
}

void RPN::AssignmentOperator(const json::iterator left, const json::iterator right, const json::iterator end) {
	if (idnMap.find((*left)["Name"].get<string>())->second.first == nullptr) {
		idnMap.find((*left)["Name"].get<string>())->second.first = new double;
	}
	if (idnMap.find((*right)["Name"].get<string>()) != idnMap.end()) {
		if (idnMap.find((*right)["Name"].get<string>())->second.first != nullptr) {
			*idnMap.find((*left)["Name"].get<string>())->second.first = *idnMap.find((*right)["Name"].get<string>())->second.first;
			stack.erase(stack.end() - 2, stack.end());
		}
		else if (idnMap.find((*right)["Name"].get<string>())->second.first == nullptr) {
			throw id_not_initialized(right);
		}
	}
	else {
		*idnMap.find((*left)["Name"].get<string>())->second.first = stod((*right)["Name"].get<string>());
		stack.erase(left, end);
	}
}

void RPN::BinaryArithmeticOperator(const json::iterator it, const json::iterator left, const json::iterator right, const json::iterator end) {
	double res;
	if (idnMap.find((*left)["Name"].get<string>()) != idnMap.end() && idnMap.find((*right)["Name"].get<string>()) != idnMap.end()) {
		if (idnMap.find((*left)["Name"].get<string>())->second.first != nullptr && idnMap.find((*right)["Name"].get<string>())->second.first != nullptr) {

			if ((*it)["ID"].get<int>() == 19) {
				res = *idnMap.find((*left)["Name"].get<string>())->second.first + *idnMap.find((*right)["Name"].get<string>())->second.first;
			}
			else if ((*it)["ID"].get<int>() == 20) {
				res = *idnMap.find((*left)["Name"].get<string>())->second.first - *idnMap.find((*right)["Name"].get<string>())->second.first;
			}
			else if ((*it)["ID"].get<int>() == 21) {
				res = *idnMap.find((*left)["Name"].get<string>())->second.first * *idnMap.find((*right)["Name"].get<string>())->second.first;
			}
			else if ((*it)["ID"].get<int>() == 22) {
				res = *idnMap.find((*left)["Name"].get<string>())->second.first / *idnMap.find((*right)["Name"].get<string>())->second.first;
			}

			stack.erase(left, end);
			
			stack.push_back({ { "Name", to_string(res) } });
		}
		else if (idnMap.find((*left)["Name"].get<string>())->second.first == nullptr) {
			throw id_not_initialized(left);
		}
		else if (idnMap.find((*right)["Name"].get<string>())->second.first == nullptr) {
			throw id_not_initialized(right);
		}
	}
	else if (idnMap.find((*left)["Name"].get<string>()) == idnMap.end() && idnMap.find((*right)["Name"].get<string>()) != idnMap.end()) {
		if (idnMap.find((*right)["Name"].get<string>())->second.first != nullptr) {

			if ((*it)["ID"].get<int>() == 19) {
				res = stod((*left)["Name"].get<string>()) + *idnMap.find((*right)["Name"].get<string>())->second.first;
			}
			else if ((*it)["ID"].get<int>() == 20) {
				res = stod((*left)["Name"].get<string>()) - *idnMap.find((*right)["Name"].get<string>())->second.first;
			}
			else if ((*it)["ID"].get<int>() == 21) {
				res = stod((*left)["Name"].get<string>()) * *idnMap.find((*right)["Name"].get<string>())->second.first;
			}
			else if ((*it)["ID"].get<int>() == 22) {
				res = stod((*left)["Name"].get<string>()) / *idnMap.find((*right)["Name"].get<string>())->second.first;
			}

			stack.erase(left, end);
			stack.push_back({ { "Name", to_string(res) } });
		}
		else if (idnMap.find((*right)["Name"].get<string>())->second.first == nullptr) {
			throw id_not_initialized(right);
		}
	}
	else if (idnMap.find((*left)["Name"].get<string>()) != idnMap.end() && idnMap.find((*right)["Name"].get<string>()) == idnMap.end()) {
		if (idnMap.find((*left)["Name"].get<string>())->second.first != nullptr) {

			if ((*it)["ID"].get<int>() == 19) {
				res = *idnMap.find((*left)["Name"].get<string>())->second.first + stod((*right)["Name"].get<string>());
			}
			else if ((*it)["ID"].get<int>() == 20) {
				res = *idnMap.find((*left)["Name"].get<string>())->second.first - stod((*right)["Name"].get<string>());
			}
			else if ((*it)["ID"].get<int>() == 21) {
				res = *idnMap.find((*left)["Name"].get<string>())->second.first * stod((*right)["Name"].get<string>());
			}
			else if ((*it)["ID"].get<int>() == 22) {
				res = *idnMap.find((*left)["Name"].get<string>())->second.first / stod((*right)["Name"].get<string>());
			}

			stack.erase(left, end);
			stack.push_back({ { "Name", to_string(res) } });
		}
		else if (idnMap.find((*left)["Name"].get<string>())->second.first == nullptr) {
			throw id_not_initialized(left);
		}
	}
	else if (idnMap.find((*left)["Name"].get<string>()) == idnMap.end() && idnMap.find((*right)["Name"].get<string>()) == idnMap.end()) {

		if ((*it)["ID"].get<int>() == 19) {
			res = stod((*left)["Name"].get<string>()) + stod((*right)["Name"].get<string>());
		}
		else if ((*it)["ID"].get<int>() == 20) {
			res = stod((*left)["Name"].get<string>()) - stod((*right)["Name"].get<string>());
		}
		else if ((*it)["ID"].get<int>() == 21) {
			res = stod((*left)["Name"].get<string>()) * stod((*right)["Name"].get<string>());
		}
		else if ((*it)["ID"].get<int>() == 22) {
			res = stod((*left)["Name"].get<string>()) / stod((*right)["Name"].get<string>());
		}

		stack.erase(left, end);
		stack.push_back({ { "Name", to_string(res) } });
	}
}

void RPN::BinaryLogicalOperator(const json::iterator it, const json::iterator left, const json::iterator right, const json::iterator end) {
	string res;
	if (idnMap.find((*left)["Name"].get<string>()) != idnMap.end() && idnMap.find((*right)["Name"].get<string>()) != idnMap.end()) {
		if (idnMap.find((*left)["Name"].get<string>())->second.first != nullptr && idnMap.find((*right)["Name"].get<string>())->second.first != nullptr) {

			if ((*it)["ID"].get<int>() == 25) {
				(*idnMap.find((*left)["Name"].get<string>())->second.first == *idnMap.find((*right)["Name"].get<string>())->second.first) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 26) {
				(*idnMap.find((*left)["Name"].get<string>())->second.first != *idnMap.find((*right)["Name"].get<string>())->second.first) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 27) {
				(*idnMap.find((*left)["Name"].get<string>())->second.first >= *idnMap.find((*right)["Name"].get<string>())->second.first) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 28) {
				(*idnMap.find((*left)["Name"].get<string>())->second.first <= *idnMap.find((*right)["Name"].get<string>())->second.first) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 29) {
				(*idnMap.find((*left)["Name"].get<string>())->second.first < *idnMap.find((*right)["Name"].get<string>())->second.first) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 30) {
				(*idnMap.find((*left)["Name"].get<string>())->second.first > *idnMap.find((*right)["Name"].get<string>())->second.first) ? res = "true" : res = "false";
			}

			stack.erase(left, end);
			stack.push_back({ { "Name", res } });
		}
		else if (idnMap.find((*left)["Name"].get<string>())->second.first == nullptr) {
			throw id_not_initialized(left);
		}
		else if (idnMap.find((*right)["Name"].get<string>())->second.first == nullptr) {
			throw id_not_initialized(right);
		}
	}
	else if (idnMap.find((*left)["Name"].get<string>()) == idnMap.end() && idnMap.find((*right)["Name"].get<string>()) != idnMap.end()) {
		if (idnMap.find((*right)["Name"].get<string>())->second.first != nullptr) {

			if ((*it)["ID"].get<int>() == 25) {
				(stod((*left)["Name"].get<string>()) == *idnMap.find((*right)["Name"].get<string>())->second.first) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 26) {
				(stod((*left)["Name"].get<string>()) != *idnMap.find((*right)["Name"].get<string>())->second.first) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 27) {
				(stod((*left)["Name"].get<string>()) >= *idnMap.find((*right)["Name"].get<string>())->second.first) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 28) {
				(stod((*left)["Name"].get<string>()) <= *idnMap.find((*right)["Name"].get<string>())->second.first) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 29) {
				(stod((*left)["Name"].get<string>()) < *idnMap.find((*right)["Name"].get<string>())->second.first) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 30) {
				(stod((*left)["Name"].get<string>()) > *idnMap.find((*right)["Name"].get<string>())->second.first) ? res = "true" : res = "false";
			}

			stack.erase(left, end);
			stack.push_back({ { "Name", res } });
		}
		else if (idnMap.find((*right)["Name"].get<string>())->second.first == nullptr) {
			throw id_not_initialized(right);
		}
	}
	else if (idnMap.find((*left)["Name"].get<string>()) != idnMap.end() && idnMap.find((*right)["Name"].get<string>()) == idnMap.end()) {
		if (idnMap.find((*left)["Name"].get<string>())->second.first != nullptr) {

			if ((*it)["ID"].get<int>() == 25) {
				(*idnMap.find((*left)["Name"].get<string>())->second.first == stod((*right)["Name"].get<string>())) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 26) {
				(*idnMap.find((*left)["Name"].get<string>())->second.first != stod((*right)["Name"].get<string>())) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 27) {
				(*idnMap.find((*left)["Name"].get<string>())->second.first >= stod((*right)["Name"].get<string>())) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 28) {
				(*idnMap.find((*left)["Name"].get<string>())->second.first <= stod((*right)["Name"].get<string>())) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 29) {
				(*idnMap.find((*left)["Name"].get<string>())->second.first < stod((*right)["Name"].get<string>())) ? res = "true" : res = "false";
			}
			else if ((*it)["ID"].get<int>() == 30) {
				(*idnMap.find((*left)["Name"].get<string>())->second.first > stod((*right)["Name"].get<string>())) ? res = "true" : res = "false";
			}

			stack.erase(left, end);
			stack.push_back({ { "Name", res } });
		}
		else if (idnMap.find((*left)["Name"].get<string>())->second.first == nullptr) {
			throw id_not_initialized(left);
		}
	}
	else if (idnMap.find((*left)["Name"].get<string>()) == idnMap.end() && idnMap.find((*right)["Name"].get<string>()) == idnMap.end()) {

		if ((*it)["ID"].get<int>() == 25) {
			(stod((*left)["Name"].get<string>()) == stod((*right)["Name"].get<string>())) ? res = "true" : res = "false";
		}
		else if ((*it)["ID"].get<int>() == 26) {
			(stod((*left)["Name"].get<string>()) != stod((*right)["Name"].get<string>())) ? res = "true" : res = "false";
		}
		else if ((*it)["ID"].get<int>() == 27) {
			(stod((*left)["Name"].get<string>()) >= stod((*right)["Name"].get<string>())) ? res = "true" : res = "false";
		}
		else if ((*it)["ID"].get<int>() == 28) {
			(stod((*left)["Name"].get<string>()) <= stod((*right)["Name"].get<string>())) ? res = "true" : res = "false";
		}
		else if ((*it)["ID"].get<int>() == 29) {
			(stod((*left)["Name"].get<string>()) < stod((*right)["Name"].get<string>())) ? res = "true" : res = "false";
		}
		else if ((*it)["ID"].get<int>() == 30) {
			(stod((*left)["Name"].get<string>()) > stod((*right)["Name"].get<string>())) ? res = "true" : res = "false";
		}

		stack.erase(left, end);
		stack.push_back({ { "Name", res } });
	}
}

void RPN::InputOperator(json::iterator it) {
	string str, storage;
	bool flag = false, flagForDot = false;
	cout << "Enter: ";
	cin >> str;

	unsigned char a;
	auto i = str.begin();

	while (i != str.end()) {
		a = *i;
		str.erase(i);

		if (!isdigit(a) && a != '.') {
			throw incorrect_data_entry(it);
		}

		storage.clear();
		flag = false;
		flagForDot = false;

		if (isdigit(a)) {
			flag = true;
			storage.push_back(a);
		}

		if (a == '.') {
			flagForDot = true;
			storage.push_back(a);
		}

		if (flag || flagForDot) {

			while (i != str.end()) {
				a = *i;
				str.erase(i);

				if ((!isdigit(a) && a != '.') || (flagForDot && a == '.')) {
					throw incorrect_data_entry(it);
				}

				if (isdigit(a)) {
					storage.push_back(a);
				}
				if (a == '.') {
					flagForDot = true;
					storage.push_back(a);
				}
			}
		}
	}

	if (idnMap.find((*(stack.end() - 1))["Name"].get<string>())->second.first == nullptr) {
		idnMap.find((*(stack.end() - 1))["Name"].get<string>())->second.first = new double;
	}

	*(idnMap.find((*(stack.end() - 1))["Name"].get<string>())->second.first) = stod(storage);
	stack.erase(stack.end() - 1, stack.end());

}

void RPN::OutputOperator(json::iterator it) {
	if (idnMap.find((*(stack.end() - 1))["Name"].get<string>()) != idnMap.end()) {
		cout << *idnMap.find((*(stack.end() - 1))["Name"].get<string>())->second.first << endl;
	}
	else {
		cout << stod((*(stack.end() - 1))["Name"].get<string>()) << endl;
	}
	stack.erase(stack.end() - 1, stack.end());
}

void RPN::ConditionalTransition(json::iterator &it) {
	if ((*(stack.end() - 1))["Name"].get<string>() == "false") {
		it = rpn.begin() + labelMap.find((*(it - 1))["Name"].get<string>())->second;
	}
	stack.erase(stack.end() - 1);
}

void RPN::UnconditionalTransition(json::iterator &it) {
	if (labelMap.find((*(it - 1))["Name"].get<string>()) != labelMap.end()) {
		it = rpn.begin() + labelMap.find((*(it - 1))["Name"].get<string>())->second;
	}
	else {
		throw no_label(it - 1);
	}
}


RPN::~RPN() {
	for (auto &i : idnMap) {
		if (i.second.first != nullptr) {
			delete i.second.first;
		}
	}
}