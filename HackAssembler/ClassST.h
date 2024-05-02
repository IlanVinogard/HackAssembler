#pragma once
#include "LibraryCollection.h"

using namespace std;

class SymbolTable {
private:
	unordered_map<string, int> table;

public:
	SymbolTable() {
		table = {
			{"R0", 0}, {"R1", 1}, {"R2", 2}, {"R3", 3}, {"R4", 4}, {"R5", 5},
			{"R6", 6}, {"R7", 7}, {"R8", 8}, {"R9", 9}, {"R10", 10},
			{"R11", 11}, {"R12", 12}, {"R13", 13}, {"R14", 14}, {"R15", 15},
			{"SP", 0}, {"LCL", 1}, {"ARG", 2}, {"THIS", 3}, {"THAT", 4},
			{"SCREEN", 16384}, {"KBD", 24576}
		};
	}
	bool contains(const string& symbol) const {
		return table.find(symbol) != table.end();
	}

	void addEntry(const string& symbol, int& lineCount) {
		if (contains(symbol)) {
			throw runtime_error("Symbol already exists in the table.");
		}
		table[symbol] = lineCount;
	}

	int getValue(const string& symbol) const {
		auto it = table.find(symbol);
		if (it != table.end()) {
			return it->second;
		}
		throw runtime_error("Symbol not found in the table.");
	}

	unordered_map<string, int>& getTable() {
		return table;
	}

	void processSymbol(string line, int& lineCount, int& freeRam) {

		if (line[0] == '(') {
			size_t last = line.find_last_of(')');
			if (last != string::npos && last > 1) {
				string symbol = line.substr(1, last - 1);
				if (symbol.empty()) throw runtime_error("Empty label in assembly code is not allowed.");

				if (contains(symbol)) throw runtime_error("Currently included to > symboltable <");

				addEntry(symbol, lineCount);
			}
			else throw runtime_error("Invalid or empty label in assembly code.");
		}
		else lineCount++;
	}
};
