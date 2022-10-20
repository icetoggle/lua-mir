#ifndef ARRAY_H
#define ARRAY_H

typedef struct {
    int *data;
    int size;
    int capacity;
} Array;

void array_init(Array *arr, int capacity);
void array_free(Array *arr);
void array_append(Array *arr, int value);
int array_exist(Array *arr, int value);
#endif
