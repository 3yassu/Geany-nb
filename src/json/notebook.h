#ifndef NOTEBOOK_H
#define NOTEBOOK_H
#include <stddef.h>
typedef struct Notebook Notebook;

enum BlockType{
	CODE,
	MKDN,
};

Notebook *notebook_from_doc(void *doc);
Notebook *notebook_from_str(char *buf, size_t len);
/* Cell *notebook_ptr(Notebook *self); */
char *notebook_get_text(Notebook *self, size_t index);
void notebook_set_text(Notebook *self, const char *buf, size_t len);
char *notebook_get_doc(Notebook *self);
size_t notebook_len(Notebook *self);
size_t notebook_get_cur_ind(Notebook *self);
char *notebook_run_cell(Notebook *self, size_t index);
char *notebook_run_all(Notebook *self);
void notebook_push_new(Notebook *self, enum BlockType type);
void notebook_free(Notebook *self);


#endif
