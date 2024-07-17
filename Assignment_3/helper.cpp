#include <iostream>
#include <vector>
#include <list>
#include <fstream>
using namespace std;

// Define constants
#define MAX_HASH_SIZE 13
#define MAX_TOKEN_LEN 100

class HashTable {
    vector<list<string>> table;
    int size;

    int getHashValue(const string& s) {
        int hash = 0;
        for (char ch : s) {
            hash = (hash + ch) % size;
        }
        return hash;
    }

public:
    HashTable(int tableSize) : size(tableSize) {
        table.resize(size);
    }

    bool insert(const string& value) {
        int index = getHashValue(value);
        list<string>& slot = table[index];
        for (const string& item : slot) {
            if (item == value) {
                return false; // Value already exists
            }
        }
        slot.push_back(value);
        return true;
    }

    bool search(const string& value) {
        int index = getHashValue(value);
        list<string>& slot = table[index];
        for (const string& item : slot) {
            if (item == value) {
                return true; // Value found
            }
        }
        return false; // Value not found
    }

    void printTable() {
        for (int i = 0; i < size; ++i) {
            cout << "bucket [" << i << "]: ";
            if (table[i].empty()) {
                cout << "nullptr";
            } else {
                for (const string& value : table[i]) {
                    cout << value << " -> ";
                }
                cout << "nullptr";
            }
            cout << endl;
        }
    }
};

// Global variables
bool afterBegin = 0;
int newLineFlag = 1;
int lineNumber = 1;
string outputFile = "output.txt";
ofstream fout;
HashTable idTable(MAX_HASH_SIZE);
HashTable intTable(MAX_HASH_SIZE);

// Function to print token information
void printTokenInfo(int tokenCode, string tokenSpec = "") {
    if (newLineFlag) {
        fout << lineNumber << "\t\t";
        newLineFlag = 0;
    } else {
        fout << " " << "\t\t";
    }

    fout << tokenCode << "\t\t";
    if (tokenCode == 22) {
        fout << "^";
    }
    if (tokenCode == 23) {
        fout << "#";
    }
    fout << tokenSpec << endl;
}

// Function to install identifier in the symbol table
int install_id(string symbol) {
    if(!afterBegin)
    {
        if (!idTable.insert(symbol)) {
            //cerr << "Error: Identifier " << symbol << " already exists in the symbol table!" << endl;
            return 0; // or handle the error appropriately
        }
        return 1;
    }
    else
    {
        if(!idTable.search(symbol))
        {
            cerr << "Error: Identifier " << symbol << " is Unknown!" << endl;
            //idTable.insert(symbol);
            return 0;
        }
    }
    return 1;
}

// Function to install number in the symbol table
int install_num(string number) {
    if (!intTable.insert(number)) {
        //cerr << "Error: Number " << number << " already exists in the symbol table!" << endl;
        return 0; // or handle the error appropriately
    }
    return 1;
}