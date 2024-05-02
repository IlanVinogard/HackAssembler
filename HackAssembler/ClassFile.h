#pragma once
#include "LibraryCollection.h"

using namespace std;

class File {
public:
	static string askFilePath() {
		cout << "Please insert file path\n";
		cout << "make sure the file ends with .asm\n";
		cout << "File path -> ";
		string path;
		cin >> path;
		return path;
	}

	static string askIfError(string& path) {
		cout << "\nInvalid path!\n";
		cout << "Example -> path.asm\n";
		cout << "Try again.\n";
		cout << "File path -> ";
		cin >> path;
		return path;
	}

	static bool endsWith(const string& path, string format) {
		if (path.size() - 1 < format.size()) return false;

		return path.compare(path.size() - format.size(), format.size(), format) == 0;
	}

	static void isValidFormat(string& path, bool& isFormat) {
		path = askIfError(path);
		isFormat = endsWith(path, ".asm");
	}
};