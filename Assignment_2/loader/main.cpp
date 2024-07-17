#include "loader.h"

int main()
{
    string input;
    cout << "Enter input file name: ";
    cin >> input;
    // Create a ProgramLoader object with the input file name
    Loader loader(input);

    // Pass One: Read the input file, process header records, and build ESTAB
    if (loader.performPassOne(loader.fileInput) < 0)
    { // Check for errors in pass one
        cout << "Error in pass one." << endl;
        loader.fileInput.close();
        loader.fileOutput.close();
        return 0;
    }
    loader.fileInput.close(); // Close input file after pass one

    // Reopen input file for pass two
    loader.fileInput.open(loader.inputFile);
    // Pass Two: Process text and modification records, load sections into memory
    if (loader.performPassTwo(loader.fileInput, loader.fileOutput) < 0)
    { // Check for errors in pass two
        cout << "Error in pass two." << endl;
        loader.fileInput.close();
        loader.fileOutput.close();
        return 0;
    }
    return 0;
}
