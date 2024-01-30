# Some Stuff That's in This STL

- `dynarray.h`: a fast dynamic array with 100% less code gen!
- `cstring.h`: a growable utf8 c-string and co.
- `slice.h`: `*mut [T]` (kind of)
- `fmt.h`: a nice library for formatting non-integer things
- `panic.h`: `PANIC()` `TODO()` etc. macros
- `assert.h`: some testing helpers

## Style

I typically conform to the kernel style guide, with a few exceptions that make
my life easier.

1. 8-spaces instead of 8-width tabs
2. Aligned comments
3. Max-width of 100 characters for code and inline comments
4. Max-width of 80 characters otherwise

I also have more relaxed rules about typedefs, namely function signatures can 
be typedefs. There are also two special case 'bad' typedefs in `slice` and 
`str`.
