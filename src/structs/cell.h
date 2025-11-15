/*
 *  cell.h
 *  Copyright 2025 Eyassu Mongalo <3yassu@gmail.com>
 */

#ifndef NOTEBOOK_CELL_H
#define NOTEBOOK_CELL_H
#include <yyjson.h>
#include <stdbool.h>

typedef enum BlockType{
	CODE,
	MKDN,
}BlockType;

//typedef union Block{
//	Code code;
//	Mkdn mkdn;
//}Block;

typedef struct Block{
	yyjson_mut_val *src;
}Block;

typedef struct Cell{
	bool valid;
	const char *key;
	Block entry;
	BlockType type;
	//int start, end; //start and end lines to keep track of different cells?
}Cell;

#endif
