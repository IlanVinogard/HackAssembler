#pragma once
#include "LibraryCollection.h"
#include "ClassST.h"
#include "ClassParser.h"
#include "ClassCode.h"

using namespace std;

class Assembler {
private:
	SymbolTable symbolTable;
	ifstream input;
	ofstream output;
	int lineCount = 0;
	int freeRam = 16;

public:
	Assembler(const string& inputPath, const string& outputPath) {
		input.open(inputPath, ios::in);
		if (!input.is_open()) {
			cerr << "Error opening input file: " << inputPath << endl;
			throw runtime_error("Failed to open input file.");
		}

		output.open(outputPath, ios::out);
		if (!output.is_open()) {
			cerr << "Error opening output file: " << outputPath << endl;
			throw runtime_error("Failed to open output file.");
		}
	}

	~Assembler() {
		input.close();
		output.close();
	}

	void firstScan() {
		string line;
		while (getline(input, line)) {
			if (Parser::isNotCommentLine(line)) {
				try {
					string validLine = Parser::cleanAndValidateLine(line);
					symbolTable.processSymbol(validLine, lineCount, freeRam);
				}
				catch (const runtime_error& e) {
					cerr << "Error: " << e.what() << endl;
				}
			}
		}
		lineCount = 0;
		input.clear();
		input.seekg(0, ios::beg);
	}

	void secondScan() {
		string line;
		while (getline(input, line)) {
			if (Parser::isNotCommentLine(line)) {
				try {
					string validLine = Parser::cleanAndValidateLine(line);
					string binInstruction;
					if (validLine[0] == '@') {
						binInstruction = Code::A_instruction(validLine, symbolTable.getTable(), lineCount, freeRam); //0-> 15 bit+=
						output << binInstruction << endl;
					}
					else if (validLine[0] == '(' || validLine[0] == '/') continue;

					else {
						binInstruction = Code::C_instruction(validLine, lineCount);
						output << binInstruction << endl;
					}
				}
				catch (const runtime_error& e) {
					cerr << "Error: " << e.what() << endl;
				}
			}
		}
	}
};