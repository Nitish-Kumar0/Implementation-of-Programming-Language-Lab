%{
#include "helper.cpp"
%}

alpha   [a-zA-Z]
digit   [0-9]
number  {digit}+(\.{digit}+)?(E[+-]?{digit}+)?
id      {alpha}({alpha}|{digit})*

%%
[ \t]       {}

"\n"        {
                lineNumber++;
                newLineFlag = 1;
            }
"\r"        { /* Ignore carriage return */ }
"PROGRAM"   { printTokenInfo(1); }
"VAR"       { printTokenInfo(2); }
"BEGIN"     { 
                printTokenInfo(3); 
                afterBegin=true;
            }
"END"       { printTokenInfo(4); }
"END."      { printTokenInfo(5); }
"INTEGER"   { printTokenInfo(6); }
"FOR"       { printTokenInfo(7); }
"READ"      { printTokenInfo(8); }
"WRITE"     { printTokenInfo(9); }
"TO"        { printTokenInfo(10); }
"DO"        { printTokenInfo(11); }
";"         { printTokenInfo(12); }
":"         { printTokenInfo(13); }
","         { printTokenInfo(14); }
":="        { printTokenInfo(15); }
"+"         { printTokenInfo(16); }
"-"         { printTokenInfo(17); }
"*"         { printTokenInfo(18); }
"DIV"       { printTokenInfo(19); }
"("         { printTokenInfo(20); }
")"         { printTokenInfo(21); }
{id}        {
                string id = yytext;
                printTokenInfo(22, id);
                install_id(id);
            }
{number}    {
                string number = yytext;
                printTokenInfo(23, number);
                install_num(number);
            }
.           { 
                string invalidToken = yytext;
                cout << "Error: Invalid token encountered at line '" << lineNumber<< "\n";
                // Handle or ignore the invalid token as needed
            }
            
%%

int yywrap(void) {
    return 1;
}

int main() {
    fout.open(outputFile);
    fout << "Line\tToken type\tToken Specifier\n";
    yylex();
    cout<<"Id Table :\n";
    idTable.printTable();
    cout<<"\nInt Table :\n";
    intTable.printTable();
    cout << "Check output file: " << outputFile << "\n" ;
    fout.close();
    return 0;
}
