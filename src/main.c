#include "./include/dynarray.h"
#include "./include/slice.h"
#include "./include/type.h"

int main()
{
        struct dynarray arr = dynarray_new();
        for (int i = 0; i < 100000; ++i) {
                *(int *)dynarray_next(&arr, TYPEINFO(int)) = i;
        }
        struct slice arr_slice = dynarray_as_slice(&arr);

        struct dynarray arr_copy = dynarray_new();
        dynarray_extend(&arr_copy, slice_begin(&arr_slice),
                        slice_end(&arr_slice));

        struct slice arr_copy_slice = dynarray_as_slice(&arr_copy);
        for (size_t i = 2345; i < 2356; ++i) {
                printf("%zu ", i);
        }
        puts("");

        dynarray_free(&arr_copy);
        dynarray_free(&arr);
}