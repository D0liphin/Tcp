#include "../include/dynarray.h"

int main()
{
        struct dynarray arr = dynarray_new();
        int values[10] = { 1, 2, 4, 4, 5, 6, 7, 8, 9, 10 };
        dynarray_extend(&arr, &values[0], &values[10]);
        for (int *it = dynarray_begin(&arr); it != dynarray_end(&arr); ++it) {
                printf("%d ", *it);
        }
        puts("");
        for (size_t _ = 0; _ < 10; ++_) {
                int n = *(int *)dynarray_pop(&arr, TYPEINFO(int));
                printf("%d ", n);
        }
        puts("");
        dynarray_free(&arr);
        return 0;
}