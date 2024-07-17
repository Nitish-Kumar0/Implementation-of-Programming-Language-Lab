#include "assembler.h"

int main()
{
    string inputfile;
    cout<<"Enter File Name : ";
    cin>>inputfile;
    Assembler A(inputfile);
	if(!A.pass1())
    {
        cout << "PASS ONE Failed.\n" << endl;
        return 0;
    }
    if(!A.pass2())
    {
        cout << "PASS TWO Failed.\n" << endl;
    }
    
}