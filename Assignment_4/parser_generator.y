%{
    // Include necessary headers
    void yyerror (const char *s);  // Function prototype for error handling
    int yylex();  // Function prototype for lexical analysis

    // Standard C++ headers
    #include <iostream>  // For input and output operations
    #include <fstream>   // For file input and output operations
    #include <vector>    // For dynamic arrays (vectors)
    #include <list>      // For linked lists
    #include <string>    // For string manipulation operations
    #include <iomanip>   // For setting output width with setw()
    using namespace std;  // Avoids typing std:: before standard library objects

    // Constants definitions
    #define MAX_SIZE 15  // Maximum size for hash table
    #define MAXLEN 1024  // Maximum length for strings

    // Global variables
    int errorFlag = 0; // Check for errors
    int newLine = 1;  // Used to check if token is the first word on a line
    int lineNo = 1;   // Indicates the current line number

    // Specify the name of output files
    string idTableFile = "id_table.txt";
    // Pointer to the output file
    ofstream fout;

    // Enumeration for numeric types
    enum NumType
    {
        IntegerNum,
        RealNum,
        NotDefined
    };

    // Enumeration for identifier types
    enum IDType
    {
        INTI,
        REALL,
        IDD,
        UNDECLARED
    };

    // Hash table class
    class HashTable
    {
        // Hash table structure
        vector<list<pair<string, pair<pair<NumType,IDType>,bool>>>> table;
        int size;

        // Helper function to generate a hash value for a string
        int getHashValue(string s)
        {
            int hash = 0;
            for (int i = 0; i < s.length(); i++)
            {
                hash = (hash + s[i]) % size;
            }
            return hash;
        }

    public:
        // Constructor for hash table
        HashTable(int size)
        {
            this->size = size;
            this->table = vector<list<pair<string, pair<pair<NumType,IDType>,bool>>>>(size);
        }

        // Function to insert a value into the hash table
        int insert(string value, NumType type, IDType idtype)
        {
            // Check if the value is already present
            if (search(value))
            {
                return 0;
            }

            // Else insert into the hash table
            int index = getHashValue(value);
            pair<NumType,IDType> pair2 = make_pair(type,idtype);
            pair<pair<NumType,IDType>,bool> pair1 = make_pair(pair2,false);
            table[index].emplace_back(value, pair1);
            return 1;
        }

        // Function to update the initialization status of an entry
        int update(string value)
        {
            int index = getHashValue(value);
            for (auto &pair : table[index])
            {
                if (pair.first == value)
                {
                    pair.second.second = true;
                    return 1;
                }
            }
            return 0;  // Entry not found
        }

        // Get the type of entry with a key value
        NumType getType(string value)
        {
            int index = getHashValue(value);
            for (const auto &pair : table[index])
            {
                if (pair.first == value)
                {
                    return pair.second.first.first;
                }
            }
            return NotDefined;  // Entry not found
        }

        // Function to search for a value in the hash table
        int search(string value)
        {
            int index = getHashValue(value);
            for (const auto &pair : table[index])
            {
                if (pair.first == value)
                {
                    return 1;  // Found
                }
            }
            return 0;  // Not found
        }
        
        // Function to check if an entry is initialized
        bool isInitialized(string value)
        {
            int index = getHashValue(value);
            for (const auto &pair : table[index])
            {
                if (pair.first == value)
                {
                    return pair.second.second;  // Return initialization status
                }
            }
            return false;  // Not initialized
        }

        // Function to print all values in the hash table to console
        void printTable()
        {
            // Printing the hash table content
            cout << "\n" << endl;
            cout << "-----------------------------------------" << endl;
            cout << "| Bucket | Content                     |" << endl;
            cout << "-----------------------------------------" << endl;
            for (int i = 0; i < size; ++i)
            {
                cout << "|   " << setw(4) << i << " | ";
                if (table[i].empty())
                {
                    cout << "NULL";
                }
                else
                {
                    for (const auto &pair : table[i])
                    {
                        cout << "(" << pair.first << ", " << pair.second.first.first <<", "<< pair.second.first.second << ", " << pair.second.second << ") -> ";
                    }
                    cout << "NULL";
                }
                cout << "\n";
            }
            cout << "-----------------------------------------" << endl;
        }
    };

    // Data structure to store IDs
    HashTable idTable(MAX_SIZE);  // Initialize hash table with specified size

    // Function to split a string into tokens
    vector<string> splitString(string s, string delimiter)
    {
        size_t pos = 0;
        string token;
        vector<string> v;
        while ((pos = s.find(delimiter)) != string::npos)
        {
            token = s.substr(0, pos);
            v.push_back(token);
            s.erase(0, pos + delimiter.length());
        }
        v.push_back(s);
        return v;
    }

    // Function to print the symbol table
    void printIdTable()
    {
        idTable.printTable();
    }

    // Function to declare identifiers with specified type
    int declareType(const char *str, NumType type)
    {
        string s = str;
        int res = 1;
        vector<string> ids = splitString(s, ",");
        for (string id : ids)
        {
            if (idTable.insert(id, type, IDD) == 0)
                res = -1;  // Symbol redeclaration detected
        }
        return res;
    }

    // Function to verify if identifiers are declared
    int verifyIds(const char *str)
    {
        string s = str;
        vector<string> ids = splitString(s, ",");
        for (string id : ids)
        {
            if (idTable.getType(id) == NotDefined)
                return -1;  // Undeclared symbol found
        }
        return 1;  // All symbols declared
    }

    // Function to get the type of a symbol
    NumType getType(const char *str)
    {
        return idTable.getType(str);
    }

    // Function to update the initialization status of symbols
    void update(const char *str)
    {
        string s = str;
        vector<string> ids = splitString(s, ",");
        for (string id : ids)
        {
            idTable.update(id);
        }
    }

    // Function to check if a symbol is initialized
    bool isInitialized(const char *str)
    {
        return idTable.isInitialized(str);
    }

    // Function to insert an integer into the symbol table
    void InsertInt(string str)
    {
        idTable.insert(str, IntegerNum, INTI);
    }

    // Function to insert a real number into the symbol table
    void InsertReal(string str)
    {
        idTable.insert(str, RealNum, REALL);
    }
    // to display error line number    
    extern int yylineno;  // External variable for line number tracking
%}

// Lexical analyzer definitions
%start prog
%define parse.error verbose

// Union definition for parser tokens
%union {
    char str[MAXLEN];  // String token
    int int_num;       // Integer token
    float real_num;    // Real number token
    NumType type;      // Token for numeric type
}

%token PROGRAM
%token VAR
%token BEGIN1
%token END
%token END_DOT
%token INTEGER
%token REAL
%token FOR
%token READ
%token WRITE
%token TO
%token DO
%token SEMICOLON
%token COLON
%token COMMA
%token EQUAL
%token ADD
%token SUB
%token MUL
%token DIV
%token OPENBRACKET
%token CLOSEBRACKET
%token CHARACTER
%token <str> ID
%token <type> INT
%token <type> REAL_NUM
%type <str> idlist
%type <type> type
%type <type> exp
%type <type> factor
%type <type> term


%% 
prog : PROGRAM progname VAR declist BEGIN1 stmtlist END_DOT 
    ;

progname : ID
    ;

declist : dec
    | declist SEMICOLON dec
    | error
    ;

dec : idlist COLON type {
                            // insert all symbols in id table with their type
                            // and check for redeclarations
                            if(declareType($1, $3) < 0)
                            {
                                cout << "Line " << yylineno << " : semantic error, symbol redeclared." << endl;
                                errorFlag = 1;
                            }
                        }
    ;

type : INTEGER          {
                            $$ = IntegerNum;
                        }

    | REAL              {
                            $$ = RealNum;
                        }
    ;

idlist : ID             {
                            strcpy($$, $1);
                        }

    | idlist COMMA ID   {
                            strcpy($$, $1); 
                            strcat($$, ","); 
                            strcat($$, $3);
                        }
    ;

stmtlist : stmt 
    | stmtlist SEMICOLON stmt
    | error
    ;

stmt : assign 
    | read
    | write
    | for
    ;

assign : ID EQUAL exp       {   
                                // if LHS not declared
                                if(getType($1) == NotDefined)
                                {
                                    cout << "Line " << yylineno << " : semantic error, symbol not declared." << endl;
                                    errorFlag = 1;
                                }
                                // if mismatch in LHS and RHS type
                                else if(getType($1) != $3 && $3 != NotDefined)
                                {
                                    cout << "Line " << yylineno << " : semantic error, type mismatch in assignment statement." << endl;
                                    errorFlag = 1;
                                }
                                else
                                {
                                    update($1);
                                }
                            }
    ;

exp : term              {
                            $$ = $1;
                        }
    | exp ADD term      {
                            // check for type mismatch
                            if($1 != $3)
                            {
                                // check if mismatch not reported already
                                if($3 != NotDefined && $1 != NotDefined)
                                    cout << "Line " << yylineno << " : semantic error, type mismatch in expression." << endl;
                                $$ = NotDefined;
                                errorFlag = 1;
                            }
                            else 
                                $$ = $1;;
                        }
    | exp SUB term      {
                            // check for type mismatch
                            if($1 != $3)
                            {
                                // check if mismatch not reported already
                                if($3 != NotDefined && $1 != NotDefined)
                                    cout << "Line " << yylineno << " : semantic error, type mismatch in expression." << endl;
                                $$ = NotDefined;
                                errorFlag = 1;
                            }
                            else 
                                $$ = $1;
                        }
    ;

term : factor           {
                            $$ = $1;
                        }
    | term MUL factor   {
                            // check for type mismatch
                            if($1 != $3)
                            {
                                // check if mismatch not reported already
                                if($3 != NotDefined && $1 != NotDefined)
                                    cout << "Line " << yylineno << " : semantic error, type mismatch in expression." << endl;
                                $$ = NotDefined;
                                errorFlag = 1;
                            }
                            else 
                                $$ = $1;
                        }
    | term DIV factor   {
                            // check for type mismatch
                            if($1 != $3)
                            {
                                // check if mismatch not reported already
                                if($3 != NotDefined && $1 != NotDefined)
                                    cout << "Line " << yylineno << " : semantic error, type mismatch in expression." << endl;
                                $$ = NotDefined;
                                errorFlag = 1;
                            }
                            else 
                                $$ = $1;
                        }
    ;

factor : ID                         {
                                        $$ = getType($1);
                                        // if the symbol is not defined
                                        if($$ == NotDefined)
                                        {
                                            cout << "Line " << yylineno << " : semantic error, symbol "<<$1<<" not declared." << endl;
                                            errorFlag = 1;
                                        }
                                        else if(!isInitialized($1))
                                        {
                                            cout << "Line " << yylineno << " : semantic error, symbol "<< $1 << " not initialized." << endl;
                                            errorFlag = 1;
                                        }
                                    }
    | INT                           {
                                        $$ = IntegerNum;
                                        
                                    }
    | REAL_NUM                      {
                                        $$ = RealNum;
                                    }
    | OPENBRACKET exp CLOSEBRACKET  {
                                        $$ = $2;
                                    }
    ;

read : READ OPENBRACKET idlist CLOSEBRACKET     {
                                                    // check if all ids are valid
                                                    if(verifyIds($3) < 0)
                                                    {
                                                        cout << "Line " << yylineno << " : semantic error, symbol not declared." << endl;
                                                        errorFlag = 1;
                                                    }
                                                    else
                                                    {
                                                        update($3);
                                                    }
                                                }
    ;

write : WRITE OPENBRACKET idlist CLOSEBRACKET   {
                                                    // check if all ids are valid
                                                    if(verifyIds($3) <  0)
                                                    {
                                                        cout << "Line " << yylineno << " : semantic error, symbol not declared." << endl;
                                                        errorFlag = 1;
                                                    }
                                                }
    ;

for : FOR indexexp DO body
    | error
    ;

indexexp : ID EQUAL exp TO exp      {
                                        // check if id not declared
                                        if(getType($1) == NotDefined)
                                        {
                                            cout << "Line " << yylineno << " : semantic error, symbol not declared in for loop index expression." << endl;
                                            errorFlag = 1;
                                        }
                                        // check for type mismatch
                                        else if(($3 != $5 || getType($1) != $3 || getType($1) != $5) && $3 != NotDefined && $5 != NotDefined)
                                        {
                                            cout << "Line " << yylineno << " : semantic error, type mismatch in for loop index expression." << endl;
                                            errorFlag = 1;
                                        }
                                        else
                                        {
                                            update($1);
                                        }
                                    }
    ;

body : stmt 
    | BEGIN1 stmtlist END
    ;

%% 

// called when a syntax error is encountered
void yyerror (const char *s) 
{
    cout << "Line " << yylineno << " : " << s << endl;
    errorFlag = 1;
} 

int main (void) 
{
    yyparse ();
    
    // check if no error found
    if(errorFlag == 0)
    {
        cout << "Program  Successfull." << endl;
    }

    // print all the symbols
    printIdTable();

    return 0;
}
