%{

int line=1;
int col=1;
int comment_col=1;
int comment_line=1;
	
%}

%option case-insensitive

ID					[a-z]([a-z0-9])*
INTLIT				[0-9]+
REALLIT 			([0-9]+"."[0-9]+)("e"([+-])?[0-9]+)?|[0-9]+("e"([+-])?[0-9]+)?
OP1 				"and"|"or"
OP2 				"<"|">"|"="|"<>"|"<="|">="
OP3 				"+"|"-"
OP4 				"*"|"/"|"mod"|"div"
RESERVED 			abs|arctan|array|case|char|chr|const|cos|dispose|downto|eof|eoln|exp|file|for|get|goto|in|input|label|ln|maxint|new|nil|odd|of|ord|pack|packed|page|pred|procedure|put|read|readln|record|reset|rewrite|round|set|sin|sqr|sqrt|succ|text|to|trunc|type|unpack|with|write
STRING 				'([^'\n]|'')*'
NSTRING				'([^'\n]|'')*

%X COMMENT

%%

"(*"|"{"						{comment_col=col; comment_line=line; col+=yyleng; BEGIN (COMMENT);}
<COMMENT><<EOF>>				{printf("Line %d, col %d: unterminated comment\n",comment_line,comment_col); BEGIN (INITIAL);}
<COMMENT>"\n"					{line++; col=1;}
<COMMENT>"*)"|"}"				{col+=yyleng; BEGIN (INITIAL);}
<COMMENT>.						{col+=yyleng;}


{STRING}						{col+=yyleng; printf("STRING(%s)\n",yytext);}					
{NSTRING}						{printf("Line %d, col %d: unterminated string\n",line,col);}
{RESERVED}						{col+=yyleng; printf("RESERVED(%s)\n",yytext);}
{INTLIT}						{col+=yyleng; printf("INTLIT(%s)\n",yytext);}		
{REALLIT}						{col+=yyleng; printf("REALLIT(%s)\n",yytext);}
":="							{col+=yyleng; printf("ASSIGN\n");}
"begin"							{col+=yyleng; printf("BEGIN\n");}
":"								{col+=yyleng; printf("COLON\n");}
","								{col+=yyleng; printf("COMMA\n");}
"do"							{col+=yyleng; printf("DO\n");}
"."								{col+=yyleng; printf("DOT\n");}
"else"							{col+=yyleng; printf("ELSE\n");}
"end"							{col+=yyleng; printf("END\n");}
"forward"						{col+=yyleng; printf("FORWARD\n");}
"function"						{col+=yyleng; printf("FUNCTION\n");}
"if"							{col+=yyleng; printf("IF\n");}
"("								{col+=yyleng; printf("LBRAC\n");}
"not"							{col+=yyleng; printf("NOT\n");}
"output"						{col+=yyleng; printf("OUTPUT\n");}
"paramstr"						{col+=yyleng; printf("PARAMSTR\n");}
"program"						{col+=yyleng; printf("PROGRAM\n");}
")"								{col+=yyleng; printf("RBRAC\n");}
"repeat"						{col+=yyleng; printf("REPEAT\n");}
";"								{col+=yyleng; printf("SEMIC\n");}
"then"							{col+=yyleng; printf("THEN\n");}
"until"							{col+=yyleng; printf("UNTIL\n");}
"val"							{col+=yyleng; printf("VAL\n");}
"var"							{col+=yyleng; printf("VAR\n");}
"while"							{col+=yyleng; printf("WHILE\n");}
"writeln"						{col+=yyleng; printf("WRITELN\n");}
{OP1}							{col+=yyleng; printf("OP1(%s)\n",yytext);}
{OP2}							{col+=yyleng; printf("OP2(%s)\n",yytext);}
{OP3}							{col+=yyleng; printf("OP3(%s)\n",yytext);}
{OP4}							{col+=yyleng; printf("OP4(%s)\n",yytext);}

" "|"\t"						{col+=yyleng;}
"\n"							{line++;col=1;}


{ID}							{col+=yyleng; printf("ID(%s)\n",yytext);}

.								{printf("Line %d, col %d: illegal character ('%s')\n",line,col,yytext); col+=yyleng;}

%%
int main(){
	yylex();
	return 0;
}

int yywrap(){
	return 1;
}

