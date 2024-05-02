#pragma once
#include <iostream>

using namespace std;

class Utility {
public:
	static void showLogo() {
		cout << "        ___                              _      _             \n";
		cout << "       / _ \\                            | |    | |            \n";
		cout << "      / /_\\ \\ ___  ___   ___  _ __ ___  | |__  | |  ___  _ __ \n";
		cout << "      |  _  |/ __|/ __| / _ \\| '_ ` _ \\ | '_ \\ | | / _ \\| '__|\n";
		cout << "      | | | |\\__ \\\\__ \\|  __/| | | | | || |_) || ||  __/| |   \n";
		cout << "      \\_| |_/|___/|___/ \\___||_| |_| |_||_.__/ |_| \\___||_|   \n";
		cout << endl;
		cout << endl;
	}

	static void clear() {
		cout << "\x1B[2J\x1B[H";
	}
};

