
#include "symbol_table.h"
#include "semantic.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

char *types2[]={"_boolean_", "_integer_", "_real_", "_function_", "_program_", "_type_", "_false_" , "_true_", "_NULL_"};

char *types3[]={"Program", "VarPart","FuncPart","VarDecl","FuncDecl","FuncDef","FuncDef2","FuncParams","Params","VarParams",
				"Assign","IfElse","Repeat","StatList","ValParam","While","WriteLn","Add","And","Call","Div","Eq","Geq",
				"Gt","Leq","Lt","Minus","Mod","Mul","Neq","Not","Or","Plus","RealDiv","Sub","Id","IntLit","RealLit","String"};



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
		}else if(strcmp(temp->name,s)==0){
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
	}
	if(env->previous!=NULL){
		return get_number_arg(aux, env->previous);
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
		printf("Line %d, col %d: Wrong number of arguments in call to function %s (got %d, expected %d)\n",aux->line,aux->col,aux->value.s,temp_number,number_arg );
		exit(0);
	}

}

te_type return_type(Node *aux, environment_list *env){
	table_element *temp = env->locals;
	char *s=(char*)strdup(aux->value.s);
	lower(s);

	while(temp!=NULL){
		if(strcmp(temp->name,s)==0){
			return temp->type;
		}
		temp=temp->next;
	}
	if(env->previous!=NULL){
		return return_type(aux, env->previous);
	}

}

te_type check_if_is_function(Node *aux, environment_list *env){
	table_element *temp = env->locals;
	char *s=(char*)strdup(aux->value.s);
	lower(s);

	while(temp!=NULL){
		//printf("temp->name: %s, s: %s ,temp->flag: %s\n",temp->name, s, temp->flag);
		if (strcmp(temp->name,s)==0 && temp->header!=NULL)
		{
			return temp->header->locals->type;
		}
		temp=temp->next;
	}
	if(env->previous!=NULL){
		return check_if_is_function(aux, env->previous);
	}else{
		return _NULL_;
	}

}

te_type check_expressions(Node *node, environment_list *env){
	Node *temp= node;
	te_type filho1;
	te_type filho2;
	te_type id_type;

	switch(temp->type){
		case And:
			filho1=check_expressions(temp->childs, env);
			filho2=check_expressions(temp->childs->next, env);
			if(filho1 == _boolean_ && filho2 == _boolean_){
				return _boolean_;
			}else{
				printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", temp->line, temp->col,temp->value.s, types2[filho1],types2[filho2]);
				exit(0);
			}
			break;

		case Or:
			filho1=check_expressions(temp->childs, env);
			filho2=check_expressions(temp->childs->next, env);
			if(filho1==_boolean_ && filho2==_boolean_){
				return _boolean_;
			}else{
				printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", temp->line, temp->col, temp->value.s, types2[filho1],types2[filho2]);
				exit(0);
			}
			break;

		case RealDiv:
			filho1=check_expressions(temp->childs, env);
			filho2=check_expressions(temp->childs->next, env);
			if((filho1==_integer_ || filho1==_real_) && (filho2==_integer_ || filho2==_real_)){
				return _real_;
			}else{
				printf("Line %d, col %d: Operator / cannot be applied to types %s, %s\n", temp->line, temp->col, types2[filho1],types2[filho2]);
				exit(0);
			}
			break;

		case Div:
			filho1=check_expressions(temp->childs, env);
			filho2=check_expressions(temp->childs->next, env);
			if(filho1==_integer_ && filho2==_integer_){
				return _integer_;
			}else{
				printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", temp->line, temp->col, temp->value.s, types2[filho1],types2[filho2]);
				exit(0);
			}
			break;

		case Mod:
			filho1=check_expressions(temp->childs, env);
			filho2=check_expressions(temp->childs->next, env);
			if(filho1==_integer_ && filho2==_integer_){
				return _integer_;
			}else{
				printf("Line %d, col %d: Operator %s cannot be applied to types %s, %s\n", temp->line, temp->col, temp->value.s, types2[filho1],types2[filho2]);
				exit(0);
			}
			break;

		case Mul:
			filho1=check_expressions(temp->childs,env);
			filho2=check_expressions(temp->childs->next, env);
			if(filho1==_integer_ && filho2==_integer_){
				return _integer_;
			}else if((filho1==_real_ || filho1==_integer_) && (filho2==_integer_ || filho2==_real_) ){
				return _real_;
			}else{
				printf("Line %d, col %d: Operator * cannot be applied to types %s, %s\n", temp->line, temp->col, types2[filho1],types2[filho2]);
				exit(0);
			}
			break;

		case Add:
			filho1=check_expressions(temp->childs, env);
			filho2=check_expressions(temp->childs->next, env);
			if(filho1==_integer_ && filho2==_integer_){
				return _integer_;
			}else if((filho1==_real_ || filho1==_integer_) && (filho2==_integer_ || filho2==_real_) ){
				return _real_;
			}else{
				printf("Line %d, col %d: Operator + cannot be applied to types %s, %s\n", temp->line, temp->col, types2[filho1],types2[filho2]);
				exit(0);
			}
			break;

		case Sub:
			filho1=check_expressions(temp->childs, env);
			filho2=check_expressions(temp->childs->next, env);
			if(filho1==_integer_ && filho2==_integer_){
				return _integer_;
			}else if((filho1==_real_ || filho1==_integer_) && (filho2==_integer_ || filho2==_real_) ){
				return _real_;
			}else{
				printf("Line %d, col %d: Operator - cannot be applied to types %s, %s\n", temp->line, temp->col, types2[filho1],types2[filho2]);
				exit(0);
			}
			break;

		case Call:
			filho1 = return_type(temp->childs,env);
			check_number_of_arguments(temp->childs,env);
			return filho1;
			break;

		case Not:
			filho1 = check_expressions(temp->childs,env);
			if(filho1!=_boolean_){
				printf("Line %d, col %d: Operator %s cannot be applied to type %s\n", temp->line, temp->col, temp->value.s, types2[filho1]);
				exit(0);
			}
			return filho1;
			break;

		case Minus:
			filho1 = check_expressions(temp->childs,env);
			if (filho1 != _real_ && filho1 != _integer_){
				printf("Line %d, col %d: Operator - cannot be applied to type %s\n", temp->line, temp->col, types2[filho1]);
				exit(0);
			}
			return filho1;
			break;

		case Plus:
			filho1 = check_expressions(temp->childs,env);
			if (filho1 != _real_ && filho1 != _integer_){
				printf("Line %d, col %d: Operator + cannot be applied to type %s\n", temp->line, temp->col, types2[filho1]);
				exit(0);
			}
			return filho1;
			break;

		case Eq:
			filho1 = check_expressions(temp->childs,env);
			filho2 = check_expressions(temp->childs->next, env);
			if ((filho1 == _integer_ || filho1 == _real_) && (filho2 == _integer_ || filho2 == _real_)){
				return _boolean_;
			}else if(filho1 == _boolean_ && filho2 == _boolean_){
				return _boolean_;
			}else{
				printf("Line %d, col %d: Operator = cannot be applied to types %s, %s\n", temp->line, temp->col, types2[filho1],types2[filho2]);
				exit(0);
			}
			
			break;

		case Geq:
			filho1 = check_expressions(temp->childs,env);
			filho2 = check_expressions(temp->childs->next, env);
			if ((filho1 == _integer_ || filho1 == _real_) && (filho2 == _integer_ || filho2 == _real_)){
				return _boolean_;
			}else if(filho1 == _boolean_ && filho2 == _boolean_){
				return _boolean_;
			}else{
				printf("Line %d, col %d: Operator >= cannot be applied to types %s, %s\n", temp->line, temp->col, types2[filho1],types2[filho2]);
				exit(0);
			}
			break;

		case Gt:
			filho1 = check_expressions(temp->childs,env);
			filho2 = check_expressions(temp->childs->next, env);
			if ((filho1 == _integer_ || filho1 == _real_) && (filho2 == _integer_ || filho2 == _real_)){
				return _boolean_;
			}else if(filho1 == _boolean_ && filho2 == _boolean_){
				return _boolean_;
			}else{
				printf("Line %d, col %d: Operator > cannot be applied to types %s, %s\n", temp->line, temp->col, types2[filho1],types2[filho2]);
				exit(0);
			}
			break;

		case Leq:
			filho1 = check_expressions(temp->childs,env);
			filho2 = check_expressions(temp->childs->next, env);
			if ((filho1 == _integer_ || filho1 == _real_) && (filho2 == _integer_ || filho2 == _real_)){
				return _boolean_;
			}else if(filho1 == _boolean_ && filho2 == _boolean_){
				return _boolean_;
			}else{
				printf("Line %d, col %d: Operator <= cannot be applied to types %s, %s\n", temp->line, temp->col, types2[filho1],types2[filho2]);
				exit(0);
			}
			break;

		case Lt:
			filho1 = check_expressions(temp->childs,env);
			filho2 = check_expressions(temp->childs->next, env);
			if ((filho1 == _integer_ || filho1 == _real_) && (filho2 == _integer_ || filho2 == _real_)){
				return _boolean_;
			}else if(filho1 == _boolean_ && filho2 == _boolean_){
				return _boolean_;
			}else{
				printf("Line %d, col %d: Operator < cannot be applied to types %s, %s\n", temp->line, temp->col, types2[filho1],types2[filho2]);
				exit(0);
			}
			break;

		case Neq:
			filho1 = check_expressions(temp->childs,env);
			filho2 = check_expressions(temp->childs->next, env);
			if ((filho1 == _integer_ || filho1 == _real_) && (filho2 == _integer_ || filho2 == _real_)){
				return _boolean_;
			}else if(filho1 == _boolean_ && filho2 == _boolean_){
				return _boolean_;
			}else{
				printf("Line %d, col %d: Operator <> cannot be applied to types %s, %s\n", temp->line, temp->col, types2[filho1],types2[filho2]);
				exit(0);
			}
			break;

		case IntLit:
			return _integer_;
			break;

		case RealLit:
			return _real_;
			break;

		case Id:
			check_if_declared(temp,env);
			id_type = return_type(temp,env);
			return id_type;
			break;

		default: break;

	}
	//printf("%s\n",types3[node->type]);
}


environment_list* insert_table(Node* node, environment_list* env, environment_list* previous, environment_list *outter_table){
	Node *aux;
	Node *aux2;
	table_element *temp;
	table_element *nodeFunc;
	te_type te_temp, te_temp2, te_temp3;
	char *s;

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
						te_temp=check_expressions(node->childs,env);
						if(te_temp!=_boolean_){
							printf("Line %d, col %d: Incompatible type in if statement (got %s, expected _boolean_)\n", node->childs->line, node->childs->col,types2[te_temp] );
							exit(0);
						}
						
						break;
		case While:		;
						te_temp=check_expressions(node->childs,env);
						if(te_temp!=_boolean_){
							printf("Line %d, col %d: Incompatible type in while statement (got %s, expected _boolean_)\n", node->childs->line, node->childs->col,types2[te_temp] );
							exit(0);
						}
						break;
		case ValParam:	;
						te_temp=check_expressions(node->childs,env);
						te_temp2=check_expressions(node->childs->next,env);
						if(te_temp!=_integer_){
							printf("Line %d, col %d: Incompatible type in val-paramstr statement (got %s, expected _integer_)\n", node->childs->line, node->childs->col,types2[te_temp] );
							exit(0);
						}
						if(te_temp2!=_integer_){
							printf("Line %d, col %d: Incompatible type in val-paramstr statement (got %s, expected _integer_)\n", node->childs->next->line, node->childs->next->col,types2[te_temp2] );
							exit(0);
						}
						break;
		case Repeat:	;
						te_temp=check_expressions(node->childs->next,env);
						if(te_temp!=_boolean_){
							printf("Line %d, col %d: Incompatible type in repeat-until statement (got %s, expected _boolean_)\n", node->childs->next->line, node->childs->next->col ,types2[te_temp] );
							exit(0);
						}
						break;					
		case Assign:	;
						//printf("env->name: %s\n",env->locals->name);
						check_if_declared(node->childs,env);
						te_temp  = check_expressions(node->childs,env);
						te_temp2 = check_expressions(node->childs->next,env);
						if(te_temp==_type_ || te_temp == _function_){
							printf("Line %d, col %d: Variable identifier expected\n", node->childs->line, node->childs->col );
							exit(0);
						}
						if(te_temp==_real_ && (te_temp2!=_integer_ && te_temp2!=_real_)){
							if(te_temp2==_function_){
								if(node->childs->next->type == Call){
									te_temp3 = check_if_is_function(node->childs->next->childs,env);
									
									s=(char*)strdup(node->childs->next->childs->value.s);
									lower(s);
									if (strcmp(env->locals->name,s)==0){
										check_number_of_arguments(node->childs->next->childs,env);
									}else if(strcmp(env->name,"Program")==0){
										check_number_of_arguments(node->childs->next->childs,env);
									}else{
										check_number_of_arguments(node->childs->next->childs,previous);
									}
									if (te_temp3!=_integer_ && te_temp3!=_real_)
									{
										printf("Line %d, col %d: Incompatible type in assigment to %s (got %s, expected _real_)\n", node->childs->next->childs->line, node->childs->next->childs->col, node->childs->value.s, types2[te_temp3] );
										exit(0);
									}
								}else{
									te_temp3 = check_if_is_function(node->childs->next,env);
									
									s=(char*)strdup(node->childs->next->value.s);
									lower(s);
									if (strcmp(env->locals->name,s)==0){
										check_number_of_arguments(node->childs->next,env);
									}else if(strcmp(env->name,"Program")==0){
										check_number_of_arguments(node->childs->next,env);
									}else{
										check_number_of_arguments(node->childs->next,previous);
									}
									if (te_temp3!=_integer_ && te_temp3!=_real_)
									{
										printf("Line %d, col %d: Incompatible type in assigment to %s (got %s, expected _real_)\n", node->childs->next->childs->line, node->childs->next->childs->col, node->childs->value.s, types2[te_temp3] );
										exit(0);
									}
								}
								
							}else{
								printf("Line %d, col %d: Incompatible type in assigment to %s (got %s, expected _real_)\n", node->childs->next->line, node->childs->next->col, node->childs->value.s, types2[te_temp2] );
								exit(0);
							}
						}else if(te_temp==_integer_ && te_temp2!=_integer_){
							if(te_temp2==_function_){
								if(node->childs->next->type == Call){
									te_temp3 = check_if_is_function(node->childs->next->childs,env);
									
									s=(char*)strdup(node->childs->next->childs->value.s);
									lower(s);
									if (strcmp(env->locals->name,s)==0){
										check_number_of_arguments(node->childs->next->childs,env);
									}else if(strcmp(env->name,"Program")==0){
										check_number_of_arguments(node->childs->next->childs,env);
									}else{
										check_number_of_arguments(node->childs->next->childs,previous);
									}
									if (te_temp3!=_integer_)
									{
										printf("Line %d, col %d: Incompatible type in assigment to %s (got %s, expected _integer_)\n", node->childs->next->childs->line, node->childs->next->childs->col, node->childs->value.s, types2[te_temp3] );
										exit(0);
									}
								}else{
									te_temp3 = check_if_is_function(node->childs->next,env);
									
									s=(char*)strdup(node->childs->next->value.s);
									lower(s);
									if (strcmp(env->locals->name,s)==0){
										check_number_of_arguments(node->childs->next,env);
									}else if(strcmp(env->name,"Program")==0){
										check_number_of_arguments(node->childs->next,env);
									}else{
										check_number_of_arguments(node->childs->next,previous);
									}
									if (te_temp3!=_integer_)
									{
										printf("Line %d, col %d: Incompatible type in assigment to %s (got %s, expected _integer_)\n", node->childs->next->childs->line, node->childs->next->childs->col, node->childs->value.s, types2[te_temp3] );
										exit(0);
									}
								}
								
							}else{
								printf("Line %d, col %d: Incompatible type in assigment to %s (got %s, expected _integer_)\n", node->childs->next->line, node->childs->next->col, node->childs->value.s, types2[te_temp2] );
								exit(0);
							}
						}
						if (te_temp==_boolean_ && te_temp2!=_boolean_){
							if(te_temp2==_function_){
								if(node->childs->next->type == Call){
									te_temp3 = check_if_is_function(node->childs->next->childs,env);
									
									s=(char*)strdup(node->childs->next->childs->value.s);
									lower(s);
									if (strcmp(env->locals->name,s)==0){
										check_number_of_arguments(node->childs->next->childs,env);
									}else if(strcmp(env->name,"Program")==0){
										check_number_of_arguments(node->childs->next->childs,env);
									}else{
										check_number_of_arguments(node->childs->next->childs,previous);
									}
									if (te_temp3!=_boolean_)
									{
										printf("Line %d, col %d: Incompatible type in assigment to %s (got %s, expected _boolean_)\n", node->childs->next->childs->line, node->childs->next->childs->col, node->childs->value.s, types2[te_temp3] );
										exit(0);
									}
								}else{
									te_temp3 = check_if_is_function(node->childs->next,env);
									
									s=(char*)strdup(node->childs->next->value.s);
									lower(s);
									if (strcmp(env->locals->name,s)==0){
										check_number_of_arguments(node->childs->next,env);
									}else if(strcmp(env->name,"Program")==0){
										check_number_of_arguments(node->childs->next,env);
									}else{
										check_number_of_arguments(node->childs->next,previous);
									}
									if (te_temp3!=_boolean_)
									{
										printf("Line %d, col %d: Incompatible type in assigment to %s (got %s, expected _boolean_)\n", node->childs->next->childs->line, node->childs->next->childs->col, node->childs->value.s, types2[te_temp3] );
										exit(0);
									}
								}
								
							}else{
								printf("Line %d, col %d: Incompatible type in assigment to %s (got %s, expected _boolean_)\n", node->childs->next->line, node->childs->next->col, node->childs->value.s, types2[te_temp2] );
								exit(0);
							}

			
						}
						break;
		
		case WriteLn:	;
						aux=node->childs;
						while(aux!=NULL){
							te_temp = check_expressions(aux,env);
							if(te_temp==_type_){
								printf("Line %d, col %d: Cannot write values of type %s\n",aux->line,aux->col,types2[te_temp]);
								exit(0);
							}
							aux=aux->next;
						}
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
