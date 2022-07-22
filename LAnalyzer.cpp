#include "LAnalyzer.h"

using namespace std;

LAnalyzer::LAnalyzer(const string& _codeFile, const string _lexemFile) {
	codeFile = _codeFile;
	lexemFile = _lexemFile;

	ifstream fin(lexemFile);

	if (fin.is_open()) {
		string reader;
		int i = 1; 

		while (fin.peek() != char_traits<char>::eof()) {
			getline(fin, reader);
			lexem.push_back(make_pair(i, reader));
			i++;
		}
		fin.close();
	}
}

void LAnalyzer::startAnalyzing() {
	ifstream fin(codeFile);

	bool flag;
	bool flagForDot;

	string buff;
	string storage;

	size_t idxIter = 1;
	size_t idxConstIter = 1;
	size_t idxLabelIter = 1;
	size_t lineCount = 1;

	if (fin.is_open()) {
		try {
			while (fin.peek() != char_traits<char>::eof()) {

				unsigned char a = fin.get();

				storage.clear();
				flag = false;
				flagForDot = false;

				if (a == '\n') {
					lexems.push_back({ { "Line", lineCount}, { "Name", "\n" }, { "ID", 34 } });
					++lineCount;
					continue;
				}

				//BRANCH 1
				for (auto i : alphabet) {
					if (a == i) {
						flag = true;
						storage.push_back(a);
						break;
					}
				}
				if (flag) {

					if (fin.peek() == char_traits<char>::eof()) {
						flag = false;
						for (const auto& it : lexem) {
							if (storage == it.second) {
								flag = true;
								lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", it.first } });
								break;
							}
						}
						if (!flag) {
							fin.unget();

							for (auto i : idn) {
								if (i["Name"] == storage) {
									flag = true;
									lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", IDN_ID }, { "Index", i["Index"] } });
									break;
								}
							}
							if (!flag) {
								lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", IDN_ID }, { "Index", idxIter } });
								idn.push_back({ { "Name", storage }, { "Index", idxIter++ } });
							}

							break;
						}
					}
					while (fin.peek() != char_traits<char>::eof()) {
						flag = false;
						a = fin.get();

						for (auto i : alphabet) {
							if (a == i) {
								flag = true;
								storage.push_back(a);
								break;
							}
						}

						if (flag && fin.peek() != char_traits<char>::eof()) {
							continue;
						}

						if (isdigit(a)) {
							storage.push_back(a);
							if (fin.peek() != char_traits<char>::eof()) {
								continue;
							}
						}


						flag = false;
						for (const auto& it : lexem) {
							if (storage == it.second) {
								flag = true;
								lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", it.first } });
								break;
							}
						}
						if (!flag) {
							fin.unget();

							for (auto i : idn) {
								if (i["Name"] == storage) {
									flag = true;
									lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", IDN_ID }, { "Index", i["Index"] } });
									break;
								}
							}
							if (!flag) {
								lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", IDN_ID }, { "Index", idxIter } });
								idn.push_back({ { "Name", storage }, { "Index", idxIter++ } });
							}

							break;
						}

						fin.unget();
						break;
					}
				}

				//BRANCH 2
				flag = false;
				if (isdigit(a)) {
					flag = true;
					storage.push_back(a);
				}

				if (a == '.') {
					flagForDot = true;
					storage.push_back(a);
				}

				if (flag || flagForDot) {

					if (flag && fin.peek() == char_traits<char>::eof()) {
						lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", IDN_CON }, { "Index", idxConstIter } });
						con.push_back({ { "Name", storage }, { "Index", idxConstIter++ } });
						break;
					}
					while (fin.peek() != char_traits<char>::eof()) {
						a = fin.get();

						//if dot detected again
						if (storage == "." && !isdigit(a)) {
							throw unexpected_dot(lineCount);
						}
						if (flagForDot && a == '.') {
							throw dot_not_one(lineCount);
						}
						//if the first symbol is a number
						if (!flagForDot) {

							if (isdigit(a)) {
								storage.push_back(a);
								if (fin.peek() != char_traits<char>::eof()) {
									continue;
								}
							}

							if (a == '.') {
								flagForDot = true;
								storage.push_back(a);
								if (fin.peek() != char_traits<char>::eof()) {
									continue;
								}
							}

							if (!isdigit(a) || a != '.' || fin.peek() == char_traits<char>::eof()) {
								flag = false;
								for (auto i : con) {
									if (i["Name"] == storage) {
										flag = true;
										lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", IDN_CON }, { "Index", i["Index"] } });
										break;
									}
								}
								if (!flag) {
									lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", IDN_CON }, { "Index", idxConstIter } });
									con.push_back({ { "Name", storage }, { "Index", idxConstIter++ } });
								}

								if (!isdigit(a) || a != '.') {
									fin.unget();
								}
								break;
							}
						}
						//if the first symbol is a dot
						if (flagForDot) {
							if (isdigit(a)) {
								storage.push_back(a);
								if (fin.peek() != char_traits<char>::eof()) {
									continue;
								}
							}
							flag = false;
							for (auto i : con) {
								if (i["Name"] == storage) {
									flag = true;
									lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", IDN_CON }, { "Index", i["Index"] } });
									break;
								}
							}
							if (!flag) {
								lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", IDN_CON }, { "Index", idxConstIter } });
								con.push_back({ { "Name", storage }, { "Index", idxConstIter++ } });
							}
							fin.unget();
							break;
						}
					}
					continue;
				}


				//BRANCH 3
				flag = false;
				for (auto i : sign) {
					if (a == i) {
						flag = true;
						storage.push_back(a);
						break;
					}
				}
				if (flag && fin.peek() == char_traits<char>::eof()) {
					for (const auto& it : lexem) {
						if (storage == it.second) {
							lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", it.first } });
							break;
						}
					}
				}
				else if (flag && fin.peek() != char_traits<char>::eof()) {
					storage.push_back(fin.get());

					flag = false;
					for (const auto& it : lexem) {
						if (storage == it.second) {
							flag = true;
							lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", it.first } });
							break;
						}
					}
					if (flag) {
						continue;
					}
					else {
						fin.unget();
						storage.pop_back();
						flag = false;
						for (const auto& it : lexem) {
							if (storage == it.second) {
								flag = true;
								lexems.push_back({ { "Line", lineCount}, { "Name", storage }, { "ID", it.first } });
								break;
							}
						}
						if (flag) {
							continue;
						}
						else {
							throw unexpected_symbol(storage, lineCount);
						}
					}
				}


				//BRANCH 4
				flag = false;
				if (a == '@') {
					storage.push_back(a);
					if (fin.peek() == char_traits<char>::eof()) {
						throw incorrect_label(lineCount);
					}

					a = fin.get();

					for (auto i : alphabet) {
						if (a == i) {
							flag = true;
							storage.push_back(a);
							break;
						}
					}
					if (flag) {
						if (fin.peek() == char_traits<char>::eof()) {
							flag = false;
							for (auto i : labels) {
								if (i["Name"] == storage) {
									flag = true;
									lexems.push_back({ { "Line", lineCount }, { "Name", storage }, { "ID", IDN_LABEL }, { "Index", i["Index"] } });
									break;
								}
							}
							if (!flag) {
								lexems.push_back({ { "Line", lineCount }, { "Name", storage }, { "ID", IDN_LABEL }, { "Index", idxLabelIter } });
								labels.push_back({ { "Name", storage }, { "Index", idxLabelIter++ } });
							}
							break;
						}

						while (fin.peek() != char_traits<char>::eof()) {
							flag = false;
							a = fin.get();

							for (auto i : alphabet) {
								if (a == i) {
									flag = true;
									storage.push_back(a);
									break;
								}
							}

							if (flag && fin.peek() != char_traits<char>::eof()) {
								continue;
							}

							if (isdigit(a)) {
								storage.push_back(a);
								if (fin.peek() != char_traits<char>::eof()) {
									continue;
								}
							}

							flag = false;
							for (auto i : labels) {
								if (i["Name"] == storage) {
									flag = true;
									lexems.push_back({ { "Line", lineCount }, { "Name", storage }, { "ID", IDN_LABEL }, { "Index", i["Index"] } });
									break;
								}
							}
							if (!flag) {
								lexems.push_back({ { "Line", lineCount }, { "Name", storage }, { "ID", IDN_LABEL }, { "Index", idxLabelIter } });
								labels.push_back({ { "Name", storage }, { "Index", idxLabelIter++ } });
							}
							fin.unget();
							break;
						}
					}
					else {
						throw incorrect_label(lineCount);
					}
				}
			 
				//BRANCH 5
				flag = false;
				
				if (!isdigit(a) && a != ' ' && a != '\t' && a != '\n' && a != '@') {
					flag = true;
					for (auto i : alphabet) {
						if (a == i) {
							flag = false;
							break;
						}
					}
					if (!flag) {
						continue;
					}
					for (auto i : sign) {
						if (a == i) {
							flag = false;
							break;
						}
					}
					if (flag) {
						throw unknown_symbol(lineCount);
					}
				}
			}
			fin.close();
		}
		catch (exception& ex) {
			cout << ex.what() << endl;
			system("pause");
			exit(1);
		}
	}

	//OUTPUT FILES TO ./OutLAnalyzer dir
	writeAll();
}


void LAnalyzer::writeLexTab() {
	ofstream file("./OutLAnalyzer/lexemsTab.json");
	file << lexems.dump(4);
	file.close();
}

void LAnalyzer::writeIdnTab() {
	ofstream file("./OutLAnalyzer/idenTab.json");
	file << idn.dump(4);
	file.close();
}

void LAnalyzer::writeConTab() {
	ofstream file("./OutLAnalyzer/constTab.json");
	file << con.dump(4);
	file.close();
}

void LAnalyzer::writeLabTab() {
	ofstream file("./OutLAnalyzer/labelsTab.json");
	file << labels.dump(4);
	file.close();
}

void LAnalyzer::writeAll() {
	if (!std::filesystem::exists("./OutLAnalyzer")) {
		std::filesystem::create_directory("OutLAnalyzer");
	}
	writeLexTab();
	writeIdnTab();
	writeConTab();
	writeLabTab();
}