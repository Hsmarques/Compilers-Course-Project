#ifndef _SYMBOL_TABLE_
#define _SYMBOL_TABLE_

#include "functions.h"


typedef struct _t1{	
	char* name;
	te_type type;
	char* flag;
	te_type value;
	struct _t1 *next;
	struct _t2 *header;
} table_element;



typedef struct _t2{
	int defined;
	int number_of_arguments;
	char* name;
	struct _t2 *previous;
	table_element *locals;
}environment_list;

#endif

