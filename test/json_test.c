#include <stdio.h>
#include <stdlib.h>
#include "json_utils.h"

int main(int argc, char *argv[]){
	json_t *root = load_json("test.json");
	json_integer_set(json_object_get(root, "nbformat"), argc);
	save_json("test.json", root);
	json_decref(root);
	printf("Hello, Geany %s", argv[0]);
	return 0;
}
