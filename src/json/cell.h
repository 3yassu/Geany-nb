/*
 *  cell.h
 *  Copyright 2025 Eyassu Mongalo <3yassu@gmail.com>
 */

#ifndef NOTEBOOK_CELL_H
#define NOTEBOOK_CELL_H
#include <yyjson.h>

typedef struct Cell{
	enum BlockType type;
	yyjson_mut_val *src;
}Cell;

#endif
