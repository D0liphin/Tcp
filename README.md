# Some Stuff

This is not really a set project, more just me fiddling around with C. For 
example, this contains the `struct dynarray` 'zero-cost' generic array.

A dynamic, exponentially growing array. Each array has an associate type, for
which the array is 'valid'. The user must keep track of this type for a 
given array, e.g. through hungarian notation. 

This implmentation aims to be just as good as other templated implementations
in regards to codegen, even on `-Oz`. 

Methods require const type information to be passed as a parameter. Since the 
API is designed with the expectation that this type information is passed as 
a constant, codegen is only evaluated for that case. 

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

## Example

The following example demonstrates some basic usage of a dynamic array

```c
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
}
```

This outputs

```
1 2 4 4 5 6 7 8 9 10 
10 9 8 7 6 5 4 4 2 1 
```

## Iterators

It's difficult to create a unified idea of an iterator here. But typically we
should adhere to the following structure for non-pointer iterators.

```c
// houses iterable items of type `my_t`
struct my_iterable { /* some fields */ }

// iterates over values of type `my_t`
struct my_iter { /* some fields */ }

struct my_iter my_iterable_iter();

my_t my_iter_next(struct my_iter *restrict self);

/**
 * Some kind of invalid value for us to compare the iterator against. 
 * Pros: Easy and C++ style
 * Cons: We can't override == so this has to be bitwise and could take a while
 */
struct my_iter my_iterable_end();

/**
 * A function that checks if the iterator is exhausted.
 * Pros: Always fast
 * Cons: Requires implementor to know if there is another value before-hand. 
 * This could involve computing the next value.
 */
bool my_iter_has_next(struct my_iter const *self);
```

## TODO

- Array comparison (and cstring comparison, slice comparison etc.) 
- Extend checked utf8 functions for strings
- Hashmaps (after TCP)
- Deque (after hashmaps)