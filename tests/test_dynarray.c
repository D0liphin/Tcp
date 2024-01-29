#include "../include/dynarray.h"
#include "../include/assert.h"

void int_dynarray_pushes()
{
        struct dynarray arr = dynarray_new();
        int values[10] = { 1, 2, 4, 4, 5, 6, 7, 8, 9, 10 };
        dynarray_extend(&arr, &values[0], &values[10]);
        ASSERT(*(int *)dynarray_get(&arr, TYPEINFO(int), 4) == 5);
        dynarray_free(&arr);
}

void int_dynarray_pops()
{
        struct dynarray arr = dynarray_new();
        int values[10] = { 1, 2, 4, 4, 5, 6, 7, 8, 9, 10 };
        dynarray_extend(&arr, &values[0], &values[10]);
        ASSERT(*(int *)dynarray_get(&arr, TYPEINFO(int), 4) == 5);
        for (size_t _ = 0; _ < 10; ++_) {
                dynarray_pop(&arr, TYPEINFO(int));
        }
        ASSERT(dynarray_length(&arr, TYPEINFO(int)) == 0);
        dynarray_free(&arr);
}

int main()
{
        RUNTEST(int_dynarray_pushes);
        RUNTEST(int_dynarray_pops);
        return 0;
}