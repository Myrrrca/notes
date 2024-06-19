#ifndef _DYNAMIC_STRING_VECTOR_H
#define _DYNAMIC_STRING_VECTOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>    

typedef struct {
  char* data;
  size_t capacity; /* total capacity */
  size_t size; /* number of elements in vector */
} stringVector;

int stringVector_init(stringVector* v);
int stringVector_init_by_size(stringVector* v, size_t init_capacity);

#ifdef __cplusplus
}
#endif


#endif
