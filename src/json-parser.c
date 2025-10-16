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

//might move to some hashtable.h or something
#define FNV_OFFSET 14695981039346656037UL 
#define FNV_PRIME 1099511628211UL

const char *cell_id_gen();
size_t notebook_hash(const char *id);
void notebook_double(Notebook *notebook);
void notebook_half(Notebook *notebook);
void add_cell(Notebook *notebook, BlockType type);
yyjson_mut_doc *notebook_create_doc(ScintillaObject *sci);

const char *cell_id_gen(){
	uuid_t uuid;
	uuid_generate(uuid);
	char *id = (char *)malloc(13);
	uuid_unparse(uuid, id);
	return id;
}

size_t notebook_hash(const char *id){
	if(!id) return 0;
	uint64_t hash = FNV_OFFSET;
	for (const char* p = id; *p; p++) {
		hash ^= (uint64_t)(unsigned char)(*p);
		hash *= FNV_PRIME;
	}
	return (size_t)(hash);
}

bool notebook_contains(Notebook *notebook, const char *key){
	if(!notebook) return false;
	return notebook->cells[notebook_hash(key) % notebook->capacity].valid;
}

void notebook_double(Notebook *notebook){
	if(!notebook) return;
	size_t new_capacity = notebook->capacity*2;
	Cell *new_notebook_cells = (Cell *)calloc(new_capacity, sizeof(Cell));
	if(notebook->cells)
		for(size_t i = 0; i < notebook->capacity; i++)
			if(notebook->cells[i].valid)
				new_notebook_cells[notebook_hash((notebook->cells)[i].key) % new_capacity] = (notebook->cells)[i];
	free(notebook->cells);
	notebook->cells = new_notebook_cells;
	notebook->capacity = new_capacity;
}

void notebook_half(Notebook *notebook){
	if(!notebook) return;
	size_t new_capacity = notebook->capacity/2;
	//if(new_capacity <= notebook->length) PANIC
	Cell *new_notebook_cells = (Cell *)calloc(new_capacity, sizeof(Cell));
	if(notebook->cells)
		for(size_t i = 0; i < notebook->capacity; i++)
			if(notebook->cells[i].valid)
				new_notebook_cells[notebook_hash(notebook->cells[i].key) % new_capacity] = notebook->cells[i];
	free(notebook->cells);
	notebook->cells = new_notebook_cells;
	notebook->capacity = new_capacity;
}

void add_cell(Notebook *notebook, BlockType type){
	if(!notebook) return;
	if(notebook->length >= (notebook->capacity)/(2))
		notebook_double(notebook);
	const char *cell_id = cell_id_gen();
	while(notebook_contains(notebook, cell_id)){
		free((void *)cell_id);
		cell_id = cell_id_gen();
	}

	yyjson_mut_val *new_cell = yyjson_mut_arr_add_obj(notebook->src, yyjson_mut_obj_get(notebook->root, "cells"));

	cell_create(notebook, type, cell_id, new_cell);
}

yyjson_mut_doc *notebook_create_doc(ScintillaObject *sci){
	if(!sci) return NULL;
	char *buffer = (char *)sci_get_contents(sci, -1);
	if(!buffer)
		return NULL;
	long buffer_size = (long)sci_get_length(sci);
	yyjson_doc *idoc = yyjson_read(buffer, buffer_size, 0);
	free(buffer);
	if(!idoc)
		return NULL;
	yyjson_mut_doc *doc = yyjson_doc_mut_copy(idoc, NULL);
	yyjson_doc_free(idoc);
	if(!doc)
		return NULL;
	return doc;
}
 


Notebook *notebook_create(ScintillaObject *sci, int flag){//flag = 0 means new file, flag = 1 means the file already exists
	Notebook *notebook = (Notebook *)malloc(sizeof(Notebook));
	*notebook = (Notebook){
			.nbformat_major = 4, .nbformat_minor = 4,
			.src = NULL, .root = NULL, .cells = NULL,
			.length = 0, .capacity = 0, .list = NULL};
	if(flag)
		notebook->src = notebook_create_doc(sci);
	if(!notebook->src){
		free(notebook);
		return NULL;
	}
	notebook->root = yyjson_mut_doc_get_root(notebook->src);
	return notebook;
}

void cell_create(Notebook *notebook, BlockType type, const char *id, yyjson_mut_val *new_cell){
	bool pass;
	switch (type) {
		case CODE:
			pass = yyjson_mut_obj_add_str(notebook->src, new_cell, "cell_type", "code");
			pass = yyjson_mut_obj_add_uint(notebook->src, new_cell, "execution_count", 0);
			break;
		case MKDN:
			pass = yyjson_mut_obj_add_str(notebook->src, new_cell, "cell_type", "markdown");
			break;
		default:
			break;//PANIC
	}
	//else DO SOME PANIC
	yyjson_mut_val *source = yyjson_mut_obj_add_arr(notebook->src, new_cell, "source");
	yyjson_mut_val *metadata = yyjson_mut_obj_add_obj(notebook->src, new_cell, "metadata");
	yyjson_mut_obj_add_str(notebook->src, metadata, "id", id);
	size_t hash = notebook_hash(id) % notebook->capacity;
	(notebook->cells)[hash] = (Cell){.valid = true, .key = id, .entry = (Block){new_cell}, .type = type};
	(notebook->length)++;
}

void notebook_load_cell(Notebook *notebook, yyjson_mut_val *cell){
	const char *id = yyjson_mut_get_str(yyjson_mut_obj_get(yyjson_mut_obj_get(cell, "metadata"), "id"));
	size_t hash = notebook_hash(id) % notebook->capacity;
	BlockType type;
	if (strcmp(yyjson_mut_get_str(yyjson_mut_obj_get(cell, "cell_type")), "code"))
		type = CODE;
	else
		type = MKDN;
	notebook->cells[hash] = (Cell){.valid = true, .key = id, .entry = (Block){cell}, .type = type};
	notebook->list[notebook->length] = id;
	(notebook->length)++;
}

bool notebook_load_cells(Notebook *notebook){
	yyjson_mut_val *arr = yyjson_mut_obj_get(notebook->root, "cells");
	if(!arr) return false;
	size_t length = yyjson_mut_arr_size(arr);
	notebook->cells = (Cell *)calloc(length * 4, sizeof(Cell));
	notebook->list = (const char **)malloc(sizeof(const char *) * length * 4);
	if(!notebook->cells) return false;
	notebook->capacity = length * 4;
	size_t idx, max;
	yyjson_mut_val *elem;
	yyjson_mut_arr_foreach(arr, idx, max, elem) {
		notebook_load_cell(notebook, elem);
	}
	return true;
}

void notebook_set_text(Notebook *notebook, ScintillaObject *sci){
	sci_set_text(sci, "\n");
	for(size_t i = 0; i < notebook->capacity; i++)
		if(notebook->cells[i].valid){
			size_t idx, max;
			yyjson_mut_val *elem;
			yyjson_mut_arr_foreach( yyjson_mut_obj_get(notebook->cells[i].entry.src, "source"), idx, max, elem){
				const char *buffer = yyjson_mut_get_str(elem);
				if(!buffer) continue;
				sci_insert_text(sci, -1, buffer);
			}
		}
}

void notebook_drop(Notebook *notebook){
	if(!notebook) return;
	//cells_drop(notebook->cells, notebook->length); keys are managed by the doc
	yyjson_mut_doc_free(notebook->src);
	free(notebook);
}
