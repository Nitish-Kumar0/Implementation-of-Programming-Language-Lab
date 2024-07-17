#ifndef LOADER_H
#define LOADER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <iomanip>
#include <bits/stdc++.h>

using namespace std;

class Loader
{
private:
    // Inner class to represent records in the input file
    class Record
    {
    public:
        int startAddress;
        string textRecord;

        // Static method to search for a text record in a vector of records
        static int searchTextRecord(vector<Record> textRecords, int numAddress)
        {
            for (int i = 0; i < textRecords.size() - 1; i++)
            {
                if (textRecords[i + 1].startAddress > numAddress)
                    return i;
            }
            return textRecords.size() - 1;
        }
    };

    int ERROR_FLAG = 0;
    int programAddress;
    int controlSectionAddress;
    int controlSectionLength;
    int executionAddress;

public:
    string inputFile;
    string outputFile = "loader_output.txt";
    unordered_map<string, int> externalSymbolTable;
    ifstream fileInput;
    ofstream fileOutput;

    int getProgramAddress()
    {
        string programAddr;
        cout << "Enter the program address (PROGADDR): ";
        cin >> programAddr;
        programAddress = stoi(programAddr, nullptr, 16);
        return 1;
    }

    string removeCaret(const string &str)
    {
        string result;
        for (char c : str)
        {
            if (c != '^')
            {
                result += c;
            }
        }
        return result;
    }

public:
    // Constructor
    Loader(string input)
    {
        inputFile = input;
        fileInput.open(inputFile);
        fileOutput.open(outputFile);
        if (!fileInput.is_open() || !fileOutput.is_open())
        {
            cout << "Error opening files." << endl;
        }
    }

    // Function for performing pass one of loading process
    int performPassOne(ifstream &inputFile)
    {
        // Get program address from the user
        getProgramAddress();
        controlSectionAddress = programAddress;
        string record;
        while (inputFile.good())
        {
            // Read header record
            getline(inputFile, record);
            record = removeCaret(record);

            if (record.length() == 0)
                continue;
            string controlSectionName = record.substr(1, 6);
            string length = record.substr(13, 6);

            controlSectionLength = (int)strtol(length.c_str(), NULL, 16);

            // Check for duplicate external symbols
            if (externalSymbolTable.find(controlSectionName) != externalSymbolTable.end())
            {
                cout << "Error: Duplicate external symbol." << endl;
                ERROR_FLAG = 1;
                return -1;
            }
            else
            {
                // Insert control section name and address into the external symbol table
                externalSymbolTable[controlSectionName] = controlSectionAddress;
            }
            while (record[0] != 'E')
            {
                // Read next input line
                getline(inputFile, record);
                record = removeCaret(record);
                if (record[0] == 'D')
                {
                    // Iterate over all symbols in record
                    for (int i = 1; i < record.length() - 1; i += 12)
                    {
                        string symbol = record.substr(i, 6);
                        string addr = record.substr(i + 6, 6);

                        // Check for duplicate external symbols
                        if (externalSymbolTable.find(symbol) != externalSymbolTable.end())
                        {
                            cout << "Error: Duplicate external symbol." << endl;
                            ERROR_FLAG = 1;
                        }
                        else
                        {
                            // Insert symbol into external symbol table
                            externalSymbolTable[symbol] = controlSectionAddress + (int)strtol(addr.c_str(), NULL, 16);
                        }
                    }
                }
            }
            controlSectionAddress += controlSectionLength;
        }
        cout << "PASS ONE COMPLETED." << endl;
        cout << "External Symbol Table (ESTAB):" << endl;
        // Print external symbol table
        for (auto entry : externalSymbolTable)
        {
            cout << entry.first << " " << entry.second << endl;
        }
        return 1;
    }

    // Function for performing pass two of loading process
    int performPassTwo(ifstream &inputFile, ofstream &outputFile)
    {
        inputFile.seekg(0, inputFile.beg);
        controlSectionAddress = programAddress;
        executionAddress = programAddress;
        string record;
        int lastLine = 0;
        vector<Record> textRecords;
        vector<vector<string>> memory(1 << 12, vector<string>(4, "........"));

        while (inputFile.good())
        {
            getline(inputFile, record);
            record = removeCaret(record);

            if (record.length() == 0)
                continue;
            string controlSectionName = record.substr(1, 6);
            string length = record.substr(13, 6);
            controlSectionLength = (int)strtol(length.c_str(), NULL, 16);

            while (record[0] != 'E')
            {
                getline(inputFile, record);
                record = removeCaret(record);

                if (record[0] == 'T')
                {
                    string addr = record.substr(1, 6);
                    int startAddress = (int)strtol(addr.c_str(), NULL, 16);

                    Record temp;
                    temp.startAddress = startAddress;
                    temp.textRecord = record;
                    textRecords.push_back(temp);
                }

                if (record[0] == 'M')
                {
                    string addr = record.substr(1, 6);
                    string len = record.substr(7, 2);
                    string symbol = record.substr(10, 6);
                    while (symbol.length() < 6)
                    {
                        symbol = symbol + ' ';
                    }

                    int numAddress = (int)strtol(addr.c_str(), NULL, 16);

                    int index = Record::searchTextRecord(textRecords, numAddress);

                    int firstAddress = textRecords[index].startAddress;
                    string rec = textRecords[index].textRecord;
                    int numLen = (int)strtol(len.c_str(), NULL, 16);

                    string toChange = rec.substr(9 + (numAddress - firstAddress) * 2 + numLen % 2, numLen);
                    int value = (int)strtol(toChange.c_str(), NULL, 16);

                    if (externalSymbolTable.find(symbol) == externalSymbolTable.end())
                    {
                        ERROR_FLAG = 1;
                        return -1;
                    }

                    if (record[9] == '+')
                    {
                        value += externalSymbolTable[symbol];
                    }
                    else
                    {
                        value -= externalSymbolTable[symbol];
                    }

                    stringstream ss;
                    ss << hex << value;
                    string res = ss.str();
                    while (res.length() < numLen)
                        res = '0' + res;

                    for (int i = 0; i < numLen; i++)
                    {
                        rec[9 + (numAddress - firstAddress) * 2 + numLen % 2 + numLen - i - 1] = toupper(res[res.length() - 1 - i]);
                    }
                    textRecords[index] = {firstAddress, rec};
                }
            }

            for (int i = 0; i < textRecords.size(); i++)
            {
                for (int j = 0; j < textRecords[i].textRecord.length() - 9; j++)
                {
                    int I = ((controlSectionAddress + textRecords[i].startAddress) * 2 + j) / 32;
                    int J = ((controlSectionAddress + textRecords[i].startAddress) * 2 + j) % 32;
                    memory[I][J / 8][J % 8] = textRecords[i].textRecord[j + 9];
                    lastLine = max(lastLine, I);
                }
            }
            textRecords.clear();
            controlSectionAddress += controlSectionLength;
        }

        if(max(programAddress / 16 - 3, 0)!=0)
        {

             outputFile << uppercase << setfill('0') << setw(4) << hex << 0;
            outputFile << "    " << memory[memory.size() - 1][0] << ' ' << memory[0][1] << ' ' << memory[0][2] << ' ' << memory[0][3] << endl;
            outputFile << "\t\t\t.\t\t.\t\t.\t\t.\n";
            outputFile << "\t\t\t.\t\t.\t\t.\t\t.\n";
            outputFile << "\t\t\t.\t\t.\t\t.\t\t.\n";
            outputFile << "\t\t\t.\t\t.\t\t.\t\t.\n";
            outputFile << "\t\t\t.\t\t.\t\t.\t\t.\n";
        }

        for (int i = max(programAddress / 16 - 3, 0); i < min((int)memory.size(), lastLine + 5); i++)
        {
            outputFile << uppercase << setfill('0') << setw(4) << hex << i * 16;
            outputFile << "    " << memory[i][0] << ' ' << memory[i][1] << ' ' << memory[i][2] << ' ' << memory[i][3] << endl;
        }

        if (min((int)memory.size(), lastLine + 5) != memory.size())
        {

            outputFile << "\t\t\t.\t\t.\t\t.\t\t.\n";
            outputFile << "\t\t\t.\t\t.\t\t.\t\t.\n";
            outputFile << "\t\t\t.\t\t.\t\t.\t\t.\n";
            outputFile << "\t\t\t.\t\t.\t\t.\t\t.\n";
            outputFile << "\t\t\t.\t\t.\t\t.\t\t.\n";

            outputFile << uppercase << setfill('0') << setw(4) << hex << (memory.size() - 1) * 16;
            outputFile << "    " << memory[memory.size() - 1][0] << ' ' << memory[memory.size() - 1][1] << ' ' << memory[memory.size() - 1][2] << ' ' << memory[memory.size() - 1][3] << endl;
        }

        cout << "PASS TWO COMPLETED." << endl;
        return 1;
    }
};

#endif // LOADER_H
