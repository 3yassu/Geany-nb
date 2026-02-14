/*
 *  notebook.c
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
#include "notebook.h"
#include "../interpreter/python.h"
#include "cell.h"
#include <stdlib.h>

#define New(T) (T *)malloc(sizeof(T))
static void notebook_push(Notebook *self, Cell val);

struct Notebook {
  size_t len, cap, cur_index;
  Cell *ptr;
  yyjson_mut_doc *src;
  yyjson_mut_val *cells;
};

Notebook *notebook_new() {
  Notebook *ptr = New(Notebook);
  if (!ptr) {
    fprintf(stderr, "Failed to Allocate!");
    return NULL;
  }
  *ptr =
      (Notebook){.len = 0, .cap = 0, .ptr = NULL, .src = NULL, .cells = NULL};
  ipython_init();
  return ptr;
}

char *notebook_get_doc(Notebook *self) {
  if (self)
    return (char *)yyjson_mut_write(self->src, YYJSON_WRITE_PRETTY, NULL);
  return NULL;
}

Notebook *notebook_from_str(char *buf, size_t len) {
  if (!buf)
    return NULL;
  Notebook *self = notebook_new();
  if (!self)
    return NULL;
  yyjson_doc *idoc = yyjson_read(buf, len, 0);
  free(buf);
  if (!idoc) {
    notebook_free(self);
    return NULL;
  }
  self->src = yyjson_doc_mut_copy(idoc, NULL);
  yyjson_doc_free(idoc);
  if (!self->src) {
    notebook_free(self);
    return NULL;
  }
  yyjson_mut_val *root = yyjson_mut_doc_get_root(self->src);
  yyjson_mut_val *arr = yyjson_mut_obj_get(root, "cells");
  if (arr) {
    size_t idx, max;
    yyjson_mut_val *elem;
    yyjson_mut_arr_foreach(arr, idx, max, elem) {
      enum BlockType type =
          strcmp(yyjson_mut_get_str(yyjson_mut_obj_get(elem, "cell_type")),
                 "code")
              ? MKDN
              : CODE;
      notebook_push(self, (Cell){.type = type, .src = elem});
    }
  }
  self->cells = arr;
  return self;
}

Cell *notebook_ptr(Notebook *self) {
  if (self)
    return self->ptr;
  return NULL;
}

size_t notebook_len(Notebook *self) {
  if (self)
    return self->len;
  return 0;
}
size_t notebook_get_cur_ind(Notebook *self) {
  if (self)
    return self->cur_index;
  return 0;
}

void notebook_grow(Notebook *self) {
  size_t new_cap = self->cap ? 2 * self->cap : 1;
  if (new_cap > ((size_t)-1 >> 1)) {
    fprintf(stderr, "Allocation Too Large!");
    return;
  }
  self->ptr =
      self->cap ? realloc(self->ptr, new_cap * sizeof(Cell)) : New(Cell);
  if (!self->ptr) {
    fprintf(stderr, "Failed to Allocate!\n");
    return;
  }
  self->cap = new_cap;
}

static void notebook_push(Notebook *self, Cell val) {
  if (self) {
    if (self->len == self->cap)
      notebook_grow(self);
    self->ptr[self->len++] = val;
  }
}

void notebook_push_new(Notebook *self, enum BlockType type) {
  if (self) {
    if (self->len == self->cap)
      notebook_grow(self);
    self->ptr[self->len++] = (Cell){
        .type = type, .src = yyjson_mut_arr_add_obj(self->src, self->cells)};
  }
}

char *notebook_get_text(Notebook *self, size_t index) {
  if (index >= self->len)
    return NULL;
  yyjson_mut_val *arr = yyjson_mut_obj_get(self->ptr[index].src, "source");

  yyjson_mut_val *elem;
  size_t idx, max, len = 0;
  yyjson_mut_arr_foreach(arr, idx, max, elem) len += yyjson_mut_get_len(elem);

  char *cell_str = (char *)malloc(len + 1);
  len = 0;
  yyjson_mut_arr_foreach(arr, idx, max, elem) {
    strcpy(cell_str + len, yyjson_mut_get_str(elem));
    len += yyjson_mut_get_len(elem);
  }
  cell_str[len] = '\0';

  self->cur_index = index;
  return cell_str;
}

void notebook_set_text(Notebook *self, const char *buf) {
  if (!self)
    return;
  yyjson_mut_val *arr =
      yyjson_mut_obj_get(self->ptr[self->cur_index].src, "source");
  yyjson_mut_arr_clear(arr);
  for (const char *i = buf, *j = buf; (i = strchr(i, '\n'));) {
    yyjson_mut_arr_add_strncpy(self->src, arr, j, i - j + 1);
    j = ++i;
  }
}

char *notebook_run_cell(Notebook *self, size_t index) {
  if (!self)
    return NULL;
  if (index >= self->len) {
    fprintf(stderr, "Index Error");
    return NULL;
  }
  if (self->ptr[index].type == MKDN) {
    fprintf(stderr, "What the hell");
    return NULL;
  }
  /* Add function notebook_get_py_text(self, index) to run commands */
  char *cell_str = notebook_get_text(self, index);
  if (!cell_str)
    return NULL;
  char *ret = ipython_run_string(cell_str);
  free(cell_str);
  return ret;
}

char *notebook_run_all(Notebook *self) {
  if (!self)
    return NULL;
  for (size_t i = 0; i < self->len; i++) {
    if (self->ptr[i].type == MKDN)
      continue;
    char *cell_str = notebook_get_text(self, i);
    if (!cell_str)
      return NULL;
    int val = ipython_run_keep_io(cell_str);
    free(cell_str);
    if (!val)
      break;
  }
  return ipython_io();
}

void notebook_free(Notebook *self) {
  if (self) {
    if (self->ptr)
      free(self->ptr);
    yyjson_mut_doc_free(self->src);
    free(self);
  }
}
