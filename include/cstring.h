#include <stdint.h>
#include <stdbool.h>

#include "./dynarray.h"
#include "./slice.h"

/**
 * Represents a unicode codepoint. This is a typedef for a valid reason -- we 
 * want lints when you try and use a `uint32_t` as this value as it is probably
 * an accident.
 */
typedef struct {
        uint32_t value;
} codepoint;

/**
 * An allocated utf-8 string. This string also has the 'benefit' of always 
 * ending in a null-byte. If either of these properties are undesirable, you
 * should just use a `struct dynarray`.
 * 
 * Common convenience functions are ommitted on purpose, to indicate their 
 * performance cost. For example `.count()` is not present. This should be done
 * by creating a `codepoints` iterator and incrementing a counter yourself. 
 * If you don't want to do all that, you should probably just use a `struct
 * dynarray`.
 * 
 * Some terminology that is used throughout:
 * 
 * - `cstring`: An owned (i.e. heap-allocated) `cstr`.
 * - `cstr`: A null-terminated `char *`. A C string literal is of this type.
 * - `str`: A slice of bytes. This is valid utf8.
 * 
 * # Example
 * Print out a greeting from the user.
 * 
 * ```c
 * struct cstring greeting = cstring_is("user says: ");
 * stdin_readline(&greeting);
 * printf("%s", cstring_as_cstr(greeting));
 * cstring_free(&greeting);
 * ```
 *
 * # Why?
 * Lots of libraries expect ascii, null-terminated strings. This satisfies that 
 * requirement, by allowing you to interpret this as such. Furthermore, it is
 * quite helpful to indicate that something is (or, well, we're writing C, so
 * 'should be') utf8 i.e. "printable".
 */
struct cstring {
        /** 
         * Internal type `char` (or `unsigned char` etc.) This is not to be 
         * confused with `character`.
         */
        struct dynarray buf;
};

/**
 * A valid utf8 slice. Roughly the same as `*mut str`. See the rust docs for
 * more information.
 */
struct str {
        /** 
         * Internal type `char` (or `unsigned char` etc.) This is not to be 
         * confused with `character`.
         */
        struct slice sl;
};

/**
 * An iterator over the codepoints in a `struct cstring`. The full iterator 
 * suite is comprised of `*_codepoints()` `codepoints_has_next()` and 
 * `codepoints_next()`. 
 * 
 * This iterator is O(n) over the bytes. 
 */
struct codepoints {
        /**
         * The slice we are iterating over. This slice should update the 
         * `begin` field as the iteration progresses. Slice has internal type
         * `codepoint`.
         */
        struct slice iter;
};

/**
 * Construct a new, empty `cstring`. No allocation is performed
 */
struct cstring cstring_new();

/**
 * Free this `cstring`, invalidating it for any future use.
 */
void cstring_free(struct cstring *self);

/**
 * Equivalent to creating an empty `cstring` and extending it with the contents
 * of the supplied cstring. This is very useful for creating owned strings in 
 * a clean way.
 * 
 * ```c
 * struct cstring greeting = cstring_is("user says: ");
 * stdin_readline(&greeting);
 * printf("%s", cstring_as_cstr(greeting));
 * cstring_free(&greeting);
 * ```
 */
struct cstring cstring_is(char *const cstr);

/** 
 * Encode a `codepoint` as utf8 and append it.
 */
void cstring_push(codepoint ch);

/**
 * Extend by the bytes in a cstr. 
 * 
 * # Safety
 * Must be valid utf8.
 */
void cstring_extend_cstr(char *const cstr);

/**
 * Extend by the bytes in a pointer iterator.
 * 
 * # Safety
 * - Must be valid utf8.
 * - Must be a valid pointer iterator.
 */
void cstring_extend(uint8_t *begin, uint8_t *end);

/**
 * Get the byte at the specified index. This returns an actual byte because 
 * there's really no reason to be modifying this byte directly in most cases.
 * If you want an ascii string, you should use `struct dynarray`.
 */
uint8_t cstring_get(struct cstring const *self, size_t index);

/**
 * Just returns a pointer to the internal buffer, so this is really quick. It's
 * guaranteed to end in a null-byte, so this can be used to interface with 
 * functions that expect a cstr.
 */
char const *cstring_as_cstr(struct cstring const *self);

/**
 * Get a `struct str` representing this cstring. The returned `str` omits the 
 * final null-byte.
 */
struct str cstring_as_str(struct cstring const *self);

/**
 * Get an iterator over the codepoints in this cstring.
 */
struct codepoints cstring_codepoints(struct cstring const *self);

/**
 * Return an iterator to the beginning of this slice. The returned pointer is
 * valid for the inner type.
 */
void *str_begin(struct str *self);

/**
 * A pointer one past the end of this iterator.
 */
void *str_end(struct str *self);

/**
 * Bounds-checked str element access of the n-th byte. 
 */
uint8_t str_get(struct str *self, size_t index);

/**
 * The length of this str in bytes. If you want the count, see 
 * `struct codepoints`.
 */
static inline size_t str_length(struct str const *self, struct type val_type);

/**
 * Construct a str with a beginning and an end. 
 */
struct str str_new(uint8_t *begin, uint8_t *end);

/**
 * Get an iterator over the codepoints in a str.
 */
struct codepoints str_codepoints(struct str const *self);

/**
 * Advance this iterator. Note that calls to this are undefined if 
 * `codepoints_has_next(self)` returns `false`.
 */
codepoint codepoints_next(struct codepoints *self);

/**
 * Check if a call to `codepoints_next` is valid.
 */
bool codepoints_has_next(struct codepoints *self);