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
#include <string.h>
typedef struct{
	char *key;
	char *value;
}Dict;

typedef struct Metadata{
	Dict *dict;
}Metadata;

typedef struct Source{
	char **src;
}Source;

typedef struct Code{ //Code
	int execution_count;
	Source src;
	Metadata info;
}Code;

typedef struct Mkdn{ //Markdown
	Source src;
	Metadata info;
}Mkdn;

#endif
