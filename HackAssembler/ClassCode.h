#pragma once
#include "LibraryCollection.h"

using namespace std;

class Code {
public:

	static void validateUpperCase(const string& line) {
		if (any_of(line.begin(), line.end(), islower)) {
			throw runtime_error("Invalid instruction: Lower case not valid.");
		}
	}

	static string findDest(const string& line, bool& hasDest) {
		static const unordered_map<string, string> destTable = {
				{"M", "001"}, {"D", "010"}, {"DM", "011"}, {"A", "100"},
				{"AM", "101"}, {"AD", "110"}, {"ADM", "111"}, {"MD", "011"},
				{"MA", "011"}, {"DA", "011"}
		};

		size_t endDest = line.find_first_of('=');
		if (endDest != string::npos) {
			hasDest = true;
			string destLine = line.substr(0, endDest);
			validateUpperCase(destLine);
			if (destLine.empty()) throw runtime_error("Invalid instruction in assembly code: Label before '=' cannot be empty.");
			auto it = destTable.find(destLine);
			if (it != destTable.end()) {
				return it->second;
			}
			throw runtime_error("Invalid destination in assembly instruction.");
		}
		else return "000";
	}

	static string findComp(const string& line, bool& hasDest) {
		static const unordered_map<string, string> compTable_a = {
				{"M", "110000"}, {"!M", "110001"}, {"-M", "110011"}, {"M+1", "110111"},
				{"M-1", "110010"}, {"D+M", "000010"}, {"D-M", "010011"}, {"M-D", "000111"},
				{"D&M", "000000"}, {"D|M", "010101"},
		};

		static const unordered_map<string, string> compTable_none = {
			{"0", "101010"}, {"1", "111111"}, {"-1", "111010"}, {"D", "001100"}, {"A", "110000"},
			{"!D", "001101"}, {"!A", "110001"}, {"-D", "001111"}, {"-A", "110011"}, {"D+1", "011111"},
			{"A+1", "110111"}, {"D-1", "001110"}, {"D-1", "001110"}, {"A-1", "110010"}, {"D+A", "000010"},
			{"D-A", "010011"}, {"A-D", "000111"}, {"D&A", "000000"}, {"D|A", "010101"}
		};

		if (hasDest) {
			string compLine = line.substr(line.find_first_of('=') + 1);
			for (char c : compLine) if (islower(c)) throw runtime_error("Invalid instruction lower case not valid");

			auto it = compTable_a.find(compLine);
			if (it != compTable_a.end()) { // a == 1
				return "1" + it->second;
			}
			else {
				auto it_none = compTable_none.find(compLine);
				if (it_none != compTable_none.end()) { // a == 0
					return "0" + it_none->second;
				}
			}
			throw runtime_error("Invalid comp code.");
		}
		else {
			size_t endComp = line.find_first_of(';');
			if (endComp != string::npos) {
				string compLine = line.substr(0, endComp);
				if (compLine.empty()) throw runtime_error("Invalid syntax for comp, required statement example -> D-M.");
				for (char c : compLine) if (islower(c)) throw runtime_error("Invalid instruction lower case not valid in > comp <.");

				auto it = compTable_a.find(compLine);
				if (it != compTable_a.end()) { // a == 1
					return "1" + it->second;
				}
				else {
					auto it_none = compTable_none.find(compLine);
					if (it_none != compTable_none.end()) { // a == 0
						return "0" + it_none->second;
					}
				}
				throw runtime_error("Invalid assembly syntax for comp.");
			}
			else throw runtime_error("Invalid assembly syntax for jump required > ; < symbol before jump statement");
		}

	}

	static string findJump(const string& line, bool& hasDest) {
		static const unordered_map<string, string> jumpTable{
			{"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"}, {"JLT", "100"},
			{"JNE", "101"}, {"JLE", "110"}, {"JMP", "111"}
		};

		if (!hasDest) {
			size_t startJump = line.find_first_of(';');
			if (startJump != string::npos) {
				string jumpLine = line.substr(startJump + 1);
				if (jumpLine.empty()) throw runtime_error("Invalid instruction, blank jump statement after > ; < symbol.");

				if (any_of(jumpLine.begin(), jumpLine.end(), islower)) {
					throw runtime_error("Invalid instruction: Lower case not valid in > jump <.");
				}

				auto it = jumpTable.find(jumpLine);
				if (it != jumpTable.end()) {
					return it->second;
				}
				else {
					throw runtime_error("Invalid jump code in instruction.");
				}
			}
			else {
				throw runtime_error("Invalid instruction for jump statement must include > ; < symbol.");
			}
		}
		return "000";
	}


	static string A_instruction(const string& line, unordered_map<string, int>& symbolTable, int& countLine, int& freeRam) {
		string binary;
		int value;
		if (line.length() > 1) {
			if (isdigit(line[1])) {
				value = stoi(line.substr(1));
				binary = bitset<15>(value).to_string();
				countLine++;
				return "0" + binary;
			}
			else {
				string symbol = line.substr(1);
				if (symbolTable.find(symbol) != symbolTable.end()) {
					value = symbolTable[symbol];
					binary = bitset<15>(value).to_string();
					return "0" + binary;
				}
				else {
					binary = bitset<15>(freeRam).to_string();
					symbolTable[symbol] = freeRam++;
					countLine++;
					return "0" + binary;
				}
			}
		}
		else throw runtime_error("Invalid or empty label in assembly code.");
	}

	static string L_instruction(const string& line, unordered_map<string, int>& symbolTable) {
		string binary;
		string symbol = line.substr(1, line.length() - 1);
		if (symbolTable.find(symbol) == symbolTable.end()) throw runtime_error("Invalid or empty label in assembly code.");
		int value = symbolTable[symbol];
		return binary = bitset<16>(value).to_string();
	}

	static string C_instruction(const string& line, int& lineCount) {
		bool hasDest = false;

		string dest = findDest(line, hasDest);
		string comp = findComp(line, hasDest);
		string jump = findJump(line, hasDest);

		string binary = "111" + comp + dest + jump;
		if (binary.length() != 16) throw runtime_error("Invalid binary length must be 16 bit instruction.");
		lineCount++;
		return binary;
	}
};
