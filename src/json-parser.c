#include <stdlib.h>
#include "json-parser.h"
#include <jansson.h>
//Going to use jansson.h for parsing


void add_cell(Notebook *notebook, Cell *cell);

Notebook *notebook_create(char *file){

} 

void cell_create(BlockType type, Notebook *notebook){
	
} 

void add_cell(Notebook *notebook, Cell *cell){
	//Generate Metadata (Specifically ID), and add to notebook create;
	//Notebook may become a Linked List if Cell struct becomes too large.

}
