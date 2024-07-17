Assignment 3

Name : Nitish Kumar Pinneti
Roll No : 210101125

Environment used to run the program:
Ubuntu 22

Installation:
sudo apt-get install bison flex

lex lexical_analyser.l
g++ lex.yy.c
./a.out < input.pas

Files
lexical_analyser.l: This is the main Flex file containing the rules for tokenizing input text. 
                    It defines tokens such as keywords (PROGRAM, VAR, BEGIN, etc.), identifiers,
                    numbers, and various symbols.

helper.cpp: This file contains helper functions and classes used by the lexer program. It implements a 
            hash table data structure for managing symbol tables (idTable and intTable). 
            The printTokenInfo function is responsible for printing token information, while install_id and install_num 
            are used to install identifiers and numbers into their respective symbol tables.

Error Handling
The lexer program detects and handles invalid tokens encountered in the input text by printing an error message to the console.

Symbol Table Management
The symbol tables (idTable and intTable) are implemented as hash tables in helper.cpp.
Each table uses separate instances of the HashTable class with a predefined maximum hash size (15).
The insert and search functions in HashTable are used to manage the insertion and retrieval of identifiers and numbers.
Errors related to duplicate identifiers or unknown identifiers (after the BEGIN keyword) are handled within the install_id function.