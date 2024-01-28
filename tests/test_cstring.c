#include "./include/cstring.h"

int main()
{
        struct cstring string = cstring_is("Hello, 世界!");
        printf("%s\n", cstring_as_cstr(&string));
}