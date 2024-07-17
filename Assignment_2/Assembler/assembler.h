#ifndef ASSEMBLER_H
#define ASSEMBLER_H


#include <bits/stdc++.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <tuple>
#include <map>
#include <string>
using namespace std;
#define DELIMITER '^'


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
            opTable["ADD"] = "18";
            opTable["COMP"] = "28";
            opTable["DIV"] = "24";
            opTable["J"] = "3C";
            opTable["JEQ"] = "30";
            opTable["JGT"] = "34";
            opTable["JLT"] = "38";
            opTable["JSUB"] = "48";
            opTable["LDA"] = "00";
            opTable["LDCH"] = "50";
            opTable["LDL"] = "08";
            opTable["LDX"] = "04";
            opTable["MUL"] = "20";
            opTable["RD"] = "D8";
            opTable["RSUB"] = "4C";
            opTable["STA"] = "0C";
            opTable["STCH"] = "54";
            opTable["STL"] = "14";
            opTable["STX"] = "10";
            opTable["SUB"] = "1C";
            opTable["TD"] = "E0";
            opTable["TIX"] = "2C";
            opTable["WD"] = "DC";
            opTable["LDB"] = "68";
            opTable["LDT"] = "74";
            opTable["COMPR"] = "A0";
            opTable["CLEAR"] = "B4";
            opTable["TIXR"] = "B8";
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
        map<string, string> symTable; // Map to store symbol and corresponding address.

    public:
        bool search(const string &symbol) // Method to search for a symbol in the symbol table.
        {
            return (symTable.find(symbol) != symTable.end()); // Return true if symbol found, false otherwise.
        }

        void insert(const string &symbol, string address) // Method to insert a symbol and its address into the symbol table.
        {
            symTable[symbol] = address; // Insert symbol and address into the symbol table.
        }

        string retrieve(const string &symbol) // Method to retrieve address for a given symbol.
        {
            return symTable[symbol]; // Return address corresponding to the symbol.
        }
    };
    

    private:
    string line,label, opcode, operand, record, recordAddress, lineAddress;
    unordered_map<string,int> sectionLengths;
    unordered_map<string, string> littab,templittab;
    unordered_set<string> type2_instructions = {"COMPR","CLEAR","TIXR","ADDR"};
    vector<string> ext_refs, ext_defs;
    vector<string> mod_recs;
    OpCodeTable opTable; // Object of OpCodeTable class to manage opcode table.
    SymbolTable symTable; // Object of SymbolTable class to manage symbol table.
    ifstream input; 
    ofstream output;
    string progName,startAddress;
    string inputfile;

    public:
    Assembler(string inp) // Constructor to initialize the assembler with an input file.
    {
        inputfile = inp;
    }

    bool pass1()
    {
        input.open(inputfile);
        output.open("intermediate.txt");
        int currentAddress = 0,progLength = 0;
        if (input.peek() == std::ifstream::traits_type::eof()) { // Check if file is empty
            std::cout << "Empty file given." << std::endl;
            input.close();
            output.close();
            return false;
        }
        getline(input,line); // Get the first line from input file
        split(line,false); // Splits the line into label opcode operand
        if(opcode == "START")
        {
            currentAddress = hexToInt(operand);
            startAddress = operand;
            progName = label;
            output << left<<setw(6) << intToHex(currentAddress) << setw(10) << label << setw(10) << opcode << setw(20) << operand << endl;
        }
        else
        { 
            cout<<"START statement not found"<<endl;
        }
        while(getline(input,line))
        {
            // Loop over every line in the file
            if (line.size()>0){
                
                split(line,false);
                if(opcode[0] == '.') continue;
                
                else if (opcode=="CSECT"){
                    sectionLengths.insert({progName,currentAddress - hexToInt(startAddress)});
                    progName = label;
                    currentAddress = 0;
                    output << left << setw(6) << formate(intToHex(currentAddress), 4,'0',false) << setw(10) << label << setw(10) << opcode << setw(20) << "" << endl;
                    continue;
                }
                if(operand[0]=='='){
                    if(templittab.count(operand.substr(1))) ;
                    else templittab[operand.substr(1)] = "";
                }
                if (opcode == "LTORG" || opcode =="END")
                {
                    for (auto elem : templittab)
                    {
                        littab[elem.first] = intToHex(currentAddress);
                        output << left << setw(6) << formate(intToHex(currentAddress), 4,'0',false) << setw(10) << "" << setw(10) << "*" << setw(20) << elem.first << endl;
                        if (elem.first[0] == 'C')
                        {
                            int len = elem.first.length() - 3;
                            currentAddress += len;
                        }
                        else if (elem.first[0] == 'X')
                        {
                            int len = elem.first.length() - 3;
                            currentAddress += len / 2;
                        }
                    }
                    templittab.clear();
                    if(opcode=="LTORG") continue;
                    else{
                        ;
                    }
                }
                else if (opcode == "EQU")
                {
                    if (operand == "*"){
                        operand = intToHex(currentAddress); 
                        symTable.insert(label, operand);
                    }
                        
                    else
                    {
                        if (is_number(operand))
                        {
                            symTable.insert(label, operand);
                        }
                        
                    }
                    continue;
                }
                output << left<< setw(6) << formate(intToHex(currentAddress),4,'0',false) << setw(10) << label << setw(10) << opcode << setw(20) << operand << endl;
                if (label != ""){
                    if(symTable.search(label)) 
                    {
                        cout<<"Duplicate label"<<endl;
                        return false;
                    }
                    else symTable.insert(label, intToHex(currentAddress));
                }
                // Check what opcode is and increment the currentAddress accordingly
                
                if(opcode=="COMPR" || opcode=="CLEAR" || opcode =="TIXR"){
                    currentAddress+=2;
                }
                else if(opTable.search(opcode)) currentAddress+=3;
                
                else if (opcode == "WORD") currentAddress+=3;
                else if (opcode == "RESW") currentAddress +=(3*stoi(operand));
                else if (opcode == "RESB") currentAddress+=stoi(operand);
                else if (opcode == "BYTE"){
                    if(operand[0] == 'X')
                        currentAddress+=(operand.length()-3)/2;
                    else if (operand[0] == 'C')
                        currentAddress +=operand.length()-3;
                }
                else if(opcode =="START") cout << "Illegal START statement" << endl;
                else if(opcode == "END") {
                    ;
                }
                else if(opcode[0]=='+'){
                    currentAddress+=4;
                }
                else if (opcode == "EXTREF" || opcode =="EXTDEF"){
                    ;
                }
                

                else 
                {
                    cout << "Illegal opcode " << opcode << " " << endl;
                    return false;
                }
                
            }	
        }
        sectionLengths.insert({progName,currentAddress - hexToInt(startAddress)});
        input.close();
        output.close();
        cout << "PASS ONE COMPLETED.\n" << endl;
        return true;
    }

    bool pass2()
    {
        bool firstSection = true;
        input.open("intermediate.txt");
        // ofstream fout;
        // fout.open("listing.txt");
        output.open("output.txt");
        int recordLength=0;
        int programCounter = 0;
        add_registers();

        while (getline(input, line)) // Read the intermediate file
        {
            split(line,true);
            
            if(recordAddress=="") recordAddress=lineAddress;
            if(opcode == "START"){
                // Output the header record to output file
                //fout<<line<<"\n";
                output<<"H"<<"^"<<formate(label,6,' ',true)<<"^"<<formate(startAddress,6,'0',false)<<"^"<<formate(intToHex(sectionLengths[label]),6,'0',false)<<endl;
            }
            else if(opcode == "RESW" || opcode =="RESB"){
                // Incase of RESW or RESB a new record needs to be started
                if(recordLength>0){
                    output << "T^" + formate(recordAddress,6,'0',false) + "^" + formate(intToHex(recordLength), 2,'0',false)  + record << endl;
                    record = "";
                    recordLength = 0;
                    continue;
                }
                recordAddress = "";
                if(opcode == "RESW") programCounter+=3*stoi(operand);
                else
                    programCounter += stoi(operand);
            }
            else if (opcode == "EXTREF")
            {

                ext_refs.clear();
                ext_refs = splitDelimiter(operand,',');
                output << "R";
                for(auto elem:ext_refs){
                    output<<"^"<<formate(elem,6,' ',true);
                }
                output<<endl;
            }
            else if (opcode == "EXTDEF"){
                ext_defs.clear();
                ext_defs = splitDelimiter(operand,',');
                output << "D";
                for (auto elem : ext_defs)
                {
                    output << "^"<<formate(elem, 6, ' ', true) << "^"<<formate(symTable.retrieve(elem),6,'0',false);
                }
                output << endl;
            }
            else{
                // Handle the case of WORD and BYTE instructions
                if(opcode == "WORD"){
                    if(is_number(operand)) record+= DELIMITER + formate(intToHex(stoi(operand)),6,'0',false);
                    else{
                        record += DELIMITER + formate("0",6,'0',false); 
                        // put in function
                        if (operand.find('-') != std::string::npos)
                        {
                            vector<string> operands = splitDelimiter(operand,'-');
                            mod_recs.push_back(formate(intToHex(programCounter+1),6,'0',false) + "^06" + "^+" + operands[0]);
                            mod_recs.push_back(formate(intToHex(programCounter+1),6,'0',false) + "^06" + "^-" + operands[1]);
                        }

                        else if (operand.find('+') != std::string::npos)
                        {
                            vector<string> operands = splitDelimiter(operand,'+');
                            mod_recs.push_back(formate(intToHex(programCounter+1),6,'0',false) + "^06" + "^+" + operands[0]);
                            mod_recs.push_back(formate(intToHex(programCounter+1),6,'0',false) + "^06" + "^+" + operands[1]);
                        }
                        else
                        {
                            cout<<"Unsupported symbol foud in operand : "<<operand<<endl;
                            return false;
                        }
                            
                    }
                        
                    recordLength+=3;
                    
                }
                else if (opcode =="BYTE"){
                    
                    if(operand[0]=='C'){
                        record+=DELIMITER;
                        for(int i=2;i<operand.length()-1;i++){
                            record+=intToHex((int)operand[i]);
                            recordLength+=1;
                        }
                        
                    }
                    else if(operand[0]=='X'){
                        record+= DELIMITER + operand.substr(2,operand.length() - 3);
                        recordLength+=(operand.length()-3)/2;
                        
                    }
                }
                else{
                    // Handle various cases of opcode and operand
                    int type = 3, indirect = 1, immediate = 1, index = 0, base = 0, pc_relative = 1,extended = 0,literal= 0;
                    int padding = type;
                    if (operand.length() > 2 && operand.substr(operand.length() - 2, 2) == ",X") //indexed addressing
                    {
                        index = 1;
                        
                        operand = operand.substr(0, operand.length() - 2);
                        
                    }
                    if(opcode[0] == '+'){
                        extended = 1;
                        pc_relative = 0;
                        opcode = opcode.substr(1);
                        type = 4;
                        padding = 5;
                        mod_recs.push_back(formate(intToHex(programCounter+1),6,'0',false)+"^05"+"^+"+operand);
                    }
                    
                    if(operand[0] == '#'){
                        indirect = 0;
                        operand = operand.substr(1);
                    }
                    else if (operand[0]=='@'){
                        immediate = 0;
                        operand = operand.substr(1);
                    }
                    else if(operand[0]=='='){
                        operand = operand.substr(1);
                        literal = 1;
                    }
                    
                    if(type2_instructions.count(opcode)){
                        type = 2;
                        pc_relative = 0;
                        padding = type;
                    }
                    string target;

                    
                    if (opcode == "CLEAR" || opcode == "TIXR"){
                        type = 2;
                        pc_relative = 0;
                        programCounter +=type;
                        record += (DELIMITER + opTable.retrieve(opcode) + formate(symTable.retrieve(operand),2,'0',true));
                        recordLength+=2;
                    }
                    else if(opcode == "COMPR"){
                        type = 2;
                        programCounter+=type;
                        record += (DELIMITER + opTable.retrieve(opcode) + symTable.retrieve(operand.substr(0,1))+symTable.retrieve(operand.substr(2)));
                        recordLength += 2;
                    }
                    else if (opTable.search(opcode) && (symTable.search(operand) || find(ext_refs.begin(), ext_refs.end(), operand) != ext_refs.end() || indirect == 0 || literal || index))
                    {
                        programCounter += type;

                        if (find(ext_refs.begin(), ext_refs.end(), operand) != ext_refs.end())
                            target = formate("", padding,'0',false);
                        else if (literal)
                        {
                            
                            target = formate(intToHex((hexToInt(littab[operand]) - programCounter)), type,'0',false);
                        }
                        else if(index){
                            target = formate("0", 5,'0',false); //Change this
                        }
                        else if (symTable.search(operand))
                        {
                            if (hexToInt(symTable.retrieve(operand)) > programCounter)
                                target = formate(intToHex((hexToInt(symTable.retrieve(operand)) - programCounter)), padding,'0',false);
                            else
                            {
                                int diff = hexToInt(symTable.retrieve(operand)) - programCounter;
                                target = formate(intToHex(diff), padding,'0',false);
                            }
                        }
                        else if (indirect == 0)
                        {
                            if (is_number(operand))
                                target = formate(operand, type,'0',false);
                            else
                                target = formate(symTable.retrieve(operand), type,'0',false);
                        }
                        
                        
                        record += (DELIMITER + get_initial(opTable.retrieve(opcode), indirect, immediate, index, base, pc_relative, extended,8)) + target;
                        recordLength += type;
                    }
                    else if(opcode =="RSUB"){
                        programCounter += type;
                        pc_relative = 0;
                        record += DELIMITER + get_initial(opTable.retrieve(opcode), indirect, immediate, index, base, pc_relative, extended,8) + formate("0", 4,'0',false);
                        recordLength += 3;
                    }
                    else if (operand.length()>2 && operand.substr(operand.length() - 2, 2) == ",X")  //indexed addressing
                    {
                        index = 1;
                        programCounter += type;
                        operand = operand.substr(0, operand.length() - 2);
                        int shifted_address = 32768+hexToInt(symTable.retrieve(operand));
                        target = formate("0",5,'0',false); //Change this 
                        record += (DELIMITER + get_initial(opTable.retrieve(opcode), indirect, immediate, index, base, pc_relative, extended, 8)) + target;
                        recordLength += type;
                    }
                    else if(opcode == "END"){
                        if (recordLength > 0)
                        {
                            output << "T^" + formate(recordAddress,6,'0',false) + "^" + formate(intToHex(recordLength), 2,'0',false)  + record << endl;
                            record = "";
                            recordLength = 0;
                        }
                        for (auto elem : mod_recs)
                        {
                            output << "M^" + elem << endl;
                        }
                        mod_recs.clear();
                        recordAddress = "";
                        output << "E";
                        if (firstSection)
                            output << "^" << formate(startAddress,6,'0',false) << endl
                                << endl;
                        else
                            output  << endl
                                << endl;
                    }
                    else if(opcode =="*"){
                        if (operand[0] == 'C')
                        {
                            record += DELIMITER;
                            for (int i = 2; i < operand.length() - 1; i++)
                            {
                                record += intToHex((int)operand[i]);
                                recordLength += 1;
                                
                            }
                            programCounter += (operand.length() - 3);
                        }
                        else if (operand[0] == 'X')
                        {
                            record += DELIMITER + operand.substr(2, operand.length() - 3);
                            recordLength += (operand.length() - 3) / 2;
                            programCounter += (operand.length() - 3) / 2;
                        }
                    }
                    else if(operand=="CSECT"){
                        
                        if (recordLength > 0)
                        {
                            output << "T^" + formate(recordAddress,6,'0',false) + "^" + formate(intToHex(recordLength), 2,'0',false) + record << endl;
                            record = "";
                            recordLength = 0;
                        }
                        for(auto elem:mod_recs){
                            output<<"M^"+elem<<endl;
                        }
                        mod_recs.clear();
                        recordAddress = "";
                        output << "E" ;
                        if(firstSection) output << "^"<<formate(startAddress,6,'0',false) << endl<< endl;
                        else output <<endl<<endl;
                        programCounter = 0;
                        firstSection = false;
                        output << "H"<< "^" << formate(opcode, 6, ' ', true) << "^" << formate(startAddress,6,'0',false) << "^" << formate(intToHex(sectionLengths[opcode]),6,'0',false) << endl;
                    }
                    else
                    { 
                        cout<<"Opcode or operand not found "<<"opcode : "<<opcode<<" operand : "<<operand<<endl;
                        return false;
                    }
                }
                if(recordLength>27){ // If recordLength has exceeded 27 a new record has to be started
                    output<<"T^"+formate(recordAddress,6,'0',false)+"^"+formate(intToHex(recordLength),2,'0',false)+record<<endl;
                    record = "";
                    recordAddress="";
                    recordLength=0;
                }
            }
        }
        cout << "PASS TWO COMPLETED.\n" << endl;
        return true;
    }


    string intToHex(int x)
    {
        //Takes an integer and return Hex string corresponding to it
        stringstream hexStream;
        hexStream<<hex<<x;
        string res(hexStream.str());
        
        return toUpper(res);
    }

    string toUpper(string x)
    {
        for (auto &c : x)
            c = toupper(c);
        return x;
    }

    int hexToInt(string x)
    {
        // Takes a hex string and returns corresponding decimal integer
        int res;
        stringstream intStream;
        intStream << x;
        intStream >> hex >> res; 

        return res;
    }

    string hexToBin(string x)
    {
        // Takes a hex string and returns corresponding decimal integer
        stringstream ss;
        ss << hex << x;
        unsigned n;
        ss >> n;
        bitset<8> b(n);
        return b.to_string();
    }

    string binToHex(string x)
    {
        stringstream ss;
        ss << hex << stoll(x, NULL, 2);
        return ss.str();
    }

    void strip(string &x)
    {
        // Strips all the whitespace from a string
        x.erase(remove(x.begin(), x.end(), ' '), x.end());
    }

    void split(string const &input, bool intermediate )
    {
        // intermediate is true if the line is read from intermediate file and false if read from input.asm
        // Split the string based on whitespace and store it in lineAddress label opcode and operand
        istringstream buffer(input);
        vector<string> ret((istream_iterator<string>(buffer)), istream_iterator<string>());
        for (int i = 0; i < ret.size(); i++)
        {
            strip(ret[i]);
        }
        if (intermediate)
        {
            if (ret.size() == 4)
                lineAddress = ret[0], label = ret[1], opcode = ret[2], operand = ret[3];
            else if (ret.size() == 3)
                lineAddress = ret[0], label = "", opcode = ret[1], operand = ret[2];
            else if (ret.size() == 2)
                lineAddress = ret[0], label = "", opcode = ret[1], operand = "";
            else
                cout << "Error in line " << line << endl;
        }
        else
        {

            if (ret.size() == 3)
                label = ret[0], opcode = ret[1], operand = ret[2];

            else if (ret.size() == 2){
                if(ret[1] == "CSECT")
                    label = ret[0], opcode = ret[1];

                else label = "", opcode = ret[0], operand = ret[1];

            }
                
            else
                label = "", opcode = ret[0], operand = "";
        }
    }

    string formate(string x, int len , char y, bool right )
    {
        // Pad a string to length len with char y on the right if right = true
        if(len <= x.length()) 
        {
            return x.substr(x.length()-len);
        }
        if (right)
        {
            return (x + string(len - x.length(), y));
        }
        return string(len - x.length(), y) + x;
    }

    vector<string> splitDelimiter(string x, char delim)
    {
        vector<string> vect;
        stringstream ss(x);
        string str;
        while (getline(ss, str, delim))
        {
            vect.push_back(str);
        }
        return vect;
    }

    string get_initial(string opcode, int n , int i, int x, int p, int b, int e, int len)
    {
        string opcodeBin = hexToBin(opcode);

        opcodeBin[len - 2] = (char)(n + '0');
        opcodeBin[len - 1] = (char)(i + '0');
        
        stringstream ss;
        ss << x<<p<<b<<e;
        opcodeBin=toUpper(binToHex(opcodeBin) + binToHex(ss.str()));
        
        return formate(opcodeBin,3,'0',false);
    }

    bool is_number(string &s)
    {
        return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
    }
    void add_registers(){
        symTable.insert("A","0");
        symTable.insert("X", "1");
        symTable.insert("L", "2");
        symTable.insert("B", "3");
        symTable.insert("S", "4");
        symTable.insert("T", "5");
        symTable.insert("F", "6");

    }


};


#endif
