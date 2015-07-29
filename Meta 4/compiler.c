#include "semantic.h"
#include "compiler.h"
#include "functions.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int str=0;
int printstr=0;
int call=0;
int var=0;
int arrayidx=0;
int ifcond=0;
int ifcont=0;
int label_then=0;
int label_else=0;


Node * get_variable_type(Node *no){
	Node *temp;
	for(temp=no;temp->next!=NULL;temp=temp->next);

	return temp;
}

te_type get_id_type(Node *n,environment_list *env){
	table_element *t;
	for(t=env->locals;t!=NULL;t=t->next){
		if(strcmp(n->value.s,t->name)==0)
			return t->type;
	}
	return _NULL_;
}



te_type expressions(Node *node, environment_list *env){
	Node *aux=node;
	te_type first_child;
	te_type second_child;
	te_type id_type;

	switch(aux->type){
		case IntLit:;
				var++;
				printf("%%%d = alloca i32\n",var );
				printf("store i32 %s, i32* %%%d\n",aux->value.s,var);
				var++;
				printf("%%%d = load i32* %%%d\n",var,var-1 );
				aux->value.s=malloc(sizeof(char)*13);
				sprintf(aux->value.s,"%%%d",var);
				return _integer_;
				break;
		case RealLit:;
				var++;
				printf("%%%d = alloca double\n",var );
				printf("store double %s, double* %%%d\n",aux->value.s,var);
				var++;
				printf("%%%d = load double* %%%d\n",var,var-1 );
				aux->value.s=malloc(sizeof(char)*13);
				sprintf(aux->value.s,"%%%d",var);
				return _real_;
				break;
		case RealDiv:;
				first_child=expressions(aux->childs,env);
				second_child=expressions(aux->childs->next,env);
				if(first_child==_integer_ && second_child==_integer_){
					var++; 
					printf("%%%d = sitofp i32 %s to double\n",var, aux->childs->value.s );
					var++;
					printf("%%%d = sitofp i32 %s to double\n",var, aux->childs->next->value.s);
					var++;
					printf("%%%d = fdiv double %%%d, %%%d\n",var,var-2,var-1);
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;
				}
				else if(first_child==_integer_){
					var++; 
					printf("%%%d = sitofp i32 %s to double\n",var, aux->childs->value.s );
					var++;
					printf("%%%d = fdiv double %%%d, %s\n",var,var-1,aux->childs->next->value.s);
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;
				}
				else if(second_child==_integer_){
					var++; 
					printf("%%%d = sitofp i32 %s to double\n",var, aux->childs->next->value.s );
					var++;
					printf("%%%d = fdiv double %s, %%%d\n",var,aux->childs->value.s,var-1);
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;
				}
				else{
					var++;
					printf("%%%d = fdiv double %.12E, %.12E\n",var,atof(aux->childs->value.s),atof(aux->childs->next->value.s));
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;	
				}
				break;
		case Div:;
				first_child=expressions(aux->childs,env);
				second_child=expressions(aux->childs->next,env);
				var++;
				printf("%%%d = sdiv i32 %s, %s\n", var,aux->childs->value.s,aux->childs->next->value.s);
				aux->value.s=malloc(sizeof(char)*13);
				sprintf(aux->value.s,"%%%d",var);
				return _integer_;
				break;
		case Mod:;
				first_child=expressions(aux->childs,env);
				second_child=expressions(aux->childs->next,env);
				var++;
				printf("%%%d = srem i32 %s, %s\n",var,aux->childs->value.s,aux->childs->next->value.s );
				ifcond++;
				label_then++;
				label_else++;
				ifcont++;
				printf("%%ifcond%d = icmp slt i32 %%%d, 0\n", ifcond,var);
				printf("br i1 %%ifcond%d, label %%then%d, label %%else%d\n",ifcond,label_then,label_else);
				
				printf("then%d:\n", label_then);
				var++;
				printf("%%%d = add i32 %%%d, %s\n",var,var-1,aux->childs->next->value.s);
				printf("br label %%ifcont%d\n",ifcont );
				
				printf("else%d:\n", label_else);
				var++;
				printf("%%%d = alloca i32\n",var);
				printf("store i32 %%%d, i32* %%%d\n", var-2,var);
				var++;
				printf("%%%d = load i32* %%%d\n",var,var-1 );
				printf("br label %%ifcont%d\n",ifcont );

				printf("ifcont%d:\n",ifcont );
				var++;
				printf("%%%d = phi i32 [ %%%d, %%then%d ], [ %%%d, %%else%d ]\n",var,var-3,label_then,var-1,label_else);
				aux->value.s=malloc(sizeof(char)*13);
				sprintf(aux->value.s,"%%%d",var);
				return _integer_;
				break;
		case Mul:;
				first_child=expressions(aux->childs,env);
				second_child=expressions(aux->childs->next,env);
				if(first_child==_integer_ && second_child==_integer_){
					var++;
					printf("%%%d = mul nsw i32 %s, %s\n", var, aux->childs->value.s,aux->childs->next->value.s);
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _integer_;
				}
				else if(first_child==_integer_ && second_child==_real_){
					var++;
					printf("%%%d = sitofp i32 %s to double\n", var,aux->childs->value.s);
					var++;
					printf("%%%d = fmul double %%%d, %s\n", var, var-1,aux->childs->next->value.s);
					
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;	
				}
				else if(first_child==_real_ && second_child==_integer_){
					var++;
					printf("%%%d = sitofp i32 %s to double\n", var,aux->childs->next->value.s);
					var++;
					printf("%%%d = fmul double %%%d, %s\n", var, var-1,aux->childs->value.s);
					
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;	
				}
				else{
					var++;
					printf("%%%d = fmul double %s, %s\n", var, aux->childs->value.s,aux->childs->next->value.s);
					
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;
				}
				
				break;
		case Add:;	
				first_child=expressions(aux->childs,env);
				second_child=expressions(aux->childs->next,env);

				if(first_child==_integer_ && second_child==_integer_){
					var++;
					printf("%%%d = add nsw i32 %s, %s\n", var, aux->childs->value.s,aux->childs->next->value.s);
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _integer_;
				}
				else if(first_child==_integer_ && second_child==_real_){
					var++;
					printf("%%%d = sitofp i32 %s to double\n", var,aux->childs->value.s);
					var++;
					printf("%%%d = fadd double %%%d, %s\n", var, var-1,aux->childs->next->value.s);
					
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;	
				}
				else if(first_child==_real_ && second_child==_integer_){
					var++;
					printf("%%%d = sitofp i32 %s to double\n", var,aux->childs->next->value.s);
					var++;
					printf("%%%d = fadd double %%%d, %s\n", var, var-1,aux->childs->value.s);
					
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;	
				}
				else{
					var++;
					printf("%%%d = fadd double %s, %s\n", var, aux->childs->value.s,aux->childs->next->value.s);
					
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;
				}
				
				break;
		case Sub:;
				first_child=expressions(aux->childs,env);
				second_child=expressions(aux->childs->next,env);
				if(first_child==_integer_ && second_child==_integer_){
					var++;
					printf("%%%d = sub nsw i32 %s, %s\n", var, aux->childs->value.s,aux->childs->next->value.s);
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _integer_;
				}
				else if(first_child==_integer_ && second_child==_real_){
					var++;
					printf("%%%d = sitofp i32 %s to double\n", var,aux->childs->value.s);
					var++;
					printf("%%%d = fsub double %%%d, %s\n", var, var-1,aux->childs->next->value.s);
					
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;	
				}
				else if(first_child==_real_ && second_child==_integer_){
					var++;
					printf("%%%d = sitofp i32 %s to double\n", var,aux->childs->next->value.s);
					var++;
					printf("%%%d = fsub double %s, %%%d\n", var,aux->childs->value.s,var-1);
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;	
				}
				else{
					var++;
					printf("%%%d = fsub double %s, %s\n", var, aux->childs->value.s,aux->childs->next->value.s);
					
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;
				}
				
				break;
		case Not:;
				break;
		case Minus:;
				first_child=expressions(aux->childs,env);
				if(first_child==_integer_){
					var++;
					printf("%%%d = mul nsw i32 %s, 2\n",var, aux->childs->value.s);
					var++;
					printf("%%%d = sub nsw i32 %%%d, %%%d\n", var,var-2,var-1);
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return first_child;	
				}
				else if(first_child==_real_){
					var++;
					printf("%%%d = fmul double %s, 2.0\n",var, aux->childs->value.s);
					var++;
					printf("%%%d = fsub double %%%d, %%%d\n", var,var-2,var-1);
					aux->value.s=malloc(sizeof(char)*13);
					sprintf(aux->value.s,"%%%d",var);
					return first_child;	
				}
				
				break;
		case Plus:;
				first_child=expressions(aux->childs,env);
				aux->value.s=malloc(sizeof(char)*13);
				aux->value.s=aux->childs->value.s;	
				return first_child;

				break;
		case Eq:;
				
				break;
		case Neq:;
				
				break;
		case Geq:;
				
				break;
		case Gt:;
				
				break;
		case Leq:;
				
				break;
		case Lt:;
				
				break;
		case Call:;

				break;
		case And:;
				
				break;
		case Or:;

				break;
		case Id:;
				id_type=return_type(aux,env);
				lower(aux->value.s);
				if(strcmp(aux->value.s,"paramcount")==0){
					var++;
					printf("%%%d = load i32* @argc.addr\n", var);
					sprintf(aux->value.s,"%%%d",var);
					return _integer_;
				}
				else if(id_type==_integer_){
					var++;
					printf("%%%d = alloca i32\n",var );
					var++;
					printf("%%%d = load i32* @%s\n", var,aux->value.s);
					printf("store i32 %%%d, i32* %%%d\n",var,var-1);
					sprintf(aux->value.s,"%%%d",var);
					return _integer_;
				}
				else if(id_type==_real_){
					var++;
					printf("%%%d = alloca double\n",var );
					var++;
					printf("%%%d = load double* @%s\n", var,aux->value.s);
					printf("store double %%%d, double* %%%d\n",var,var-1);
					sprintf(aux->value.s,"%%%d",var);
					return _real_;
				}
				else if(id_type==_boolean_){
					var++;
					printf("%%%d = alloca i1\n",var );
					var++;
					printf("%%%d = load i1* @%s\n", var,aux->value.s);
					printf("store i1 %%%d, i1* %%%d\n",var,var-1);
					sprintf(aux->value.s,"%%%d",var);
					return _boolean_;
				}
				break;

		default: break;
	}
	

}

void main_body(Node *node, environment_list *env){
	Node *aux;
	Node *temp;
	if (node == NULL)
		return;

	switch(node->type) {
		case WriteLn:;
					aux=node->childs;
					while(aux!=NULL){
						if(aux->type==String){
							printstr++;
							call++;							
							int size=get_str_size(aux->value.s);
							printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([%d x i8]* @.str%d, i32 0, i32 0))\n", call,size,printstr);
						}
						else if(aux->type==Id){
							lower(aux->value.s);
							te_type type=get_id_type(aux,env);
							if(strcmp("paramcount",aux->value.s)==0){
								var++;
								call++;
								printf("%%%d = load i32* @argc.addr\n",var );
								printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([3 x i8]* @.str_int, i32 0, i32 0), i32 %%%d)\n", call,var);
							}
							else if(strcmp("true",aux->value.s)==0){
								call++;
								printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([5 x i8]* @.str_true, i32 0, i32 0))\n", call);
							}
							else if(strcmp("false",aux->value.s)==0){
								call++;
								printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([6 x i8]* @.str_false, i32 0, i32 0))\n", call);
							}
							else if(type==_integer_){
								var++;
								call++;
								printf("%%%d = load i32* @%s\n",var,aux->value.s);
								printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([3 x i8]* @.str_int, i32 0, i32 0), i32 %%%d)\n", call,var);
							}
							else if(type==_real_){
								var++;
								call++;
								printf("%%%d = load double* @%s\n",var,aux->value.s);
								printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([6 x i8]* @.str_real, i32 0, i32 0), double %%%d)\n", call,var);	
							}
							else if(type==_function_){
								//TRATAR DAS FUNCOES SEM ARGUMENTOS
								
							}
							else if(type==_boolean_){//boolean
								var++;
								ifcond++;
								ifcont++;
								label_else++;
								label_then++;
								printf("%%%d = load i1* @%s\n",var,aux->value.s);
								printf("%%ifcond%d = icmp eq i1 %%%d, 1\n",ifcond,var);
								printf("br i1 %%ifcond%d, label %%then%d, label %%else%d\n", ifcond,label_then,label_else);
								printf("then%d:\n",label_then);
								call++;
								printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([5 x i8]* @.str_true, i32 0, i32 0))\n",call);
								printf("br label %%ifcont%d\n",ifcont);
								printf("else%d:\n", label_else);
								call++;
								printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([6 x i8]* @.str_false, i32 0, i32 0))\n",call);
								printf("br label %%ifcont%d\n",ifcont);
								printf("ifcont%d:\n", ifcont);
							}
						}
						else if(aux->type==IntLit){
							call++; 
							printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([3 x i8]* @.str_int, i32 0, i32 0), i32 %d)\n", call,atoi(aux->value.s));
					
						}
						else if(aux->type==RealLit){
							call++; 
							printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([6 x i8]* @.str_real, i32 0, i32 0), double %.12E)\n", call,atof(aux->value.s));
					
						}
						else if(aux->type==Call){
							//TRATAR DAS FUNCOES COM ARGUMENTOS
						}
						else{
							te_type type3=expressions(aux,env);
							if(type3==_integer_){
								
								
								call++; 
								printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([3 x i8]* @.str_int, i32 0, i32 0), i32 %s)\n", call,aux->value.s);
					
								
							}
							else if(type3==_real_){
								call++; 
								printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([6 x i8]* @.str_real, i32 0, i32 0), double %s)\n", call,aux->value.s);
					
							}
							else if(type3==_boolean_){
								var++;
								ifcond++;
								ifcont++;
								label_else++;
								label_then++;
								printf("%%%d = load i1* %s\n",var,aux->value.s);
								printf("%%ifcond%d = icmp eq i1 %%%d, 1\n",ifcond,var);
								printf("br i1 %%ifcond%d, label %%then%d, label %%else%d\n", ifcond,label_then,label_else);
								printf("then%d:\n",label_then);
								call++;
								printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([5 x i8]* @.str_true, i32 0, i32 0))\n",call);
								printf("br label %%ifcont%d\n",ifcont);
								printf("else%d:\n", label_else);
								call++;
								printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([6 x i8]* @.str_false, i32 0, i32 0))\n",call);
								printf("br label %%ifcont%d\n",ifcont);
								printf("ifcont%d:\n", ifcont);
							}
						}
						aux=aux->next;
					}
					call++; 
					printf("%%call%d = call i32 (i8*, ...)* @printf(i8* getelementptr inbounds ([2 x i8]* @.new_line, i32 0, i32 0))\n", call);
					break;
		case ValParam:;
					aux=node->childs;
					int s;
					if(aux->type==IntLit){
						s=atoi(aux->value.s);

					}
					else{
						var++;
						lower(aux->value.s);
						printf("%%%d = load i32* @%s\n",var,aux->value.s);
						s=var;
					}
					var++;
					call++;
					arrayidx++;
					printf("%%%d = load i8*** @argv.addr\n", var);
					printf("%%arrayidx%d = getelementptr inbounds i8** %%%d, i32 %d\n",arrayidx,var,s);
					aux=aux->next;
					lower(aux->value.s);
					var++;
					call++;
					printf("%%%d = load i8** %%arrayidx%d\n", var,arrayidx);
					printf("%%call%d = call i32 @atoi(i8* %%%d)\n", call,var);
					printf("store i32 %%call%d, i32* @%s\n", call, aux->value.s);		
					
					break;
		case Assign:;
					aux=node->childs;
					lower(aux->value.s);
					te_type type=get_id_type(aux,env);
					char *v_type;
					char *v;
					if(type==_integer_){
						v_type="i32";
					}
					else if(type==_real_){
						v_type="double";
					}
					else if(type==_boolean_){//BOOLEAN
						v_type="i1";
					}
					v=strdup(aux->value.s);
					aux=aux->next;
					
					
					if(aux->type==IntLit || aux->type==RealLit){
						if(type==_real_){
							printf("store %s %.12E, %s* @%s \n",v_type,atof(aux->value.s),v_type,v);	
						}
						else{
							printf("store %s %s, %s* @%s \n",v_type,aux->value.s,v_type,v);
						}
					}
					else if(aux->type==Id){
						lower(aux->value.s);
						te_type type2=get_id_type(aux,env);
						if(strcmp(aux->value.s,"paramcount")==0){
							var++;	
							printf("%%%d = load i32* @argc.addr\n",var );
							printf("store i32 %%%d, i32* @%s\n",var,v );
						}
						else if(type==_real_ && type2==_integer_){
							var++;
							printf("%%%d = load i32* @%s\n",var,aux->value.s);
							var++;
							printf("%%%d = uitofp i32 %%%d to double\n", var,var-1);
							printf("store double %%%d, double* @%s\n",var,v);
						}
						else{
							var++;
							printf("%%%d = load %s* @%s\n",var,v_type,aux->value.s);
							printf("store %s %%%d, %s* @%s\n",v_type,var,v_type,v);
						}
						
						
						
					}
					else{//EXPRESSION
						te_type type3=expressions(aux,env);
						if(type3==_integer_){
							if(type==_real_){
								var++;
								printf("%%%d = sitofp i32 %%%d to double\n",var,var-1 );
								printf("store double %%%d, double* @%s\n",var,v );
							}
							else if(type==_integer_){
								printf("store i32 %%%d, i32* @%s\n",var,v);
							}
							
						}
						else if(type3==_real_){

							printf("store double %%%d, double* @%s\n",var,v);
						}
					}

					break;

		
	}

	if(node->childs)
		main_body(node->childs,env);
	if(node->next)
		main_body(node->next,env);
}

void main_declaration(Node *node, environment_list *env){
	printf("@.new_line = private unnamed_addr constant [2 x i8] c\"\\0A\\00\"\n");
	printf("@argc.addr = common global i32 0\n");
	printf("@argv.addr = common global i8** null\n");
	printf("@.str_int = private unnamed_addr constant [3 x i8] c\"%%d\\00\"\n");
	printf("@.str_real = private unnamed_addr constant [6 x i8] c\"%%.12E\\00\"\n");
	printf("@.str_true = private unnamed_addr constant [5 x i8] c\"TRUE\\00\"\n");
	printf("@.str_false = private unnamed_addr constant [6 x i8] c\"FALSE\\00\"\n");
	printf("@true = common global i1 1\n");
	printf("@false = common global i1 0\n");
	printf("define i32 @main (i32 %%argc, i8** %%argv){\n");
	printf("entry:\n");
	printf("%%0 = sub i32 %%argc, 1\n");
	printf("store i32 %%0, i32* @argc.addr\n");
	printf("store i8** %%argv, i8*** @argv.addr\n");

	table_element *t;
	environment_list *e;
	for(t=env->locals;t->next!=NULL;t=t->next);//go to program table
	e=t->header;

	main_body(node,e);

	printf("ret i32 0\n}\n");
	printf("declare i32 @printf(i8*, ...)\n");
	printf("declare i32 @atoi(i8*)\n");
}

int get_str_size(char *s){
	int i;
	int size=0;
	int plicas=0;
	
	for(i=1;i<strlen(s);i++){
		if(s[i]=='\'')
			plicas++;
		else
			plicas=0;
		if(plicas<2)
			size++;
		else if(plicas==3){
			plicas=1;
			size++;
		}
	}
	return size;
}


void global_declaration(environment_list *env, Node *node){
	Node *aux;
	Node *temp;
	if (node == NULL)
		return;
	int i;
	switch(node->type) {
		case String:;
					str++;
					
					int size=get_str_size(node->value.s);

					printf("@.str%d = private unnamed_addr constant [%d x i8] c \"",str,size);
					int plicas=0;
					for(i=1;i<strlen(node->value.s)-1;i++){
						if(node->value.s[i]=='\'')
							plicas++;
						else
							plicas=0;

						if(plicas<2)
							printf("%c",node->value.s[i]);	
						else if(plicas==3){
							plicas=1;
							printf("%c",node->value.s[i]);
						}
					}
					printf("\\00\"\n");


					break;

		case VarDecl:;
					
					aux=node->childs;
					
					temp=get_variable_type(aux);
					lower(temp->value.s);
					lower(aux->value.s);

					while(aux->next!=NULL){
						
						if(strcmp(temp->value.s,"integer")==0){
							printf("@%s = common global i32 0\n",aux->value.s);
						}
						else if(strcmp(temp->value.s,"real")==0){
							printf("@%s = common global double 0.000000e+00\n",aux->value.s);
						}
						else{
							printf("@%s = common global i1 0\n",aux->value.s);
						}
						aux=aux->next;
					}

					break;
	}

	if(node->childs)
		global_declaration(env,node->childs);
	if(node->next)
		global_declaration(env,node->next);



}

