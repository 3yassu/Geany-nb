#include "json_utils.h"
#include <stdio.h>
#include <stdlib.h>
int save_json(const char *filepath, json_t *json_data) {
    FILE *json_file = fopen(filepath, "w");
    if (!json_file) {
        fprintf(stderr, "Error opening file %s to write\n", filepath);
        return -1;
    }

    char *json_str = json_dumps(json_data, JSON_INDENT(4));
    if (!json_str) {
        fprintf(stderr, "Error converting JSON to string\n");
        fclose(json_file);
        return -1;
    }

    fprintf(json_file, "%s", json_str);
    free(json_str);

    fclose(json_file);
    return 0;
}

json_t *load_json(const char *filepath){
	json_t *root;
	json_error_t error;

	FILE *json_file = fopen(filepath, "r");
	if(!json_file){
		fprintf(stderr, "Error opening %s\n", filepath);
		return NULL;
	}
	
	fseek(json_file, 0, SEEK_END);
	long file_size = ftell(json_file);
	fseek(json_file, 0, SEEK_SET);
	
	char *buffer = (char *)malloc(file_size + 1);
	if(!buffer){
		fprintf(stderr, "Memory allocation error\n");
		fclose(json_file);
		return NULL;
	}
	fread(buffer, 1, file_size, json_file);
	buffer[file_size] = '\0';
	fclose(json_file);
	root = json_loads(buffer, 0, &error);
	if(!root){
		fprintf(stderr, "Error parsing JSON: %s\n", error.text);
		return NULL;
	}
	free(buffer);
	return root;
}
