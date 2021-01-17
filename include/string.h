#include <stddef.h>
#include <stdint.h>

#ifndef _C_STRING_H_
#define _C_STRING_H_

#define STRING_NPOS -1

// string type
typedef char* string_t;

struct __string_header;

struct __string_interface
{
    string_t            (*create)           (const char* c_string);
    string_t            (*clone)            (const string_t string);
    string_t            (*reserve)          (string_t* ptr, const size_t n);
    string_t            (*substring_create) (const string_t str, const size_t init_pos, const size_t end_pos);
    void                (*free)             (string_t* string);

    size_t              (*length)           (const string_t string);
    size_t              (*allocated_size)   (const string_t string);

    int                 (*compare)          (const string_t str1, const string_t str2);
    int                 (*c_compare)        (const string_t str1, const char* str2);

    string_t            (*copy)             (string_t* dst, const string_t src);
    string_t            (*copy_n)           (string_t* dst, const string_t src, const size_t n);
    string_t            (*c_copy)           (string_t* dst, const char* src);
    string_t            (*c_copy_n)         (string_t* dst, const char* src, const size_t n);

    string_t            (*move)             (string_t* dst, string_t* src);
    string_t            (*swap)             (string_t* str1, string_t* str2);
    string_t            (*reverse)          (string_t string);

    string_t            (*concat)           (string_t* dst, const string_t src);
    string_t            (*concat_n)         (string_t* dst, const string_t src, const size_t n);
    string_t            (*c_concat)         (string_t* dst, const char* src);
    string_t            (*c_concat_n)       (string_t* dst, const char* src, const size_t n);

    long                (*char_pos)         (const string_t string, const char ch);
    char*               (*char_ptr)         (const string_t string, const char ch);

    int                 (*char_at)          (const string_t string, const size_t index);
    char*               (*ptr_at)           (const string_t string, const size_t index);

    long                (*substring_pos)    (const string_t string, const char* pattern);
    char*               (*substring_ptr)    (const string_t string, const char* pattern);

    long double         (*to_ld)            (const string_t string, char** endptr);
    double              (*to_d)             (const string_t string, char** endptr);
    float               (*to_f)             (const string_t string, char** endptr);
    unsigned long long  (*to_ull)           (const string_t string, char** endptr, int base);
    long long           (*to_ll)            (const string_t string, char** endptr, int base);
    unsigned long       (*to_ul)            (const string_t string, char** endptr, int base);
    long                (*to_l)             (const string_t string, char** endptr, int base);
};



extern const struct __string_interface string;

#endif // _C_STRING_H_
