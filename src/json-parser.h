/*
 *  json-parser.h
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
#ifndef NOTEBOOK_JSON_PARSER_H
#define NOTEBOOK_JSON_PARSER_H
//#include "Scintilla.h"
//#include "ScintillaWidget.h"
#include <yyjson.h>
#include <stdbool.h>

typedef struct Code{
	yyjson_mut_val *src;
}Code;

typedef struct Mkdn{
	yyjson_mut_val *src;
}Mkdn;

typedef enum BlockType{
	CELL;
	MKDN;
}BlockType;

typedef union Block{
	Code code;
	Mkdn mkdn;
}Block;

typedef struct Cell{
	const char *key;
	Block entry;
	BlockType type;
}Cell;

typedef struct Notebook{
  int nbformat_major, nbformat_minor; //might make yyjson_mut_val
  yyjson_mut_doc *src;
  yyjson_mut_val *root;
  Cell *cells;
  size_t length;
  size_t capacity;
} Notebook;

Notebook *notebook_create(char *file);
void cell_create(BlockType type, Notebook *notebook);
void cells_drop(Cell *cell, size_t length);
void notebook_drop(Notebook *notebook);

#endif
