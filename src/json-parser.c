/*
 *  json-parser.c
 *
 *  Copyright 2025 Eyassu Mongalo <3yassu@gmail.com>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "json-parser.h"
//#include "utils.h"
#include <uuid/uuid.h>

#define FNV_OFFSET 14695981039346656037UL //might move to some hashtable.h or something
#define FNV_PRIME 1099511628211UL

const char *cell_id_gen();
size_t notebook_hash(const char *id);
void notebook_double(Notebook *notebook);
void add_cell(Notebook *notebook, BlockType, type, const char *id);

const char *cell_id_gen(){
	uuid_t uuid;
	uuid_generate(uuid);
	char *id = (char *)malloc(9);
	uuid_unparse(uuid, id);
	return id;
}

size_t notebook_hash(const char *id){
	uint16_t hash = FNV_OFFSET;
		for (const char* p = key; *p; p++) {
		hash ^= (uint64_t)(unsigned char)(*p);
		hash *= FNV_PRIME;
	}
	return (size_t)(hash)
}

void notebook_double(Notebook *notebook){
	Cell *new_notebook_cells = (Cell *)calloc(notebook_capacity*2, sizeof(cell));
	for(size_t i == 0; i < notebook_capacity; i++)
		if(notebook->cells[i])
			new_notebook_cells[notebook_hash((notebook->cells)[i]->id)] = (notebook->cells)[i];
	free(notebook->cells);
	notebook->cells = new_notebook_cells;
	notebook->capacity *= 2;
}

void add_cell(Notebook *notebook, BlockType, type, const char *id){
	if(type == CODE){
		yyjson_mut_obj_add_str(notebook->src, new_cell, "cell_type", "code");
		yyjson_mut_obj_add_uint(notebook->src, new_cell, "execution_count", 0);
	}
	else if(type == MKDN)
		yyjson_mut_obj_add_str(notebook->src, new_cell, "cell_type", "markdown");
	//else DO SOME PANIC
	yyjson_mut_val *source = yyjson_mut_obj_add_arr(notebook->src, new_cell, "source");
	yyjson_mut_val *metadata = yyjson_mut_obj_add_obj(notebook->src, new_cell, "metadata");
	yyjson_mut_obj_add_str(notebook->src, metadata, "id", id);
	(notebook->cells)[hash] = (Cell){.key = cell_id, .entry = (Block){source}, .type = type};
	(notebook->length)++;
}


Notebook *notebook_create(char *file, int flag){//will need to change the file pointer
	Notebook *notebook = (Notebook *)malloc(sizeof(Notebook));
	if(!flag){
		*notebook = (Notebook){.nbformat_major = 4,
				.nbformat_minor = 4,
				.src = NULL, .doc = NULL, .cells = NULL,
				.length = 0, .capacity = 0};
		notebook->src = notebook_create_doc(file);//NOT IMPLEMENTED
		//if(!src) PANIC
		notebook->root = yyjson_mut_doc_get_root(notebook->src);
	}
}

void cell_create(BlockType type, Notebook *notebook){
	if(notebook->length >= (notebook->capacity)/(2))
		notebook_double(notebook);
	const char *cell_id = cell_id_gen();
	size_t hash = notebook_hash(cell_id) % notebook->capacity;
	while(notebook->cells[hash]){
		free(cell_id);
		cell_id = cell_id_gen();
		hash = notebook_hash(cell_id) % notebook->capacity; 
	}

	yyjson_mut_val *new_cell = yyjson_mut_arr_add_obj(notebook->src, yyjson_mut_obj_get(root, "cells"));

	notebook_add_cell(notebook, type, cell_id);
}

void cells_drop(Cell *cell, size_t lenght){
	for(size_t i = 0, i < length, i++)
		if(cell+i)
			free(cell[i].key);
}
void notebook_drop(Notebook *notebook){
	cells_drop(notebook->cells, notebook->length);
	yyjson_mut_doc_free(notebook->src);
	free(notebook);
}
