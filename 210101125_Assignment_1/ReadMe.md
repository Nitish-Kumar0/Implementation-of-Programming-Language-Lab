Name : Nitish Kumar Pinneti
Roll Number : 210101125

In my Assignment 1 there are 2 main files SIC_Assembler.h and main.cpp.

main.cpp code uses the class Assembler and takes the input file from user.
SIC_Assembler.h contains the class Assembler which contains functions for Pass1 and Pass2.
i have made classes for storing serching and retrival for opcodes and symbols in the form of OpCodeTable and SymbolTable.
i have added 5 txt files as input "copy.txt", "count.txt", "sum.txt", "program.txt"and "program1.txt".
there are 2 files generated in the process "output.txt"(by pass 2) and "intermediate.txt"(by pass 1).
Each pass returns if success or not even if pass 2 is not succesfull it will give some output

Limitations:
    The Input must be seperated by a tab('\t') between each part of the sic instruction
    ex: "SUM	START	4000"  not  "SUM    START   4000"
    in vs code the default even when pressed tab spaces will appear to change this:

    To convert existing indentation from spaces to tabs(\t) hit Ctrl+Shift+P and type:
    >Convert indentation to Tabs
    This will change the indentation for your document based on the defined settings to Tabs.

    there must be a word tab word tab word this is the formate (word can be empty).
    there must not be more than 3 worder if present it considers first 3


