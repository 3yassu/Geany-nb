#ifndef JSON_UTILS
#define JSON_UTILS

#include <jansson.h>
int save_json(const char *filepath, json_t *json_data);
json_t *load_json(const char *filepath);

#endif
