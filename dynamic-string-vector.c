#include <stddef.h> 
#include <stdlib.h>
#include "dynamic-string-vector.h"

int stringVector_init_by_size(stringVector* v, size_t init_capacity) {
  v->data = (char*) malloc(init_capacity * sizeof(char));
 // printf("OK!\n");
  if (!v->data)
      return -1;
  v->size = 0;
  v->capacity = init_capacity;

  return 0; /* success */
}

int stringVector_init(stringVector* v) {
    return stringVector_init_by_size(v, 1);
}

int vector_resize(stringVector* v, size_t new_size) {
    v->data = (int64_t*) realloc(v->data, new_size*sizeof(int64_t));
    if (v->data == NULL)
        return -1;
    v->capacity = new_size;
    return 0;
}

int vector_double(stringVector* v) {
    return vector_resize(v, 2 * (v->capacity));
}

int vector_div2(stringVector* v) {
    return vector_resize(v, v->capacity / 2);
}

void vector_destroy(stringVector* v) {
    free(v->data);
    v->data=NULL;
    v->size = 0;
    v->capacity = 0;
    free(v);
}

void vector_push_back(stringVector* v, char c) {
    v->size += 1;
    if (v->size > v->capacity)
        vector_double(v);
    v->data[v->size - 1] = c;
}

char vector_get_back(stringVector* v) {
    return v->data[v->size - 1];
}

char vector_pop_back(stringVector* v) {
    char ret = vector_get_back(v);
    v->size -= 1;
    if (v->size < v->capacity / 2)
        vector_div2(v);
    return ret;
}

//for tests
int main() {
    
}
