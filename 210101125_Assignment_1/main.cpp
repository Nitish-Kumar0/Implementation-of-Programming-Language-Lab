#include "SIC_Assembler.h"
#include<iostream>
using namespace std;

int main()
{
	string filename;
    cout<<"PLEASE ENTER FILE NAME\n";
    cin>>filename;
	Assembler A(filename);
	bool success1 = A.pass1();
    bool success2;
    if(success1)
    {
	    success2 = A.pass2();
    }
    else
    {
        cout<<"\nPass 1 Failed\n";
    }
    if(!success2)
    {
        cout<<"\nPass 2 Failed\n";
    }
    A.print_stats();

	return 0;
}