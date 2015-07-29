%{

#include <stdio.h>
#include <stdlib.h>
#include "functions.h"
#include "semantic.h"
#include "symbol_table.h"


int line=1;
int col=1;
char* yytext;
extern int yylex();
extern void yyerror();


Node *my_program;
environment_list *env;

%}

%union {
	int i;
	char *s;
	Node *node;
}


%token ASSIGN BEGIN2 COLON COMMA DO DOT ELSE END FORWARD FUNCTION IF 
%token LBRAC NOT OUTPUT PARAMSTR PROGRAM RBRAC REPEAT SEMIC THEN UNTIL VAL VAR WHILE 
%token WRITELN LT OR AND GT EQ NEQ LEQ GEQ ADD SUB MUL REALDIV MOD DIV PLUS MINUS CALL
%token <s> INTLIT
%token <s> REALLIT
%token <s> ID
%token <s> STRING
%token <s> RESERVED

%type <node> Start Prog ProgHeading ProgBlock VarPart VarPartPrime VarDeclaration IDList 
%type <node> IDListPrime FuncPart FuncDeclaration FuncHeading FuncIdent FormalParamList 
%type <node> FormalParamListPrime FormalParams FuncBlock StatPart CompStat StatList 
%type <node> Stat WritePList WritePListPrime WritePListDPrime ParamList ParamListPrime
%type <node> Expr SimpleExpr Term Factor


%nonassoc IFX
%nonassoc ELSE
%nonassoc LT GT EQ NEQ LEQ GEQ 
%left ADD SUB OR
%left MUL DIV REALDIV MOD AND
%nonassoc NOT


%%

Start: 	Prog {$$=$1;my_program=$$;}
	;

Prog:	ProgHeading SEMIC ProgBlock DOT {$$=insert_double_child(Program,$1,$3,0,0);}
	;

ProgHeading:	PROGRAM ID LBRAC OUTPUT RBRAC {$$=insert_char(Id,$2,@2.first_line,@2.first_column);}
			;

ProgBlock:	VarPart FuncPart StatPart {
				Node *VP;
				Node *FP;
				Node *SP;
				if($1==NULL){
					VP=make_node(VarPart);
				}
				else{
					VP=$1;
				}
				if($2==NULL){
					FP=make_node(FuncPart);
				} 
				else{
					FP=insert_single_child(FuncPart,$2,0,0);
				}
				insert_list(FP,$3);
				$$=insert_list(VP,FP); 
				
			}
		;

VarPart:	VAR VarDeclaration SEMIC VarPartPrime {$$=insert_double_child(VarPart,$2,$4,0,0);}
		|	{ $$ = NULL;}
		; 

VarPartPrime:	VarPartPrime VarDeclaration SEMIC {$$=insert_list($1,$2);}
			|	{ $$ = NULL;}
			;

VarDeclaration:	IDList COLON ID {$$=insert_double_child(VarDecl,$1,insert_char(Id,$3,@3.first_line,@3.first_column),0,0);}
			;
	
IDList:	ID IDListPrime {$$=insert_list(insert_char(Id,$1,@1.first_line,@1.first_column),$2);}
	;

IDListPrime:	IDListPrime COMMA ID {$$=insert_list($1,insert_char(Id,$3,@3.first_line,@3.first_column));}
			|	{ $$ = NULL;}
			;

FuncPart:	FuncPart FuncDeclaration SEMIC {
				$$=insert_list($1,$2);
			}
		|	{ $$ = NULL;}
		;

FuncDeclaration:	FuncHeading SEMIC FORWARD {$$=insert_single_child(FuncDecl,$1,0,0);}
				|	FuncIdent SEMIC FuncBlock {$$=insert_double_child(FuncDef2,$1,$3,0,0);}
				|	FuncHeading SEMIC FuncBlock {$$=insert_double_child(FuncDef,$1,$3,0,0);}
				;

FuncHeading:	FUNCTION ID FormalParamList COLON ID {
					Node *FP;
					if($3==NULL){
						FP=make_node(FuncParams);
					} 
					else{
						$$=insert_list(insert_char(Id,$2,@2.first_line,@2.first_column),$3);
						insert_list($3,insert_char(Id,$5,@5.first_line,@5.first_column));
					}
				}
			|	FUNCTION ID COLON ID {Node *aux=make_node(FuncParams);
				$$=insert_list(insert_char(Id,$2,@2.first_line,@2.first_column),aux);
				insert_list(aux,insert_char(Id,$4,@4.first_line,@4.first_column));}
			;

FuncIdent:	FUNCTION ID {$$=insert_char(Id,$2,@2.first_line,@2.first_column);}
		;

FormalParamList:	LBRAC FormalParams FormalParamListPrime RBRAC {$$=insert_double_child(FuncParams,$2,$3,0,0);}
				;

FormalParamListPrime:	FormalParamListPrime SEMIC FormalParams {$$=insert_list($1,$3);}
					|	{ $$ = NULL;}
					;

FormalParams:	VAR IDList COLON ID {$$=insert_double_child(VarParams,$2,insert_char(Id,$4,@4.first_line,@4.first_column),0,0);}
			|	IDList COLON ID {$$=insert_double_child(Params,$1,insert_char(Id,$3,@3.first_line,@3.first_column),0,0);}
			;

FuncBlock:	VarPart StatPart {
				
				Node *VP;
				if($1==NULL){
					VP=make_node(VarPart);
				}
				else{
					VP=$1;
				}
				$$=insert_list(VP,$2);
			}
		;


StatPart:	CompStat {
				if($1==NULL){
					$$=make_node(StatList);
				}
				else{
					$$=$1;
				}
			}				
		;

CompStat:	BEGIN2 StatList END {
				
				if($2==NULL){
					$$=$2;
				}
				else if($2->next!=NULL){
					$$=insert_single_child(StatList,$2,@1.first_line,@1.first_column);	
				}else{
					$$=$2;
				}
			}
		;

StatList:	StatList SEMIC Stat {
				if($1==NULL){
					$$=$3;
				}else if($3==NULL){
					$$=$1;
				}else{
					$$=insert_list($1,$3);
				}
			}
		|	Stat { $$=$1; };
		;


Stat:	CompStat {
				$$=$1;
		}
	|	IF Expr THEN Stat %prec IFX{
		Node *stat;
		if($4==NULL){
			stat=make_node(StatList);
		}
		else{
			stat=$4;
		}
		$$=insert_triple_child(IfElse,$2,stat,make_node(StatList),@1.first_line,@1.first_column);
	}
	
	|	IF Expr THEN Stat ELSE Stat {
			Node *stat1;
			Node *stat2;
			if($4==NULL){
				stat1=make_node(StatList);
			}
			else{
				stat1=$4;
			}
			if($6==NULL){
				stat2=make_node(StatList);
			}
			else{
				stat2=$6;
			}
			$$=insert_triple_child(IfElse,$2,stat1,stat2,@1.first_line,@1.first_column);
	}

	|	WHILE Expr DO Stat {
			Node *stat;
			if($4==NULL){
				stat=make_node(StatList);
			}
			else{
				stat=$4;
			}
			$$=insert_double_child(While,$2,stat,@1.first_line,@1.first_column);
		}
	
	|	REPEAT StatList UNTIL Expr {

			Node *stat;
			if($2==NULL){
				stat=make_node(StatList);
			}
			else if($2->next!=NULL){
					stat=insert_single_child(StatList,$2,@2.first_line,@2.first_column);	
			}else{
					stat=$2;
			}
			
			$$=insert_double_child(Repeat,stat,$4,@1.first_line,@1.first_column);
		}
	
	|	VAL LBRAC PARAMSTR LBRAC Expr RBRAC COMMA ID RBRAC {$$=insert_double_child(ValParam,$5,insert_char(Id,$8,@8.first_line,@8.first_column),0,0);}
	
	|	ID ASSIGN Expr {$$=insert_double_child(Assign,insert_char(Id,$1,@1.first_line,@1.first_column),$3,@2.first_line,@2.first_column);}
	
	|	WRITELN {$$=make_node(WriteLn);}
	
	|	WRITELN WritePList {$$=insert_single_child(WriteLn,$2,@1.first_line,@1.first_column);}
	
	|	{ $$ = NULL;}
	;

WritePList:	LBRAC WritePListPrime WritePListDPrime RBRAC {
				$$=insert_list($2,$3);
				
			}
		;

WritePListPrime:	Expr {$$=$1;}
				|	STRING {$$=insert_char(String,$1,@1.first_line,@1.first_column);}
				;

WritePListDPrime:	WritePListDPrime COMMA WritePListPrime {
							$$=insert_list($1,$3);
					}
				|	{ $$ = NULL;}
				;


Expr:	SimpleExpr {$$=$1;}	
	|	SimpleExpr LT SimpleExpr {$$=insert_double_child(Lt,$1,$3,@2.first_line,@2.first_column);}
	|	SimpleExpr LEQ SimpleExpr {$$=insert_double_child(Leq,$1,$3,@2.first_line,@2.first_column);}
	|	SimpleExpr GT SimpleExpr {$$=insert_double_child(Gt,$1,$3,@2.first_line,@2.first_column);}
	|	SimpleExpr GEQ SimpleExpr {$$=insert_double_child(Geq,$1,$3,@2.first_line,@2.first_column);}
	|	SimpleExpr EQ SimpleExpr {$$=insert_double_child(Eq,$1,$3,@2.first_line,@2.first_column);}
	|	SimpleExpr NEQ SimpleExpr {$$=insert_double_child(Neq,$1,$3,@2.first_line,@2.first_column);}
	;


SimpleExpr: Term {$$=$1;}
		|	SimpleExpr ADD Term {$$=insert_double_child(Add,$1,$3,@2.first_line,@2.first_column);}
		|	SimpleExpr SUB Term {$$=insert_double_child(Sub,$1,$3,@2.first_line,@2.first_column);}
		|	SimpleExpr OR Term {$$=insert_double_child(Or,$1,$3,@2.first_line,@2.first_column);}
		|	ADD Term {$$=insert_single_child(Plus,$2,@1.first_line,@1.first_column);}
		|	SUB Term {$$=insert_single_child(Minus,$2,@1.first_line,@1.first_column);}
		;


Term:	Factor {$$=$1;}
	|	Term MUL Term {$$=insert_double_child(Mul,$1,$3,@2.first_line,@2.first_column);}
	|	Term REALDIV Term {$$=insert_double_child(RealDiv,$1,$3,@2.first_line,@2.first_column);}
	|	Term DIV Term {$$=insert_double_child(Div,$1,$3,@2.first_line,@2.first_column);}
	|	Term MOD Term {$$=insert_double_child(Mod,$1,$3,@2.first_line,@2.first_column);}
	|	Term AND Term {$$=insert_double_child(And,$1,$3,@2.first_line,@2.first_column);}
	;


Factor:	NOT	Factor {$$=insert_single_child(Not,$2,@1.first_line,@1.first_column);}
	|	LBRAC Expr RBRAC {$$=$2;}
	|	INTLIT {$$=insert_char(IntLit,$1,@1.first_line,@1.first_column);}
	|	REALLIT {$$=insert_char(RealLit,$1,@1.first_line,@1.first_column);}
	|	ID {$$=insert_char(Id,$1,@1.first_line,@1.first_column);}
	|	ID ParamList {$$=insert_double_child(Call,insert_char(Id,$1,@1.first_line,@1.first_column),$2,0,0);}
	;

ParamList:	LBRAC Expr ParamListPrime RBRAC {$$=insert_list($2,$3);}
		;

ParamListPrime:	ParamListPrime COMMA SimpleExpr {$$=insert_list($1,$3);}
			|	{ $$ = NULL;}
			;

%%
int main(int argc, char** argv){
	int i;
	if(yyparse()==0){
		for(i=0; i< argc; i++){
			if (strcmp(argv[i],"-t")==0){
				print_node(my_program, 0);	
			}
			if (strcmp(argv[i],"-s")==0){	
				env = semantic_analysis(my_program);
				
				print_semantic(env);
				
			}
		}
	}
	return 0;
}
