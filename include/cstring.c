#include "./cstring.h"
#include "./dynarray.h"
#include "./type.h"
#include "./panic.h"

struct cstring cstring_new()
{
        struct cstring s;
        s.buf = dynarray_new();
        *(uint8_t *)dynarray_next(&s.buf, TYPEINFO(uint8_t)) = 0x00;
        return s;
}

void cstring_free(struct cstring *restrict self)
{
        dynarray_free(&self->buf);
}

struct cstring cstring_is(char const *cstr)
{
        struct cstring s;
        s.buf = dynarray_new();
        cstring_extend_cstr(&s.buf, cstr);
        return s;
}

/** max value of utf8 codepoint */
#define UTF8MAX 0x10ffff
#define UTF8MAX1 0x80
#define UTF8MAX2 0x800
#define UTF8MAX3 0x10000

/**
 * Convert a codepoint to its utf8 encoding, returns the number of bytes in 
 * the encoding.
 */
size_t encode_utf8(uint8_t out[4], codepoint ch)
{
        uint32_t i = codepoint_as_uint32(ch);
        if (i < UTF8MAX1) {
                out[0] = (int8_t)i;
                return 1;
        } else if (i < UTF8MAX2) {
                out[0] = (uint8_t)(0b11000000 | (i >> 6));       // first 5 bits
                out[1] = (uint8_t)(0b10000000 | (i & 0b111111)); // last 6 bits
                return 2;
        } else if (i < UTF8MAX3) {
                out[0] = (uint8_t)(0b11100000 | (i >> 12));             // first 4 bits
                out[1] = (uint8_t)(0b10000000 | ((i >> 6) & 0b111111)); // penultimate 6-bit chunk
                out[2] = (uint8_t)(0b10000000 | (i & 0b111111));        // last 6 bits
                return 3;
        } else {
                out[0] = (uint8_t)(0b11110000 | (i >> 18));              // first 3 bits
                out[1] = (uint8_t)(0b10000000 | ((i >> 12) & 0b111111)); // penultimate 6-bit chunk
                out[2] = (uint8_t)(0b10000000 | ((i >> 6) & 0b111111));  // penultimate 6-bit chunk
                out[3] = (uint8_t)(0b10000000 | (i & 0b111111));         // last 6 bits
                return 4;
        }
}

void cstring_push(struct cstring *restrict self, codepoint ch)
{
        uint8_t out[4];
        size_t encoding_length = encode_utf8(out, ch);
        dynarray_pop(self, TYPEINFO(uint8_t));
        for (size_t i = 0; i < encoding_length; ++i) {
                *(uint8_t *)dynarray_next(&self->buf, TYPEINFO(uint8_t)) = out[i];
        }
        *(uint8_t *)dynarray_next(&self->buf, TYPEINFO(uint8_t)) = 0x00;
}

void cstring_extend_cstr(struct cstring *restrict self, char *const cstr)
{
        char *end = cstr + strlen(cstr) + 1;
        cstring_extend(self, cstr, end);
}

void cstring_extend(struct cstring *restrict self, uint8_t *begin, uint8_t *end)
{
        dynarray_pop(self, TYPEINFO(uint8_t));
        dynarray_extend(&self->buf, (void *)begin, (void *)end);
        *(uint8_t *)dynarray_next(&self->buf, TYPEINFO(uint8_t)) = 0x00;
}

uint8_t cstring_get(struct cstring const *self, size_t index)
{
        return str_get(cstring_as_str(self), index);
}

char const *cstring_as_cstr(struct cstring const *self)
{
        return dynarray_begin(&self->buf);
}

str cstring_as_str(struct cstring const *self)
{
        // ignores null-terminator
        return str_new(dynarray_begin(&self->buf), dynarray_end(&self->buf) - 1);
}

struct codepoints cstring_codepoints(struct cstring const *self)
{
        TODO();
}

uint8_t *str_begin(str self)
{
        return self.sl.begin;
}

uint8_t *str_end(str self)
{
        return self.sl.end;
}

uint8_t str_get(str self, size_t index)
{
        return *(uint8_t *)slice_get(self.sl, TYPEINFO(uint8_t), index);
}

/**
 * Construct a str with a beginning and an end. Returns an empty slice if the
 * provided buffer is not utf8.
 */
str str_new(uint8_t *begin, uint8_t *end)
{
        TODO();
}

/**
 * Get an iterator over the codepoints in a str.
 */
struct codepoints str_codepoints(str const self)
{
        TODO();
}

/**
 * Advance this iterator. Note that calls to this are undefined if 
 * `codepoints_has_next(self)` returns `false`.
 */
codepoint codepoints_next(struct codepoints *self)
{
        TODO();
}

/**
 * Check if a call to `codepoints_next` is valid.
 */
bool codepoints_has_next(struct codepoints *self)
{
        TODO();
}