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

/* handle locations */
#define YY_USER_ACTION yylloc.first_line = yylloc.last_line = line; \
    yylloc.first_column = col; yylloc.last_column = col+yyleng-1; \
    col += yyleng;
	
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

"(*"|"{"						{ comment_col=col; comment_line=line; BEGIN (COMMENT);}
<COMMENT><<EOF>>				{ printf("Line %d, col %d: unterminated comment\n",comment_line,comment_col); col++; return 0;}
<COMMENT>"\n"					{ line++; col=1;}
<COMMENT>"*)"|"}"				{ BEGIN (INITIAL);}
<COMMENT>.						{ }


{STRING}						{ yylval.s=(char*)strdup(yytext); return STRING;}					
{NSTRING}						{ printf("Line %d, col %d: unterminated string\n",line,col);}
{RESERVED}						{ yylval.s=(char*)strdup(yytext); return RESERVED;}
{INTLIT}						{ yylval.s=(char*)strdup(yytext); return INTLIT;}		
{REALLIT}						{ yylval.s=(char*)strdup(yytext); return REALLIT;}
":="							{ return ASSIGN;}
"begin"							{ yylval.s=(char*)strdup(yytext); return BEGIN2;}
":"								{ return COLON;}
","								{ return COMMA;}
"do"							{ yylval.s=(char*)strdup(yytext); return DO;}
"."								{ return DOT;}
"else"							{ yylval.s=(char*)strdup(yytext); return ELSE;}
"end"							{ yylval.s=(char*)strdup(yytext); return END;}
"forward"						{ yylval.s=(char*)strdup(yytext); return FORWARD;}
"function"						{ yylval.s=(char*)strdup(yytext); return FUNCTION;}
"if"							{ yylval.s=(char*)strdup(yytext); return IF;}
"("								{ return LBRAC;}
"not"							{ yylval.s=(char*)strdup(yytext); return NOT;}
"output"						{ yylval.s=(char*)strdup(yytext); return OUTPUT;}
"paramstr"						{ yylval.s=(char*)strdup(yytext); return PARAMSTR;}
"program"						{ yylval.s=(char*)strdup(yytext); return PROGRAM;}
")"								{ return RBRAC;}
"repeat"						{ yylval.s=(char*)strdup(yytext); return REPEAT;}
";"								{ return SEMIC;}
"then"							{ yylval.s=(char*)strdup(yytext); return THEN;}
"until"							{ yylval.s=(char*)strdup(yytext); return UNTIL;}
"val"							{ yylval.s=(char*)strdup(yytext); return VAL;}
"var"							{ yylval.s=(char*)strdup(yytext); return VAR;}
"while"							{ yylval.s=(char*)strdup(yytext); return WHILE;}
"writeln"						{ yylval.s=(char*)strdup(yytext); return WRITELN;}
"<"								{ return LT;}
"or"							{ yylval.s=(char*)strdup(yytext); return OR;}
"and"							{ yylval.s=(char*)strdup(yytext); return AND;}
">"								{ return GT;}
"="								{ return EQ;}
"<>"							{ return NEQ;}
"<="							{ return LEQ;}
">="							{ return GEQ;}
"+"								{ return ADD;}
"-"								{ return SUB;}
"*"								{ return MUL;}
"/"								{ return REALDIV;}
"mod"							{ yylval.s=(char*)strdup(yytext); return MOD;}
"div"							{ yylval.s=(char*)strdup(yytext); return DIV;}
" "|"\t"						{ }
"\n"							{ line++;col=1;}
{ID}							{ yylval.s=(char*)strdup(yytext); return ID;}
.								{ printf("Line %d, col %d: illegal character ('%s')\n",line,col,yytext); }
<<EOF>>							{ col++; yyterminate(); return 0;}
%%

void yyerror (char *s) {
	printf ("Line %d, col %d: %s: %s\n", line, col-yyleng, s, yytext);
}

int yywrap(){
	return 1;
}

