#pragma once
#include "LibraryCollection.h"

using namespace std;

class Parser {
public:
	static string cleanAndValidateLine(const string& line) {
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

	static bool isNotCommentLine(const string line) {
		if (line.empty() || line.find("//") != string::npos) return false;
		return true;
	}
};
