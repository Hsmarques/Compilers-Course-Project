
typedef enum{
	Program,VarPart,FuncPart,VarDecl,FuncDecl,FuncDef,FuncDef2,FuncParams,Params,
	VarParams,Assign,IfElse,Repeat,StatList,ValParam,While,WriteLn,Add,And,Call,Div,Eq,
	Geq,Gt,Leq,Lt,Minus,Mod,Mul,Neq,Not,Or,Plus,RealDiv,Sub,Id,IntLit,RealLit,String
} node_type;

typedef struct Node{
	node_type type;
 	union{
 		int i;
 		char *s;
 	} value;
 	struct Node *next, *childs;
} Node;


Node *insert_single_child(node_type type, Node *n);

Node *insert_double_child(node_type type, Node *n1, Node *n2);

Node *insert_triple_child(node_type type, Node *n1, Node *n2, Node *n3);

Node *insert_quad_child(node_type type, Node *n1, Node *n2, Node *n3, Node *n4);

Node *insert_penta_child(node_type type, Node *n1, Node *n2, Node *n3, Node *n4, Node *n5);

Node *insert_int(node_type type, int i);

Node *insert_char(node_type type, char *s);

Node *insert_list(Node *list, Node *node);

void print_node(Node *node, int ident); 

Node *make_node(node_type type);