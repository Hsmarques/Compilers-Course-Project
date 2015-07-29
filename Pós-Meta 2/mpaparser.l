%{

#include "functions.h"
#include <stdlib.h>
#include <string.h>
#include "y.tab.h"


int yywrap();

extern char* yytext;
extern int line;
extern int col;

int comment_col=1;
int comment_line=1;

	
%}

%option case-insensitive

ID					[a-z]([a-z0-9])*
INTLIT				[0-9]+
REALLIT 			([0-9]+"."[0-9]+)("e"([+-])?[0-9]+)?|[0-9]+("e"([+-])?[0-9]+)?
RESERVED 			abs|arctan|array|case|char|chr|const|cos|dispose|downto|eof|eoln|exp|file|for|get|goto|in|input|label|ln|maxint|new|nil|odd|of|ord|pack|packed|page|pred|procedure|put|read|readln|record|reset|rewrite|round|set|sin|sqr|sqrt|succ|text|to|trunc|type|unpack|with|write
STRING 				'([^'\n]|'')*'
NSTRING				'([^'\n]|'')*

%X COMMENT

%%

"(*"|"{"						{comment_col=col; comment_line=line; col+=yyleng; BEGIN (COMMENT);}
<COMMENT><<EOF>>				{printf("Line %d, col %d: unterminated comment\n",comment_line,comment_col); col++; return 0;}
<COMMENT>"\n"					{line++; col=1;}
<COMMENT>"*)"|"}"				{col+=yyleng; BEGIN (INITIAL);}
<COMMENT>.						{col+=yyleng;}


{STRING}						{col+=yyleng; yylval.s=(char*)strdup(yytext); return STRING;}					
{NSTRING}						{printf("Line %d, col %d: unterminated string\n",line,col);}
{RESERVED}						{col+=yyleng; yylval.s=(char*)strdup(yytext); return RESERVED;}
{INTLIT}						{col+=yyleng; yylval.s=(char*)strdup(yytext); return INTLIT;}		
{REALLIT}						{col+=yyleng; yylval.s=(char*)strdup(yytext); return REALLIT;}
":="							{col+=yyleng; return ASSIGN;}
"begin"							{col+=yyleng; return BEGIN2;}
":"								{col+=yyleng; return COLON;}
","								{col+=yyleng; return COMMA;}
"do"							{col+=yyleng; return DO;}
"."								{col+=yyleng; return DOT;}
"else"							{col+=yyleng; return ELSE;}
"end"							{col+=yyleng; return END;}
"forward"						{col+=yyleng; return FORWARD;}
"function"						{col+=yyleng; return FUNCTION;}
"if"							{col+=yyleng; return IF;}
"("								{col+=yyleng; return LBRAC;}
"not"							{col+=yyleng; return NOT;}
"output"						{col+=yyleng; return OUTPUT;}
"paramstr"						{col+=yyleng; return PARAMSTR;}
"program"						{col+=yyleng; return PROGRAM;}
")"								{col+=yyleng; return RBRAC;}
"repeat"						{col+=yyleng; return REPEAT;}
";"								{col+=yyleng; return SEMIC;}
"then"							{col+=yyleng; return THEN;}
"until"							{col+=yyleng; return UNTIL;}
"val"							{col+=yyleng; return VAL;}
"var"							{col+=yyleng; return VAR;}
"while"							{col+=yyleng; return WHILE;}
"writeln"						{col+=yyleng; return WRITELN;}
"<"								{col+=yyleng; return LT;}
"or"							{col+=yyleng; return OR;}
"and"							{col+=yyleng; return AND;}
">"								{col+=yyleng; return GT;}
"="								{col+=yyleng; return EQ;}
"<>"							{col+=yyleng; return NEQ;}
"<="							{col+=yyleng; return LEQ;}
">="							{col+=yyleng; return GEQ;}
"+"								{col+=yyleng; return ADD;}
"-"								{col+=yyleng; return SUB;}
"*"								{col+=yyleng; return MUL;}
"/"								{col+=yyleng; return REALDIV;}
"mod"							{col+=yyleng; return MOD;}
"div"							{col+=yyleng; return DIV;}
" "|"\t"						{col+=yyleng;}
"\n"							{line++;col=1;}
{ID}							{col+=yyleng; yylval.s=(char*)strdup(yytext); return ID;}
.								{printf("Line %d, col %d: illegal character ('%s')\n",line,col,yytext); col+=yyleng;}
<<EOF>>							{col++; yyterminate(); return 0;}
%%

void yyerror (char *s) {
	printf ("Line %d, col %d: %s: %s\n", line, col-yyleng, s, yytext);
}

int yywrap(){
	return 1;
}

