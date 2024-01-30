#include "./include/dynarray.h"
#include "./include/slice.h"
#include "./include/type.h"
#include "./include/cstring.h"
#include "./include/fmt.h"

typedef void (*formatter)(struct cstring *restrict, void const *);

int main()
{
        struct dynarray arr = DYNARRAY_IS(int, 1, 2, 3);

        struct cstring my_string = cstring_is("myarray = ");
        fmt_dynarray(&my_string, &arr, TYPEINFO(int), (formatter)fmt_int);
        puts(cstring_as_cstr(&my_string));
        cstring_free(&my_string);

        dynarray_free(&arr);
}
