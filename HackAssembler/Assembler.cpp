#pragma
#include "LibraryCollection.h"
#include "ClassUtility.h"
#include "ClassFile.h"
#include "ClassST.h"
#include "ClassParser.h"
#include "ClassCode.h"
#include "ClassAssembler.h"

using namespace std;

int main() {
	Utility::showLogo();
	try {
		while (true) {
			string path = File::askFilePath();

			//check for suffix '.asm'
			bool isFormat = File::endsWith(path, ".asm");
			while (!isFormat) {
				File::isValidFormat(path, isFormat);
				Utility::clear();
			}

			Assembler assembler(path, "Prog.hack");

			try {
				cout << "File successfully opened" << endl;
				assembler.firstScan();   //first scan.
				assembler.secondScan();  //second scan.
				cout << "Finished converting!\n" << endl;
			}
			catch (const runtime_error& e) {
				cerr << "Error during assembly: " << e.what() << endl;
			}
		}
	}
	catch (const exception& e) {
		cerr << "Unhandled exception: " << e.what() << '\n';
		return 1;
	}

	return 0;
}