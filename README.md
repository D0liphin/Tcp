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
