#include <stddef.h>
#include <stdint.h>

#ifndef _C_STRING_H_
#define _C_STRING_H_

#define STRING_NPOS -1

typedef char* string_t;
struct __string_header;

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
string_t __string_allocate(const size_t n);
string_t __string_realloc_if_need(string_t* ptr, const size_t _size, const size_t __size, const size_t new_size);
void string_free(string_t* string);

string_t string_reserve(string_t* ptr, const size_t n);

string_t string_init(const char* c_string);
string_t string_clone(const string_t string);
string_t string_substring_create(const string_t str, const size_t init_pos, const size_t end_pos);

extern size_t string_length(const string_t string);
extern size_t string_allocated_size(const string_t string);

extern int string_compare(const string_t str1, const string_t str2);
extern int string_c_compare(const string_t str1, const char* str2);

string_t string_copy(string_t* dst, const string_t src);
string_t string_copy_n(string_t* dst, const string_t src, const size_t n);
string_t string_c_copy(string_t* dst, const char* src);
string_t string_c_copy_n(string_t* dst, const char* src, const size_t n);

string_t string_move(string_t* dst, string_t* src);
string_t string_swap(string_t* str1, string_t* str2);

string_t string_concat(string_t* dst, const string_t src);
string_t string_concat_n(string_t* dst, const string_t src, const size_t n);
string_t string_c_concat(string_t* dst, const char* src);
string_t string_c_concat_n(string_t* dst, const char* src, const size_t n);

extern long string_char_pos(const string_t string, const char ch);
extern char* string_char_ptr(const string_t string, const char ch);

extern int string_contains(const string_t string, const char ch);
extern int string_char_at(const string_t string, const size_t index);
extern char* string_ptr_at(const string_t string, const size_t index);

extern long string_substring_pos(const string_t string, const char* pattern);
extern char* string_substring_ptr(const string_t string, const char* pattern);
#ifdef __cplusplus
}
#endif // __cplusplus

#endif // _C_STRING_H_
