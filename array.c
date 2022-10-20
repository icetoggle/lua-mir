#include "array.h"
#include <stdlib.h>

void array_init(Array *arr, int capacity)
{
    arr->data = malloc(capacity * sizeof(int));
    arr->capacity = capacity;
    arr->size = 0;
}

void array_free(Array *array)
{
    free(array->data);
    array->size = 0;
    array->capacity = 0;
}

void array_append(Array *array, int value)
{
    if(array->size == array->capacity) {
        array->capacity *= 2;
        array->data = realloc(array->data, array->capacity * sizeof(int));
    }
    array->data[array->size++] = value;
}

int array_exist(Array *array, int value)
{
    for(int i = 0; i < array->size; i++) {
        if(array->data[i] == value) {
            return 1;
        }
    }
    return 0;
}

