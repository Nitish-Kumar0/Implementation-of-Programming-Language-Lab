Name : Nitish Kumar Pinneti
Roll No : 210101125


Assembler Readme

Description:
This is an assembler program written in C++ that reads assembly code, processes it, and generates object code.
The assembler consists of two passes:

Pass 1: In this pass, the assembler reads the input assembly file, generates an intermediate file, 
        and builds the symbol table. It also calculates the program length and records the section lengths.

Pass 2: In this pass, the assembler reads the intermediate file generated in pass 1, processes it, and generates the 
        object code. It handles instructions, literals, symbols, and various addressing modes.

File Structure:

main.cpp (assembler.h): The main source file containing the implementation of the assembler.
Input assembly code file to be assembled taken as input from user.
intermediate.txt: Intermediate file generated during pass 1.
output.txt: Final object code output file generated during pass 2.

Usage:
Compile the main.cpp file using a C++ compiler.
g++ -o assembler main.cpp

Run the compiled executable.
./assembler

Enter the name of the input assembly file when prompted.

Enter File Name : input.txt
The assembler will perform pass 1 and pass 2, generating the intermediate file (intermediate.txt)
and the object code output file (output.txt).

start with . for commenting.


Loader Program

Overview
This is a C++ program designed to load programs into memory by processing input files containing program information, including header 
records, text records, and modification records. 

The program operates in two passes:
Pass One: Reads the input file, processes header records, and builds an External Symbol Table (ESTAB) containing external symbols 
          and their addresses.
Pass Two: Reads the input file again, processes text and modification records, loads control sections into memory, and writes memory 
          contents to an output file.

Usage:

Compilation: Compile the program using a C++ compiler.

            g++ main.cpp -o loader
Execution: Run the compiled executable and follow the prompts to enter the input file name.

            ./loader

Input : The program takes the input file and programm addresses from user.            
Output: The program generates an output file (loader_output.txt) containing the loaded memory contents.

Input File Format
The input file is expected to follow a specific format, consisting of header records, text records, and modification records. 
Each record type is identified by a prefix character ('H', 'T', 'M', 'D', 'E') followed by relevant information.