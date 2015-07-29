#ifndef _FUNCTIONS_
#define _FUNCTIONS_
typedef enum{
	Program,VarPart,FuncPart,VarDecl,FuncDecl,FuncDef,FuncDef2,FuncParams,Params,
	VarParams,Assign,IfElse,Repeat,StatList,ValParam,While,WriteLn,Add,And,Call,Div,Eq,
	Geq,Gt,Leq,Lt,Minus,Mod,Mul,Neq,Not,Or,Plus,RealDiv,Sub,Id,IntLit,RealLit,String
} node_type;

typedef enum{
	_boolean_, _integer_, _real_, _function_, _program_, _type_, _false_ , _true_ , _NULL_
} te_type;

typedef struct Node{
	node_type type;
	int line;
	int col;
 	union{
 		int i;
 		char *s;
 	} value;
 	struct Node *next, *childs;
} Node;


Node *insert_single_child(node_type type, char *s, Node *n, int line, int col);

Node *insert_double_child(node_type type, char *s, Node *n1, Node *n2, int line, int col);

Node *insert_triple_child(node_type type, char *s, Node *n1, Node *n2, Node *n3, int line, int col);

Node *insert_int(node_type type, int i);

Node *insert_char(node_type type, char *s,int line,int col);

Node *insert_list(Node *list, Node *node);

void print_node(Node *node, int ident); 

Node *make_node(node_type type);

#endif