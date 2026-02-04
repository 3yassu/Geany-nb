#include <stdlib.h>
#include "notebook.h"
#include "cell.h"
#define New(T) (T *)malloc(sizeof(T))
void notebook_push(Notebook *self, Cell val);

struct Notebook {
	size_t len, cap, cur_index;
	Cell *ptr;
	yyjson_mut_doc *src;
	yyjson_mut_val *cells;
};

Notebook *notebook_new(){
	Notebook *ptr = New(Notebook);
	if(!ptr){
		fprintf(stderr, "Failed to Allocate!");
		return NULL;
	}
	*ptr = (Notebook){.len = 0, .cap = 0, .ptr = NULL, .src = NULL, .cells = NULL};
	return ptr;
}

Notebook *notebook_from_doc(void *doc){
	Notebook *self = notebook_new();
	if(!self)
		return NULL;
	self->src = (yyjson_mut_doc *)doc;
	yyjson_mut_val *root = yyjson_mut_doc_get_root(self->src);
	yyjson_mut_val *arr = yyjson_mut_obj_get(root, "cells");
	if(arr){
		size_t idx, max;
		yyjson_mut_val *elem;
		yyjson_mut_arr_foreach(arr, idx, max, elem){
			enum BlockType type = strcmp(yyjson_mut_get_str(yyjson_mut_obj_get(elem, "cell_type")), "code") ? CODE : MKDN;
			notebook_push(self, (Cell){.type = type, .src = elem});
		}
	}
	self->cells = arr;
	return self;
}

const char *notebook_get_doc(Notebook *self){
	if(self)
		return (const char *)yyjson_mut_write(self->src, YYJSON_WRITE_PRETTY, NULL);
	return NULL;
}

Notebook *notebook_from_str(char *buf, size_t len){
	if(!buf)
		return NULL;
	Notebook *self = notebook_new();
	if(!self)
		return NULL;
	yyjson_doc *idoc = yyjson_read(buf, len, 0);
	free(buf);
	if(!idoc){
		notebook_free(self);
		return NULL;
	}
	self->src = yyjson_doc_mut_copy(idoc, NULL);
	yyjson_doc_free(idoc);
	if(!self->src){
		notebook_free(self);
		return NULL;
	}
	yyjson_mut_val *root = yyjson_mut_doc_get_root(self->src);
	yyjson_mut_val *arr = yyjson_mut_obj_get(root, "cells");
	if(arr){
		size_t idx, max;
		yyjson_mut_val *elem;
		yyjson_mut_arr_foreach(arr, idx, max, elem){
			enum BlockType type = strcmp(yyjson_mut_get_str(yyjson_mut_obj_get(elem, "cell_type")), "code") ? CODE : MKDN;
			notebook_push(self, (Cell){.type = type, .src = elem});
		}
	}
	self->cells = arr;
	return self;
}

Cell *notebook_ptr(Notebook *self){
	if(self)
		return self->ptr;
	return NULL;
}

size_t notebook_len(Notebook *self){
	if(self)
		return self->len;
	return 0;
}
size_t notebook_get_cur_ind(Notebook *self){
	if(self)
		return self->cur_index;
	return 0;
}

void notebook_grow(Notebook *self){
	size_t new_cap = self->cap ? 2 * self->cap : 1;
	if(new_cap > ((size_t)-1 >> 1)){
		fprintf(stderr, "Allocation Too Large!");
		return;
	}
	self->ptr = self->cap ? realloc(self->ptr, new_cap * sizeof(Cell)) : New(Cell);
	if(!self->ptr) {
		fprintf(stderr, "Failed to Allocate!\n");
		return;
	}
	self->cap = new_cap;
}

void notebook_push(Notebook *self, Cell val){
	if(self){
		if(self->len == self->cap)
			notebook_grow(self);
		self->ptr[self->len++] = val;
	}
}

void notebook_push_new(Notebook *self, enum BlockType type){
	if(self){
		if(self->len == self->cap)
			notebook_grow(self);
		self->ptr[self->len++] = (Cell){.type = type, .src = yyjson_mut_arr_add_obj(self->src, self->cells)};
	}
}

const char **notebook_get_text(Notebook *self, size_t index){
	if(index >= self->len)
		return NULL;
	yyjson_mut_val *arr = yyjson_mut_obj_get(self->ptr[index].src, "source");
	size_t size = yyjson_mut_arr_size(arr);
	const char **bufs = (const char **)calloc(size + 1, sizeof(const char *));
	if(!bufs)
		return NULL;
	size_t idx;
	yyjson_mut_val *elem;
	for(idx = 1, elem = yyjson_mut_arr_get_first(arr); idx <= size; idx++, elem = elem->next){
		bufs[size-idx] = yyjson_mut_get_str(elem);
	}
	self->cur_index = index;
	return bufs;
}

void notebook_set_text(Notebook *self, const char *buf, size_t len){
	if(!self)
		return;
	yyjson_mut_val *arr = yyjson_mut_obj_get(self->ptr[self->cur_index].src, "source");
	yyjson_mut_arr_clear(arr);
	
	char *ptr = (char *)malloc(len);
	strcpy(ptr, buf);
	for(char *i = ptr, *j = ptr; (i = strchr(++i, '\n'));){
		yyjson_mut_arr_add_strncpy(self->src, arr, buf + (j - ptr), i - j);
		j = i;
	}
	free(ptr);
}

void notebook_run_cell(Notebook *self, size_t index){
	if(!self)
		return;
	if(index >= self->len){
		fprintf(stderr, "Index Error");
		return;
	} 	
}

void notebook_free(Notebook *self){
	if(self){
		if(self->ptr)
			free(self->ptr);
		yyjson_mut_doc_free(self->src);
		free(self);
	}
}
