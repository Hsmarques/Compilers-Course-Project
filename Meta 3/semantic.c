
#include "symbol_table.h"
#include "semantic.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *types2[]={"_boolean_", "_integer_", "_real_", "_function_", "_program_", "_type_", "_false_" , "_true_", "_NULL_"};



void lower(char *p){
	for ( ; *p; ++p) *p = tolower(*p);
}


void check_if_declared(Node *aux, environment_list *env){
	table_element *temp;
	
	char *s=(char*)strdup(aux->value.s);
	lower(s);
	for(temp = env->locals; temp!=NULL; temp=temp->next){
		
		if(strcmp(temp->name,s)==0){
			return;
		}
	}
	if(env->previous!=NULL){
		
		check_if_declared(aux, env->previous);
	}else{
		printf("Line %d, col %d: Symbol %s not defined\n",aux->line,aux->col,aux->value.s);
		exit(0);
	}
}

void check_if_defined(Node *aux,environment_list *env){
	table_element *temp;
	char *s=(char*)strdup(aux->value.s);
	lower(s);
	for(temp = env->locals; temp!=NULL; temp=temp->next){
		if(strcmp(temp->name,s)==0 && temp->type==_function_){
			printf("Line %d, col %d: Symbol %s already defined\n",aux->line, aux->col, aux->value.s);
			exit(0);
		}
	}
	if(env->previous->previous!=NULL){
		check_if_defined(aux, env->previous);
	}
	return;
}

te_type check_type(Node *aux ,environment_list *env){
	table_element *temp;
	
	char *s=(char*)strdup(aux->value.s);
	lower(s);

	for(temp = env->locals; temp!=NULL; temp=temp->next){
		if(strcmp(temp->name,s)==0 && temp->type==_type_){
			return temp->value;
		}
		else if(strcmp(temp->name,s)==0){
			printf("Line %d, col %d: Type identifier expected\n",aux->line,aux->col);
			exit(0);
		}
	}
	if(env->previous!=NULL){
		return check_type(aux, env->previous);
	}else{
		printf("Line %d, col %d: Symbol %s not defined\n",aux->line,aux->col,aux->value.s);
		exit(0);
	}
}


Node * get_type(Node *no){
	Node *temp;
	for(temp=no;temp->next!=NULL;temp=temp->next);

	return temp;
}


table_element *insert_func(int is_defined,Node *aux, environment_list *env, environment_list *outter_table){
	check_if_defined(aux,env);
	lower(aux->value.s);//value to lowercase
						
	table_element *nodeFuncDef = malloc(sizeof(table_element));//cria elemento que vai ser introduzido na tabela 
	nodeFuncDef->name=aux->value.s;
	nodeFuncDef->type=_function_;
	nodeFuncDef->flag="";
	nodeFuncDef->value=_NULL_;
	nodeFuncDef->header=malloc(sizeof(environment_list));//cria new table ligada ao elemento
	nodeFuncDef->header->defined=is_defined;
	nodeFuncDef->header->name="Function";
	nodeFuncDef->header->previous=env;
	//Primeira linha da new table contem ID da function, tipo e flag
	//tipo da function está localizado em aux->next->next->value.s
	//insere primeira linha da new table
	tolower(aux->next->next->value.s);
	nodeFuncDef->header->locals=insert_line(nodeFuncDef->header->locals,aux->value.s,check_type(aux->next->next,env),"return",_NULL_);
	//insere o resto das linhas da new table
	nodeFuncDef->header=insert_table(aux->next,nodeFuncDef->header,env,outter_table);

	return nodeFuncDef;
}

table_element *insert_funcdef2(Node *aux, environment_list *env, environment_list *outter_table){
	table_element *temp;

	char *s=(char*)strdup(aux->value.s);
	lower(s);
	for(temp=env->locals;temp!=NULL;temp=temp->next){
		if(strcmp(temp->name,s)==0 && temp->type==_function_){
			if(temp->header->defined){
				printf("Line %d, col %d: Symbol %s already defined\n",aux->line, aux->col, aux->value.s);
				exit(0);
			}
			temp->header=insert_table(aux->next,temp->header,env,outter_table);
			temp->header->defined=1;
			return temp;
		}
		else if(strcmp(temp->name,s)==0){
			printf("Line %d, col %d: Function identifier expected\n",aux->line,aux->col);
			exit(0);		
		}
	}
	return NULL;
	printf("Line %d, col %d: Symbol %s not defined\n",aux->line,aux->col,aux->value.s);
	exit(0);
	
}

void check_duplicates(Node *aux,table_element *initial){
	table_element *temp;

	if(initial==NULL)
		return;
	char *s=(char*)strdup(aux->value.s);
	lower(s);
	for(temp=initial;temp!=NULL;temp=temp->next){
		if(strcmp(s,temp->name)==0){
			printf("Line %d, col %d: Symbol %s already defined\n",aux->line, aux->col, aux->value.s);
			exit(0);
		}
	}
}


int get_number_arg(Node *aux,environment_list *env){
	table_element *temp;
	char *s=(char*)strdup(aux->value.s);
	lower(s);

	for(temp=env->locals;temp!=NULL;temp=temp->next){

		if(strcmp(temp->name,s)==0 && temp->type==_function_){

			return temp->header->number_of_arguments;
		}
		else if(strcmp(temp->name,s)==0){
			printf("Line %d, col %d: Type identifier expected\n",aux->line,aux->col);
			exit(0);
		}
	}
	if(env->previous!=NULL){
		return get_number_arg(aux, env->previous);
	}else{

		printf("Line %d, col %d: Symbol %s not defined\n",aux->line,aux->col,aux->value.s);
		exit(0);
	}
}

void check_number_of_arguments(Node *aux, environment_list *env){
	Node *temp;
	char *s=(char*)strdup(aux->value.s);
	lower(s);
	int temp_number=0;
	int number_arg=get_number_arg(aux,env);
	temp=aux;
	
	temp=temp->next;
	
	while(temp!=NULL){
		temp_number++;
		temp=temp->next;
	}
	if(temp_number!=number_arg){
		printf("Line %d, col %d: Wrong number of arguments in call to function %s (got %d,expected %d)\n",aux->line,aux->col,aux->value.s,temp_number,number_arg );
		exit(0);
	}

}

environment_list* insert_table(Node* node, environment_list* env, environment_list* previous, environment_list *outter_table){
	Node *aux;
	Node *aux2;
	table_element *temp;
	table_element *nodeFunc;
	switch (node->type){

		case VarDecl: 	;
						aux=node;
						while(aux!=NULL){
							aux2=aux->childs;
							Node *value=get_type(aux2);
							while(aux2->next!=NULL){
								check_duplicates(aux2,env->locals);
								lower(aux2->value.s);
								env->locals=insert_line(env->locals,aux2->value.s,check_type(value,env),"",_NULL_);
								aux2=aux2->next;
							}
							aux=aux->next;
						}
						return env;
		case FuncDecl:
		case FuncDef2:	
		case FuncDef:	;
						aux=node;
						while(aux!=NULL){
							if(aux->type==FuncDef)
								nodeFunc=insert_func(1,aux->childs,env,outter_table);
							else if(aux->type==FuncDef2)
								nodeFunc=insert_funcdef2(aux->childs,env,outter_table);
							else if(aux->type==FuncDecl)
								nodeFunc=insert_func(0,aux->childs,env,outter_table);

							if(env->locals==NULL){
								env->locals=nodeFunc;
							}
							else if(aux->type==FuncDef2){
								if(nodeFunc!=NULL){
									for(temp=env->locals;temp->next!=NULL;temp=temp->next){
										if(strcmp(temp->name,nodeFunc->name)==0){
											temp=nodeFunc;
											break;	
										}
									}
								}	
							}
							else{
								for(temp=env->locals;temp->next!=NULL;temp=temp->next);
								temp->next=nodeFunc;
							}

								aux=aux->next;
						}
						return env;			

		case Params:
		case VarParams:	;
						aux=node;
						while(aux!=NULL){
							aux2=aux->childs;
							char *flag;
							if(aux->type==Params) flag="param";
							else flag="varparam";
							Node *value=get_type(aux2);
							while(aux2->next!=NULL){
								check_duplicates(aux2,env->locals);
								lower(aux2->value.s);
								env->number_of_arguments++;
								env->locals=insert_line(env->locals,aux2->value.s,check_type(value,env),flag,_NULL_);
								aux2=aux2->next;
							}
							aux=aux->next;
						}

							return env;
		
		case IfElse:	;
						//check_statements(node->childs,"if",env);
						break;
		case While:		;
						//check_statements(node->childs,"while",env);
						break;
		case ValParam:	;
						//check_statements(node->childs,"val-paramstr",env);
						break;
		case Repeat:	;
						//check_statements(node->childs->next,"repeat-until",env);
						break;					
		case Assign:	;
						//check_expressions();
						break;
		
		case WriteLn:	;
						//check_writeln(node);
						break;
		case Call:	;
					check_number_of_arguments(node->childs,env);
					break;
		
		default: break;
	}
	
	if(node->childs){
		env = insert_table(node->childs,env,previous,outter_table);
	}
	if(node->next)
		env = insert_table(node->next, env,previous,outter_table);
		
	return env;
}


table_element *insert_line(table_element *initial, char* name, te_type type, char* flag, te_type value){
	
	table_element *temp;
    table_element *node = malloc(sizeof(table_element));
    node->name = (char*)strdup(name);
    node->type = type;
    node->flag = (char*)strdup(flag);
    node->value= value;

    if(initial==NULL){
        initial=node;
        return initial;
    }

    for(temp = initial; temp->next != NULL; temp = temp->next);
        temp->next = node;
         
    return initial;

}

environment_list *insert_paramcount(environment_list *previous){
	environment_list *node=malloc(sizeof(environment_list));
	node->name="Function";
	node->previous=previous;
	node->locals=insert_line(node->locals,"paramcount",_integer_,"return",_NULL_);

	return node;

}

environment_list *insert_program(environment_list *previous){
	environment_list *node=malloc(sizeof(environment_list));
	node->name="Program";
	node->previous=previous;

	return node;

}

table_element *insert_outter_table(environment_list *current_header,table_element *initial, char* name, te_type type, char* flag, te_type value){
    table_element *temp;
 
    table_element *node = malloc(sizeof(table_element));
    node->name = (char*)strdup(name);
    node->type = type;
    node->flag = (char*)strdup(flag);
    node->value= value;
    
    if(strcmp(name,"paramcount")==0){
    	node->header=insert_paramcount(current_header);
    }
    if(strcmp(name,"program")==0){
    	node->header=insert_program(current_header);
    }

    if(initial==NULL){
        initial=node;
        return initial;
    }
     
    for(temp = initial; temp->next != NULL; temp = temp->next);
        temp->next = node;
         
    return initial;
}

environment_list* semantic_analysis(Node *myprogram){
	
	
	environment_list* env = malloc(sizeof(environment_list));
	env->previous=NULL;
	env->name="Outer";
	env->locals=insert_outter_table(env,env->locals,"boolean",_type_,"constant",_boolean_);
	env->locals=insert_outter_table(env,env->locals,"integer", _type_, "constant", _integer_);
	env->locals=insert_outter_table(env,env->locals,"real", _type_, "constant", _real_);
	env->locals=insert_outter_table(env,env->locals,"false" ,_boolean_ ,"constant" ,_false_);
	env->locals=insert_outter_table(env,env->locals,"true", _boolean_, "constant", _true_);
	env->locals=insert_outter_table(env,env->locals,"paramcount", _function_, "", _NULL_);
	env->locals=insert_outter_table(env,env->locals,"program", _program_, "", _NULL_);
	
	table_element *temp;
	for(temp=env->locals;temp->type!=_program_;temp=temp->next);
	

	temp->header = insert_table(myprogram, temp->header, env, env);
	
	return env;
}



void print_semantic(environment_list* env){
	table_element *temp;

	printf("===== %s Symbol Table =====\n", env->name);
	for(temp=env->locals;temp!=NULL;temp=temp->next){
		if (strcmp(temp->flag, "")==0)    
		 	printf("%s\t%s\n", temp->name,types2[temp->type]);
        else if (temp->value==_NULL_)
            printf("%s\t%s\t%s\n", temp->name,types2[temp->type],temp->flag);
      	else    
      		printf("%s\t%s\t%s\t%s\n",temp->name,types2[temp->type],temp->flag,types2[temp->value]);
	}
	
	
	for(temp=env->locals;temp!=NULL;temp=temp->next){	
		if(temp->type==_function_ || temp->type==_program_){	
			if(temp->header!=NULL){
				printf("\n");	
				print_semantic(temp->header);
			}
		}
	}
	
}
