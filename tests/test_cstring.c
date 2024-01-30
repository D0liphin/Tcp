#include "./include/cstring.h"
#include "./include/assert.h"

static void cstring_extends()
{
        struct cstring string = cstring_is("Hello, ");
        cstring_extend_cstr(&string, "世界!");
        struct cstring should_be = cstring_is("Hello, 世界!");
        ASSERT(cstring_eq(&string, &should_be));
        cstring_free(&string);
        cstring_free(&should_be);
}

static void cstring_pushes()
{
        struct cstring string = cstring_is("Hello, ");
        cstring_push(&string, CODEPOINT(L'世'));
        cstring_push(&string, CODEPOINT(L'界'));
        cstring_push(&string, CODEPOINT(L'!'));
        ASSERT(cstr_eq(cstring_as_cstr(&string), "Hello, 世界!"));
        cstring_free(&string);
}

int main()
{
        RUNTEST(cstring_extends);
        RUNTEST(cstring_pushes);
        return 0;
}