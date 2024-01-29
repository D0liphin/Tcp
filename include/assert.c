#include <stdbool.h>
#include <stdio.h>
#include "./assert.h"

void assert(int line_nr, char const *filename, char const *assert_str, bool condition)
{
        if (!condition) {
                panic(line_nr, filename, "assertion failed: %s", assert_str);
        }
}

void run_test(char const *test_name, void (*testfn)())
{
        testfn();
        printf("%s: \e[0;32msuccess!\e[0m\n", test_name);
}