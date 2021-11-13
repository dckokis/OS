#pragma once

typedef struct {
    size_t *storage;
    size_t amountOfElems;
    size_t capacity;
} smartArray;

smartArray *init(size_t size) {
    smartArray *block = (smartArray *) malloc(sizeof(block));
    if (block == NULL)
        return block;
    block->storage = (size_t *) malloc(size * sizeof(size_t));
    if (block->storage == NULL) {
        free(block);
        return NULL;
    }
    block->amountOfElems = 0;
    block->capacity = size;
    return block;
}

bool add(smartArray *vec, size_t elem) {
    if (vec->amountOfElems == vec->capacity) {
        vec->capacity *= 2;
        size_t *data = (size_t *) realloc(vec->storage, vec->capacity * sizeof(size_t));
        if (data == NULL) {
            vec->capacity /= 2;
            return false;
        }
        vec->storage = data;
    }
    vec->storage[vec->amountOfElems++] = elem;
    return true;
}

void destroy(smartArray *vec) {
    free(vec->storage);
    vec->storage = NULL;
    vec->amountOfElems = vec->capacity = 0;
}