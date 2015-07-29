#include <stdlib.h>
#include <stdio.h>
#include "functions.h"



char *types[]={"Program", "VarPart","FuncPart","VarDecl","FuncDecl","FuncDef","FuncDef2","FuncParams","Params","VarParams",
				"Assign","IfElse","Repeat","StatList","ValParam","While","WriteLn","Add","And","Call","Div","Eq","Geq",
				"Gt","Leq","Lt","Minus","Mod","Mul","Neq","Not","Or","Plus","RealDiv","Sub","Id","IntLit","RealLit","String"};


Node *insert_single_child(node_type type, Node *n) {
	Node *node = (Node*) malloc(sizeof(Node));

	node->type = type;
	node->next = NULL;	
	node->childs = n;
	
	return node;
}

Node *insert_double_child(node_type type, Node *n1, Node *n2) {
	
	Node *node = (Node*) malloc(sizeof(Node));
	node->type = type;
	node->next = NULL;
	n1 = insert_list(n1, n2);
	
	node->childs = n1;
	
	return node;
}

Node *insert_triple_child(node_type type, Node *n1, Node *n2, Node *n3) {

	Node *node = (Node*) malloc(sizeof(Node));
	node->type = type;
	node->next = NULL;
	
	n2 = insert_list(n2, n3);
	n1 = insert_list(n1, n2);
	
	node->childs = n1;
	
	return node;
}


Node *insert_int(node_type type, int i) {

	Node *node = (Node*) malloc(sizeof(Node));
	node->type = type;
	node->value.i = i;
	node->next = NULL;
	node->childs = NULL;
	
	return node;
}

Node *insert_char(node_type type, char *s) {

	Node *node = (Node*) malloc(sizeof(Node));
	node->type = type;
	node->value.s = s;
	node->next = NULL;
	node->childs = NULL;
	
	return node;
}

Node *make_node(node_type type){
	Node *node = (Node*) malloc(sizeof(Node));
	node->type = type;
	node->next = NULL;

	return node;
}

Node *insert_list(Node *list, Node *node) {
		
	Node *temp;
	if(list == NULL){
		return node;
	}

	for(temp = list; temp->next != NULL; temp = temp->next);
		
	temp->next = node;
		
	return list;
}

void print_node(Node *node, int ident) {
	if (node == NULL)
		return;
	int i;
	for(i=0;i<ident;i++)
		printf(".");
	switch(node->type) {
		case Program: printf("Program\n"); break;
		case Id: printf("Id(%s)\n", node->value.s); break;
		case RealLit: printf("RealLit(%s)\n", node->value.s); break;
		case IntLit: printf("IntLit(%s)\n",  node->value.s); break;
		case String: printf("String(%s)\n",  node->value.s); break;
		default: printf("%s\n",  types[node->type]); break;
	}

	if(node->childs)
		print_node(node->childs, ident+2);
	if(node->next)
		print_node(node->next, ident);
}
