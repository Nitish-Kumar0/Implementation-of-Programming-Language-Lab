%{
    #include "y.tab.c"
%}

alpha   [a-zA-z]
digit   [0-9]
number  {digit}+
id      {alpha}({alpha}|{digit})*
real    {digit}+(\.{digit}+)?(E[+-]?{digit}+)?

%option yylineno

%%

[ \t]       {
                // Do nothing
                // Ignore whitespaces
            }

"\n"        {
                // indicate start of new line
                lineNo ++;
                newLine = 1;
            }

"PROGRAM"   {
                return PROGRAM;
            }

"VAR"       {
                return VAR;
            }

"BEGIN"     {
                return BEGIN1;
            }

"END"       {
                return END;
            }
            
"END."      {
                return END_DOT;
            }
            
"INTEGER"   {
                return INTEGER;
            }

"REAL"      {
                return REAL;
            }
            
"FOR"       {
                return FOR;
            }
            
"READ"      {
                return READ;
            }
            
"WRITE"     {
                return WRITE;
            }
            
"TO"        {
                return TO;
            }
            
"DO"        {
                return DO;
            }
            
";"         {
                return SEMICOLON;
            }
            
":"         {
                return COLON;
            }
            
","         {
                return COMMA;
            }
            
":="        {
                return EQUAL;
            }
            
"+"         {
                return ADD;
            }
            
"-"         {
                return SUB;
            }
            
"*"         {
                return MUL;
            }
            
"DIV"       {
                return DIV;
            }
            
"("         {
                return OPENBRACKET;
            }
            
")"         {
                return CLOSEBRACKET;
            }
            
{id}        {
                string id = yytext;
                strcpy(yylval.str, yytext);
                return ID;
            }
            
{number}    {
                string number = yytext;
                yylval.int_num = atoi(yytext);
                InsertInt(number);
                return INT;
            }

{real}      {
                string real = yytext;
                yylval.real_num = atof(yytext);
                InsertReal(real);
                return REAL_NUM;                
            }
.           {
                return CHARACTER;
            }
            
%%

// called by lex when input is exhausted
int yywrap(void) 
{
    return 1;
}
