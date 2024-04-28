#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <stdexcept>
#include <bitset>
#include <unordered_map>

using namespace std;

void showLogo() {
	cout << "        ___                              _      _             \n";
	cout << "       / _ \\                            | |    | |            \n";
	cout << "      / /_\\ \\ ___  ___   ___  _ __ ___  | |__  | |  ___  _ __ \n";
	cout << "      |  _  |/ __|/ __| / _ \\| '_ ` _ \\ | '_ \\ | | / _ \\| '__|\n";
	cout << "      | | | |\\__ \\\\__ \\|  __/| | | | | || |_) || ||  __/| |   \n";
	cout << "      \\_| |_/|___/|___/ \\___||_| |_| |_||_.__/ |_| \\___||_|   \n";
	cout << endl;
	cout << endl;
}

void clear() {
	cout << "\x1B[2J\x1B[H";
}

string askFilePath() {
	cout << "Please insert file path" << endl;
	cout << "make sure the file ends with .asm" << endl;
	cout << "File path -> ";
	string path;
	cin >> path;
	return path;
}

string askIfError(string path) {
	cout << "\nInvalid path!\n";
	cout << "Example -> path.asm\n";
	cout << "Try again.\n";
	cout << "File path -> ";
	cin >> path;
	return path;
}

bool endsWith(const string& path, string format) {
	if (path.size() - 1 < format.size()) return false;

	return path.compare(path.size() - format.size(), format.size(), format) == 0;
}

bool isNotCommentLine(string line) {
	if (line.empty() || line.find("//") != string::npos) return false;
	return true;
}

string cleanAndValidateLine(string& line) {
	string validLine;

	size_t first = line.find_first_not_of(' ');
	if (first == string::npos) throw runtime_error("Empty or invalid line");

	size_t last = line.find_last_not_of(' ');
	validLine = line.substr(first, (last - first + 1));

	if (validLine.find(' ') != string::npos) {
		throw runtime_error("Invalid: more than one word or space inside the expression");
	}
	return validLine;
}

void addToSymbolTable(string line, unordered_map<string, int>& symbolTable, int& lineCount, int& freeRam) {
	string symbol;
	size_t last;

	if(line[0] == '(') {
		last = line.find_last_of(')');
		if (last != string::npos && last > 1) {
			symbol = line.substr(1, last - 1);
			if (symbol.empty()) {
				throw runtime_error("Empty label in assembly code is not allowed.");
			}
			if(symbolTable.find(symbol) != symbolTable.end()) throw runtime_error("Indeed included to > symbolTable <");
			symbolTable[symbol] = lineCount;
		}
		else throw runtime_error("Invalid or empty label in assembly code.");
	}
	else {
		lineCount++;
	}
}

void isDest(bool& hasDest) {
	hasDest = true;
}

string findDest(string line, bool& hasDest) {
	unordered_map<string, string> destTable = {
			{"M", "001"}, {"D", "010"}, {"DM", "011"}, {"A", "100"},
			{"AM", "101"}, {"AD", "110"}, {"ADM", "111"}, {"MD", "011"},
			{"MA", "011"}, {"DA", "011"}
	};

	string dest;
	size_t endDest = line.find_first_of('=');
	if (endDest != string::npos) {
		isDest(hasDest);
		string destLine = line.substr(0, endDest);
		for (char c : destLine) if (islower(c)) throw runtime_error("Invalid instruction lower case not valid in > dest <.");
		if (destLine.empty()) throw runtime_error("Invalid instruction in assembly code: Label before '=' cannot be empty.");
		return destTable[destLine];
	}
	else return "000";
}

string findComp(string& line, bool& hasDest) {
	unordered_map<string, string> compTable_a = {
			{"M", "110000"}, {"!M", "110001"}, {"-M", "110011"}, {"M+1", "110111"},
			{"M-1", "110010"}, {"D+M", "000010"}, {"D-M", "010011"}, {"M-D", "000111"},
			{"D&M", "000000"}, {"D|M", "010101"},
	};

	unordered_map<string, string> compTable_none = {
		{"0", "101010"}, {"1", "111111"}, {"-1", "111010"}, {"D", "001100"}, {"A", "110000"},
		{"!D", "001101"}, {"!A", "110001"}, {"-D", "001111"}, {"-A", "110011"}, {"D+1", "011111"},
		{"A+1", "110111"}, {"D-1", "001110"}, {"D-1", "001110"}, {"A-1", "110010"}, {"D+A", "000010"},
		{"D-A", "010011"}, {"A-D", "000111"}, {"D&A", "000000"}, {"D|A", "010101"}
	};

	if (hasDest) {
		string compLine = line.substr(line.find_first_of('=') + 1);
		for (char c : compLine) if (islower(c))  throw runtime_error("Invalid instruction lower case not valid");
		if (compTable_a.find(compLine) != compTable_a.end()) { // a == 1
			return "1" + compTable_a[compLine];
		}
		else { // a == 0
			return "0" + compTable_none[compLine];
		}
	}
	else {
		size_t endComp = line.find_first_of(';');
		if (endComp != string::npos) {
			string compLine = line.substr(0, endComp);
			if (compLine.empty()) throw runtime_error("Invalid syntax for comp, required statement example -> D-M.");
			for (char c : compLine) if (islower(c))  throw runtime_error("Invalid instruction lower case not valid in > comp <.");
			if (compTable_a.find(compLine) != compTable_a.end()) { // a == 1
				return "1" + compTable_a[compLine];
			}
			else { // a == 0
				return "0" + compTable_none[compLine];
			}
		}
		else throw runtime_error("Invalid assembly syntax for jump required > ; < symbol befor jump statement");
	}
}

string findJump(string& line, bool& hasDest) {
	unordered_map<string, string> jumpTable{
		{"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"}, {"JLT", "100"},
		{"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"}
	};

	if (!hasDest) {
		size_t startJump = line.find_first_of(';');
		if (startJump != string::npos) {
			string jumpLine = line.substr(line.find_first_of(';') + 1);
			for (char c : jumpLine) if (islower(c)) throw runtime_error("Invalid instruction lower case not valid in > jump <.");
			if (jumpLine.empty()) throw runtime_error("Invalid instruction, blank jump statement after > ; < symbol.");
			return jumpTable[jumpLine];
		}
		else throw runtime_error("Invalid instruction for jump statement must be > ; < symbol.");
	}
	return "000";
}

string A_instruction(string line, unordered_map<string, int>& symbolTable, int& countLine, int& freeRam) {
	string binary;
	int value;
	if (line.length() > 1) {
		if (isdigit(line[1])) {
			value = stoi(line.substr(1));
			binary = bitset<15>(value).to_string();
			countLine++;
			return "0" + binary;
		} else {
			string symbol = line.substr(1);
			if (symbolTable.find(symbol) != symbolTable.end()) {
				value = symbolTable[symbol];
				binary = bitset<15>(value).to_string();
				return "0" + binary;
			} else {
				binary = bitset<15>(freeRam).to_string();
				symbolTable[symbol] = freeRam++;
				countLine++;
				return "0" + binary;
			}
		}
	} else throw runtime_error("Invalid or empty label in assembly code.");
}

string L_instruction(string line, unordered_map<string, int>& symbolTable) {
	string binary;
	string symbol = line.substr(1, line.length() - 1);
	if (symbolTable.find(symbol) == symbolTable.end()) throw runtime_error("Invalid or empty label in assembly code.");
	int value = symbolTable[symbol];
	return binary = bitset<16>(value).to_string();
}

string C_instruction(string line, int& lineCount) {
	bool hasDest = false;

	string dest = findDest(line, hasDest);
	string comp = findComp(line, hasDest);
	string jump = findJump(line, hasDest);

	string binary = "111" + comp + dest + jump;
	if (binary.length() != 16) throw runtime_error("Invalid binary length must be 16 bit instruction.");
	lineCount++;
	return binary;
}

void scanAssemblyFile(ifstream& asmFile, ofstream& myFile, int& freeRam, int& lineCount, unordered_map<string, int>& symbolTable, bool& isScanned) {
	string line;
	while (getline(asmFile, line)) {
		if (isNotCommentLine(line)) {
			try {
				string validLine = cleanAndValidateLine(line);
				if (isScanned) {
					string binInstruction;

					switch (validLine[0]) {
					case '@':
						binInstruction = A_instruction(validLine, symbolTable, lineCount, freeRam); //0-> 15 bit+=
						myFile << binInstruction << endl;
						break;
					case '(':
						break;
					case '/':
						break;
					default:
						binInstruction = C_instruction(validLine, lineCount);
						myFile << binInstruction << endl;
						break;
					}
				}
				else {
					addToSymbolTable(validLine, symbolTable, lineCount, freeRam);
				}
			}
			catch (const runtime_error& e) {
				cerr << "Error: " << e.what() << endl;
			}
		}
	}
}

int main() {
	showLogo();
	try {
		while (true) {
			string path = askFilePath();

			bool isFormat = endsWith(path, ".asm");
			while (!isFormat) {
				path = askIfError(path);
				isFormat = endsWith(path, ".asm");
				clear();
			}
			
			ofstream myFile("1.hack");
			string fullPath = "C:\\Users\\Lenovo\\Desktop\\nand2tetris\\projects\\6\\rect\\" + path;
			ifstream asmFile(fullPath);

			bool isScanned = false;
			int lineCount = 0;
			int freeRam = 16;

			unordered_map<string, int> symbolTable = {
				{"R0", 0}, {"R1", 1}, {"R2", 2}, {"R3", 3}, {"R4", 4}, {"R5", 5},
				{"R6", 6}, {"R7", 7}, {"R8", 8}, {"R9", 9}, {"R10", 10},
				{"R11", 11}, {"R12", 12}, {"R13", 13}, {"R14", 14}, {"R15", 15},
				{"SP", 0}, {"LCL", 1}, {"ARG", 2}, {"THIS", 3}, {"THAT", 4},
				{"SCREEN", 16384}, {"KBD", 24576}
			};

			try {
				if (asmFile.is_open()) {
					cout << "File successfully opened" << endl;
					//first scan.
					scanAssemblyFile(asmFile, myFile, freeRam, lineCount, symbolTable, isScanned);
					isScanned = true;
					lineCount = 0;
					asmFile.clear();
					asmFile.seekg(0, ios::beg);
					//second scan + write to myFile.
					scanAssemblyFile(asmFile, myFile, freeRam, lineCount, symbolTable, isScanned);
					cout << "Finished converting!\n" << endl;
				}
				else {
					cerr << "Error opening file '" << path << "'. Check if the file exists and you have read permissions." << endl;
				}
			}
			catch (const runtime_error& e) {
				cerr << "Error: " << e.what() << endl;
			}
			myFile.close();
			asmFile.close();
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Unhandled Exception: " << e.what() << '\n';
		return 1;
	}
	catch (...) {
			std::cerr << "Unknown exception caught\n";
			return 1;
	}
	return 0;
}
//https://stackoverflow.com/questions/7372918/whats-the-use-of-r-escape-sequence