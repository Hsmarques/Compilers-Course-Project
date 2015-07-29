#include "symbol_table.h"

environment_list* semantic_analysis(Node *myprogram);

void print_semantic(environment_list* env);

table_element *insert_line(table_element *initial, char* name, te_type type, char* flag, te_type value);

environment_list* insert_table(Node* node, environment_list* env, environment_list* previous, environment_list *outter_table);


