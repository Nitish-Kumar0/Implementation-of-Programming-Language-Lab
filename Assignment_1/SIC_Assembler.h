#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <map>
#include <string>
using namespace std;

class Assembler 
{
public:
    class OpCodeTable 
    {
        map<string, string> opTable; // Map to store opcode and corresponding machine code.

    public:
        OpCodeTable() // Constructor to initialize opcode table.
        {
            initialize(); // Initialize opcode table.
        }

        void initialize() // Method to initialize opcode table with predefined values.
        {
            // Initialize opcode table
            opTable["LDA"] = "00";
            opTable["LDX"] = "04";
            opTable["LDL"] = "08";
            opTable["STA"] = "0c";
            opTable["STX"] = "10";
            opTable["STL"] = "14";
            opTable["LDCH"] = "50";
            opTable["STCH"] = "54";
            opTable["ADD"] = "18";
            opTable["SUB"] = "1c";
            opTable["MUL"] = "20";
            opTable["DIV"] = "24";
            opTable["COMP"] = "28";
            opTable["J"] = "3c";
            opTable["JLT"] = "38";
            opTable["JEQ"] = "30";
            opTable["JGT"] = "34";
            opTable["JSUB"] = "48";
            opTable["RSUB"] = "4c";
            opTable["TIX"] = "2c";
            opTable["TD"] = "e0";
            opTable["RD"] = "d8";
            opTable["WD"] = "dC";
        }

        bool search(const string &opcode) // Method to search for an opcode in the opcode table.
        {
            return (opTable.find(opcode) != opTable.end()); // Return true if opcode found, false otherwise.
        }

        string retrieve(const string &opcode) // Method to retrieve machine code for a given opcode.
        {
            return opTable[opcode]; // Return machine code corresponding to the opcode.
        }
    };

    class SymbolTable 
    {
        map<string, int> symTable; // Map to store symbol and corresponding address.

    public:
        bool search(const string &symbol) // Method to search for a symbol in the symbol table.
        {
            return (symTable.find(symbol) != symTable.end()); // Return true if symbol found, false otherwise.
        }

        void insert(const string &symbol, int address) // Method to insert a symbol and its address into the symbol table.
        {
            symTable[symbol] = address; // Insert symbol and address into the symbol table.
        }

        int retrieve(const string &symbol) // Method to retrieve address for a given symbol.
        {
            return symTable[symbol]; // Return address corresponding to the symbol.
        }
    };

    private:
    string programName, inputFile, buffer; // Strings to store program name, input file name, and buffer for file content.
    OpCodeTable opTable; // Object of OpCodeTable class to manage opcode table.
    SymbolTable symTable; // Object of SymbolTable class to manage symbol table.
    ifstream inputFileStream; // File stream for input file.
    int locationCounter, startingAddress, programLength, errorRedefined, errorUndefined, errorInvalidOpcode; // Variables to store assembler state and error counts.


    public:
    // Constructor
    Assembler(const string &file) // Constructor to initialize the assembler with an input file.
    {
        errorRedefined = 0; // Initialize redefined error count.
        errorUndefined = 0; // Initialize undefined error count.
        errorInvalidOpcode = 0; // Initialize invalid opcode error count.
        inputFile = file; // Store input file name.

        inputFileStream.open(inputFile); // Open input file.
        if (!inputFileStream.is_open()) // Check if input file opening failed.
        {
            cerr << "Error: input file opening failed.\n"; // Display error message.
            exit(1); // Exit program.
        }

        std::getline(inputFileStream, buffer, '\0'); // Read contents of input file into buffer.
        cout << "<---INPUT FILE--->\n\n" << buffer << "\n"; // Display input file content.
        inputFileStream.seekg(0, ios::beg); // Set file pointer to the beginning of the file.
    }

    // Function to check if the line is a comment
    bool isComment(string line)
    {
        return line[0]=='.';
    }

    // Function to convert hexadecimal string to decimal
    int hexToDecimal(const string& hexString) 
    {
        int result = 0;
        for (char c : hexString) 
        {
            result *= 16;
            if (isdigit(c)) 
            {
                result += c - '0';
            } 
            else if (isalpha(c)) 
            {
                result += toupper(c) - 'A' + 10;
            } 
            else 
            {
                // Invalid character in hex string
                cerr << "Error: Invalid hexadecimal string\n";
                exit(EXIT_FAILURE);
            }
        }
        return result;
    }

    // Function to parse a line during pass 1
    tuple<string, string, string> parseLinePass1(const string& line) 
    {
        string label, opcode, operand;
        size_t pos = 0;
        size_t prev_pos = 0;
        
        pos = line.find('\t', prev_pos);// Find the first tab        
        label = line.substr(prev_pos, pos - prev_pos);// Extract label        
        prev_pos = pos + 1;// Update prev_pos to one character after the tab
        pos = line.find('\t', prev_pos);// Find the second tab
        opcode = line.substr(prev_pos, pos - prev_pos);// Extract opcode
        prev_pos = pos + 1;// Update prev_pos to one character after the second tab
        pos = line.find('\t', prev_pos);// Find the third tab
        operand = line.substr(prev_pos, pos - prev_pos);// Extract operand

        return make_tuple(label, opcode, operand);
    }

    // Function to parse a line during pass 2
    tuple<string, string, string, string> parseLinePass2(const string& line) {
        string address, label, opcode, operand;
        size_t pos = 0;
        size_t prev_pos = 0;

        pos = line.find('\t', prev_pos);// Find the first tab    
        address = line.substr(prev_pos, pos - prev_pos);// Extract address
        prev_pos = pos + 1;// Update prev_pos to one character after the tab
        pos = line.find('\t', prev_pos);// Find the second tab       
        label = line.substr(prev_pos, pos - prev_pos); // Extract label       
        prev_pos = pos + 1; // Update prev_pos to one character after the second tab       
        pos = line.find('\t', prev_pos); // Find the third tab       
        opcode = line.substr(prev_pos, pos - prev_pos); // Extract opcode       
        prev_pos = pos + 1; // Update prev_pos to one character after the third tab
        operand = line.substr(prev_pos); // Extract operand

        return make_tuple(address, label, opcode, operand);
    }

    bool pass1() // Function definition for Pass 1 of the assembler.
    {
        bool encounteredStart = false; // Flag to check if 'START' directive encountered.
        ofstream intermediateFile("intermediate.txt"); // File stream for writing intermediate results.
        string line; // String to store each line of input.
        string label, opcode, operand; // Strings to store label, opcode, and operand extracted from each line.
        bool success = true; // Flag to indicate success or failure of Pass 1.

        // Loop through each line of input.
        while (std::getline(inputFileStream, line)) 
        {
            if(isComment(line) || line == "") // Check if line is a comment or empty.
            {
                continue; // Skip processing if comment or empty line.
            }
            // Reset label, opcode, operand for each line.
            label.clear();
            opcode.clear();
            operand.clear();

            // Parse the line to extract label, opcode, and operand.
            tie(label, opcode, operand) = parseLinePass1(line);

            if(opcode == "END") // Check if opcode is 'END'.
            {
                // Write to intermediate file.
                intermediateFile << "\t";
                intermediateFile << label << "\t";
                intermediateFile << opcode << "\t";
                intermediateFile << operand << "\n";
                break; // Exit loop if 'END' directive encountered.
            }

            if (opcode == "START") // Check if opcode is 'START'.
            {
                // Set program name and starting address.
                programName = label;
                startingAddress = hexToDecimal(operand);
                locationCounter = startingAddress; // Initialize location counter here.
                // Write to intermediate file.
                intermediateFile << setw(4) << setfill('0') << hex << locationCounter << "\t";
                intermediateFile << label << "\t";
                intermediateFile << opcode << "\t";
                intermediateFile << operand << "\n";
                encounteredStart = true; // Set flag to indicate 'START' encountered.
                continue; // Move to next line.
            } 
            else if (!encounteredStart) // Check if 'START' directive not encountered yet.
            { 
                // Error: No START directive found.
                cerr << "Error: No START directive found.\n";
                success = false; // Set flag to indicate failure.
                exit(1); // Exit program.
            }

            int loc = locationCounter; // Store current location counter value.
            if (!label.empty()) // Check if label is not empty.
            {
                if (!symTable.search(label)) // Check if label is not already in symbol table.
                {
                    symTable.insert(label, locationCounter); // Insert label and its corresponding address into symbol table.
                }
                else 
                {
                    cout << "ERROR Label: \"" << label << "\" REDEFINED\n"; // Error: Label redefined.
                    success = false; // Set flag to indicate failure.
                    errorRedefined++; // Increment redefined error count.
                }
            }
            // Check opcode and update location counter accordingly.
            if (opTable.search(opcode)) // If opcode is found in operation table.
            {
                locationCounter += 3; // Increment location counter by 3.
            }
            else if (opcode == "WORD") // If opcode is 'WORD'.
            {
                locationCounter += 3; // Increment location counter by 3.
            }
            else if (opcode == "RESW") // If opcode is 'RESW'.
            {
                locationCounter += (3 * stoi(operand)); // Increment location counter by 3 times the operand value.
            }
            else if (opcode == "RESB") // If opcode is 'RESB'.
            {
                locationCounter += stoi(operand); // Increment location counter by the operand value.
            }
            else if (opcode == "BYTE") // If opcode is 'BYTE'.
            {
                if (operand[0] == 'X') // If operand is in hexadecimal format.
                {
                    locationCounter += (operand.length() - 3) / 2; // Increment location counter accordingly.
                }
                else // If operand is in character format.
                {
                    locationCounter += (operand.length() - 3); // Increment location counter accordingly.
                }
            } 
            else // If opcode is invalid.
            {
                cout << "ERROR: Invalid Opcode \"" << opcode << "\"\n"; // Error: Invalid opcode.
                success = false; // Set flag to indicate failure.
                errorInvalidOpcode++; // Increment invalid opcode error count.
            }
            // Write intermediate results to file.
            intermediateFile << setw(4) << setfill('0') << hex << loc;
            intermediateFile << "\t";
            intermediateFile << label << "\t";
            intermediateFile << opcode << "\t";
            intermediateFile << operand << "\n";
        }
        
        // Close input and intermediate files.
        inputFileStream.close();
        intermediateFile.close();
        
        // Read contents of intermediate file into buffer.
        ifstream intermediateFileStream("intermediate.txt");
        std::getline(intermediateFileStream, buffer, '\0');
        cout << "\n<---INTERMEDIATE FILE--->\n\n" << buffer << "\n";
        intermediateFileStream.close();
        
        // Calculate program length and display final location counter.
        programLength = locationCounter - startingAddress;
        cout<<"Final location: "<< hex << locationCounter;
        cout<<"\nProgram Length: "<< dec <<programLength<<"\n";
        return success; // Return success status.
    }


    // Pass 2 of the assembler
    bool pass2()
    {
        bool success = true; // Flag to indicate success or failure of Pass 2.
        cout << "\n<---OBJECT CODE--->\n\n"; // Output message indicating the start of object code section.
        inputFileStream.open("intermediate.txt"); // Open intermediate file for reading.
        ofstream outputFileStream; // File stream for writing output.
        outputFileStream.open("output.txt"); // Open output file for writing.
        int textRecordLength=0,increment=0; // Variables to track length of text record and increment value.
        string objectCode,textRecord; // String to hold object code and text record.
        bool res = false; // Flag to indicate if RESW or RESB encountered.
        string address,label,opcode,operand,firstAddress,line; // Strings to hold line components.
        stringstream T(textRecord),O(objectCode); // String streams for text record and object code.

        // Loop through each line of intermediate file.
        while (std::getline(inputFileStream, line)) 
        {
            tie(address, label, opcode, operand) = parseLinePass2(line); // Parse line into components.

            if (opcode == "START") // Check if opcode is 'START'.
            {
                // Write header record to output file.
                outputFileStream<<"H^"<<programName;
                cout<<"H^"<<programName;
                for(int i=0;i<6-programName.length();i++)
                {
                    cout<<" ";
                }
                outputFileStream<<"^"<<setw(6)<<setfill('0')<<hex<<startingAddress<<"^";
                outputFileStream<<setw(6)<<setfill('0')<<hex<<programLength<<"\n";
                cout<<"^"<<setw(6)<<setfill('0')<<hex<<startingAddress<<"^";
                cout<<setw(6)<<setfill('0')<<hex<<programLength<<"\n";
                firstAddress=address; // Store first address encountered.
                continue; // Move to next line.
            }

            if(opcode == "END") // Check if opcode is 'END'.
            {
                if(!res)
                {
                    // Write text record to output file.
                    outputFileStream<<"T^"<<setw(6)<<setfill('0')<<firstAddress<<"^"<<setw(2)<<setfill('0')<<hex<<textRecordLength<<T.str()<<"\n";
                    cout<<"T^"<<setw(6)<<setfill('0')<<firstAddress<<"^"<<setw(2)<<setfill('0')<<hex<<textRecordLength<<T.str()<<"\n";
                }
                break; // Exit loop if 'END' encountered.
            }

            res = false; // Reset RESW/RESB flag.
            // Check for RESW/RESB instructions.
            if((opcode=="RESW") || (opcode=="RESB"))
            {
                // Write text record to output file.
                outputFileStream<<"T^"<<setw(6)<<setfill('0')<<firstAddress<<"^"<<setw(2)<<setfill('0')<<hex<<textRecordLength<<T.str()<<"\n";
                cout<<"T^"<<setw(6)<<setfill('0')<<firstAddress<<"^"<<setw(2)<<setfill('0')<<hex<<textRecordLength<<T.str()<<"\n";
                textRecordLength=0; // Reset text record length.
                T.str(""); // Clear text record stringstream.
            }
            // Skip lines until a non-RESW/RESB instruction is found.
            while((opcode=="RESW") || (opcode=="RESB"))
            {
                std::getline(inputFileStream, line); // Read next line.
                tie(address, label, opcode, operand) = parseLinePass2(line); // Parse line into components.
                res=true; // Set RESW/RESB flag.
            }
            if(res)
            {
                firstAddress=address; // Update first address if RESW/RESB encountered.
            }

            O.str(""); // Clear object code stringstream.
            if (opTable.search(opcode)) // Check if opcode is found in operation table.
            {
                if (!operand.empty()) // Check if operand is not empty.
                {
                    bool relative = false; // Flag to indicate relative addressing mode.
                    string actualOperand = ""; // String to store actual operand value.
                    for (size_t i = 0; i < operand.length() - 2; ++i) // Iterate through operand.
                    {
                        actualOperand += operand[i]; // Append character to actual operand.
                        if (operand[i + 1] == ',' && operand[i + 2] == 'X') // Check for indexed addressing.
                        {
                            relative = true; // Set relative addressing flag.
                            break; // Exit loop.
                        }
                    }
                    if (relative) // If relative addressing.
                    {
                        if (symTable.search(actualOperand)) // If symbol found in symbol table.
                        {
                            O << opTable.retrieve(opcode) << setw(4) << setfill('0') << hex << symTable.retrieve(actualOperand) + 32768; // Append object code with base address.
                        } 
                        else // If symbol not found.
                        {
                            O << opTable.retrieve(opcode) << "0000"; // Append object code with zeros.
                            cout << "ERROR Operand: \"" << operand << "\" UNDEFINED\n"; // Error: Undefined symbol.
                            errorUndefined++; // Increment undefined symbol error count.
                        }
                    } 
                    else if (symTable.search(operand)) // If symbol found in symbol table.
                    {
                        O << opTable.retrieve(opcode) << setw(4) << setfill('0') << hex << symTable.retrieve(operand); // Append object code with address.
                    }
                    else // If symbol not found.
                    {
                        O << opTable.retrieve(opcode) << "0000"; // Append object code with zeros.
                        cout << "ERROR Operand: \"" << operand << "\" UNDEFINED\n"; // Error: Undefined symbol.
                        errorUndefined++; // Increment undefined symbol error count.
                    }
                } 
                else // If operand is empty.
                {
                    O << opTable.retrieve(opcode) << "0000"; // Append object code with zeros.
                }
                increment= 3; // Set increment value.
            } 
            else if ((opcode == "BYTE") || (opcode == "WORD")) // Check for BYTE or WORD instructions.
            {
                // Handle BYTE and WORD instructions
                if (opcode == "BYTE") {
                    if (operand[0] == 'C') // If operand is character.
                    {
                        for (size_t i = 2; i < operand.length() - 1; ++i) // Iterate through characters.
                        {
                            O << hex << (int)operand[i]; // Convert character to hex and append to object code.
                        }
                        increment= operand.length() - 3; // Set increment value.
                    } 
                    else if (operand[0] == 'X') // If operand is hexadecimal.
                    {
                        for (size_t i = 2; i < operand.length() - 1; ++i) // Iterate through characters.
                        {
                            O << (char)tolower(operand[i]); // Convert character to lowercase and append to object code.
                        }
                        increment= (operand.length() - 3) / 2; // Set increment value.
                    } 
                    else // If operand is decimal.
                    {
                        O << setw(6) << setfill('0') << hex << stoi(operand); // Append decimal value to object code.
                        increment= 3; // Set increment value.
                    }
                } 
                else if (opcode == "WORD") // If opcode is 'WORD'.
                {
                    O << setw(6) << setfill('0') << hex << stoi(operand); // Append operand value to object code.
                    increment = 3; // Set increment value.
                }
            }
            textRecordLength+=increment; // Update text record length.
            if (textRecordLength > 30) // Check if text record length exceeds limit.
            {
                // Write text record to output file.
                outputFileStream << "T^" << setw(6) << setfill('0') << firstAddress << "^" << setw(2) << setfill('0') << hex << textRecordLength - 3 << T.str() << "\n";
                cout << "T^" << setw(6) << setfill('0') << firstAddress << "^" << setw(2) << setfill('0') << hex << textRecordLength - 3 << T.str() << "\n";
                textRecordLength = increment; // Reset text record length.
                firstAddress = address; // Update first address.
                T.str(""); // Clear text record stringstream.
            }
            T << "^" << O.str(); // Append object code to text record.
        }
        // Write end record to output file.
        outputFileStream<<"E^"<<setw(6)<<setfill('0')<<hex<<startingAddress;
        cout<<"E^"<<setw(6)<<setfill('0')<<hex<<startingAddress;
        inputFileStream.close(); // Close input file.
        outputFileStream.close(); // Close output file.
        return true; // Return success status.
    }


    void print_stats()
    {
        cout<<"\n\nSTATS:\n";
        cout<<"REDEFINED SYMBOLS : "<<errorRedefined<<"\n";
        cout<<"UNDEFINED SYMBOLS : "<<errorUndefined<<"\n";
        cout<<"INVALID OPCODES   : "<<errorInvalidOpcode<<"\n";
    }
};
#endif // ASSEMBLER_H