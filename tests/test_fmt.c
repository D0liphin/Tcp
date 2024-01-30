#include "../include/dynarray.h"
#include "../include/slice.h"
#include "../include/type.h"
#include "../include/cstring.h"
#include "../include/fmt.h"
#include "../include/assert.h"

static void int_dynarray_formats()
{
        struct dynarray arr = DYNARRAY_IS(int, 1, 2, 3);

        struct cstring my_string = cstring_new();
        fmt_dynarray(&my_string, &arr, TYPEINFO(int), (formatter)fmt_int);
        ASSERT(cstr_eq(cstring_as_cstr(&my_string), "{ 1, 2, 3 }"));
        cstring_free(&my_string);

        dynarray_free(&arr);
}

int main()
{
        RUNTEST(int_dynarray_formats);
}
