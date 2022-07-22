#include "Compiler.h"


Compiler::Compiler() {
	try {
		keys[0] = make_pair("--codePath", "-cp");
		keys[1] = make_pair("--grammarPath", "-gp");
		keys[2] = make_pair("--lexemPath", "-lp");
		keys[3] = make_pair("--info", "-i");
		keys[4] = make_pair("--help", "-h");

		this->fileFind(0);
		this->fileFind(1); 
		this->fileFind(2);
	}
	catch (exception &ex) {
		cout << ex.what() << endl;
		system("pause");
		exit(1);
	}
}


void Compiler::Begin() {
	json lexems, idn;

	if (this->isHelpActive()) {
		helpOutput();
	}

	lAnalyzer(lexems, idn);
	sAnalyzer(lexems); 
	rpn(lexems, idn);
}


void Compiler::lAnalyzer(json &lexems, json &idn) {
	LAnalyzer obj(codeFile, lexemFile);
	obj.startAnalyzing();

	lexems = obj.getLexems();
	idn = obj.getIdn();
}


void Compiler::sAnalyzer(json lexems) {
	SAnalyzer obj(lexems, grammarFile, this->isInfoActive());
	obj.startSAnalyzing();
}


void Compiler::rpn(json &lexems, json &idn) {
	RPN obj(lexems, idn, this->isInfoActive());
	obj.ReversePolishNotation();
}


void Compiler::fileFind(int keyid) {
	bool pathFlag = false;

	for (size_t i = 0; i < __argc; ++i) {
		if (__argv[i] == keys[keyid].first || __argv[i] == keys[keyid].second) {
			if (pathFlag == true) {
				switch (keyid) {
				case 0:
					throw codeFileKeyIsRepeating(i);
					break;
				case 1:
					throw grammarFileKeyIsRepeating(i);
					break;
				case 2:
					throw lexemFileKeyIsRepeating(i);
					break;
				}
			}
				
			pathFlag = true;

			if (i + 1 != __argc && std::filesystem::exists(__argv[i + 1])) {
				switch (keyid) {
				case 0:
					codeFile = __argv[i + 1];
					break;
				case 1:
					grammarFile = __argv[i + 1];
					break;
				case 2:
					lexemFile = __argv[i + 1];
					break;
				}
			}
			else {
				switch (keyid) {
				case 0:
					throw codeFileDoesNotExists();
					break;
				case 1:
					throw grammarFileDoesNotExists();
					break;
				case 2:
					throw lexemFileDoesNotExists();
					break;
				}
			}
		}
	}

	if (pathFlag == false) {
		switch (keyid) {
		case 0:
			if (std::filesystem::exists("./ProgramCode/DemonCode.txt"))
				codeFile = "./ProgramCode/DemonCode.txt";
			else 
				throw codeFileKeyIsMissing();
			break;
		case 1:
			if (std::filesystem::exists("./Resources/Grammar.txt"))
				grammarFile = "./Resources/Grammar.txt";
			else
				throw grammarFileKeyIsMissing();
			break;
		case 2:
			if (std::filesystem::exists("./Resources/Lexem.txt"))
				lexemFile = "./Resources/Lexem.txt";
			else
				throw lexemFileKeyIsMissing();
			break;
		}
	}
}


bool Compiler::isInfoActive() {
	for (size_t i = 0; i < __argc; ++i) {
		if (__argv[i] == keys[3].first || __argv[i] == keys[3].second) {
			return true;
		}
	}
	return false;
}

bool Compiler::isHelpActive() {
	for (size_t i = 0; i < __argc; ++i) {
		if (__argv[i] == keys[4].first || __argv[i] == keys[4].second) {
			return true;
		}
	}
	return false;
}

void Compiler::helpOutput() {
	cout << "*******************HELP*******************\n"
		"Set the path to files using next keys:\n"
		"\t--codePath/-cp     -  set the path to the file with program code.\n"
		"\t--grammarPath/-gp  -  set the path to the file with language grammar.\n"
		"\t--lexemPath/-lp    -  set the path to the file with language lexems.\n"
		"\t--info/-i          -  gives you all the information about the process of parsing and executing the program code.\n"
		"\t--help/-h          -  provides the help in using this console app.\n"
		"You can set any file names for necessary files in paths.\n"
		"Also you can use the keys in any order.\n"
		"It is not necessary to specify each file directory.The application defines default paths for each file.\n\n"
		"Example of using:\n"
		"\tSimpleCppInterpreter.exe -cp \"D:\\DemonCode.txt\" -gp \"D:\\Grammar.txt\" -lp \"D:\\Lexem.txt\" -info\n"
		"********************************************\n\n";
}