#include <stdio.h>
#include <stdlib.h>
#include <yyjson.h>

int main(int argc, char *argv[]){
	yyjson_read_err r_err;
	yyjson_doc *doc = yyjson_read_file("test.json", 0, NULL, &r_err);
	if(!doc)
		fprintf(stderr, "read error: %s, code: %u at byte position: %lu\n", r_err.msg, r_err.code, r_err.pos);
		
	yyjson_mut_doc *mut_doc = yyjson_doc_mut_copy(doc, NULL);
	yyjson_doc_free(doc);
	
	yyjson_mut_val *root = yyjson_mut_doc_get_root(mut_doc);
	yyjson_mut_set_int(yyjson_mut_obj_get(root, "nbformat"), argc);
	
	yyjson_write_err w_err;
	bool pass = yyjson_mut_write_file("test.json", mut_doc, YYJSON_WRITE_PRETTY, NULL, &w_err);
	if(!pass)
		fprintf(stderr, "err: %u msg:%s\n", w_err.code, w_err.msg);
		
	yyjson_mut_doc_free(mut_doc);
	printf("Hello, Geany %s\n", argv[0]); //hello geany :)
	return 0;
}
