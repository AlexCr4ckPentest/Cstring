#include "../include/string.h"

#include <malloc.h>
#include <string.h>



enum { _STRING_MAGIC_NUMBER_ = (uint16_t)0xf8f };

/**
 * @brief String metadata
 */
struct __attribute__((packed, aligned(1))) __string_header
{
    size_t length;
    size_t allocated_size;
    uint16_t magic_number;
};



#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
#undef NDEBUG
#include <assert.h>
#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus
/**
 * @brief Check string magic number (for security)
 *
 * @param string existing string
 * @return 1 if magic number is valid, otherwise - 0
 */
inline static int __string_check_magic_number(const string_t string)
{
    return (((struct __string_header*)(string - sizeof(struct __string_header)))->magic_number == _STRING_MAGIC_NUMBER_);
}
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_



/**
 * @brief Access to string header (length)
 * 
 * @param ptr string
 * @return pointer to struct field (length)
 */
inline static size_t* __string_length_address(const string_t ptr)
{
    return &(((struct __string_header*)(ptr - sizeof(struct __string_header)))->length); // Warning: unaligned pointer value
}



/**
 * @brief Access to string header (allocated size)
 * 
 * @param ptr string
 * @return pointer to struct field (allocated_size)
 */
inline static size_t* __string_allocated_size_address(const string_t ptr)
{
    return &(((struct __string_header*)(ptr - sizeof(struct __string_header)))->allocated_size); // Warning: unaligned pointer value
}
#ifdef __cplusplus
}
#endif // __cplusplus



/**
 * @brief Allocate n + sizeof(struct __string_header) bytes of raw memory
 * 
 * @param n bytes to allocate
 * @return user pointer
 */
string_t __string_allocate(const size_t n)
{
    string_t raw_memory_block_ptr = (string_t)malloc(n + sizeof(struct __string_header));

    if (raw_memory_block_ptr == NULL)
    {
        return NULL;
    }

    struct __string_header string_header = {
        .allocated_size = n + sizeof(struct __string_header),
        .length = 0,
        .magic_number = _STRING_MAGIC_NUMBER_
    };

    *((struct __string_header*)(raw_memory_block_ptr)) = string_header;

    return (raw_memory_block_ptr + sizeof(struct __string_header));
}



/**
 * @brief Reallocate memory for existing string
 * 
 * @param ptr pointer to existing string
 * @param _size size of current existing string 
 * @param __size size of other existing string
 * @param new_size new size to allocate
 * @return pointer to existing string
 */
string_t __string_realloc_if_need(string_t* ptr, const size_t _size, const size_t __size, const size_t new_size)
{
    if (_size > __size)
    {
        *ptr -= sizeof(struct __string_header);
        *ptr = (string_t)realloc(*ptr, new_size);

        if (*ptr == NULL)
        {
            return NULL;
        }

        ((struct __string_header*)(*ptr))->allocated_size = new_size;
        *ptr += sizeof(struct __string_header);
    }
    return *ptr;
}



/**
 * @brief Free allocated memory block and invalidate a pointer
 * 
 * @param string pointer to existing string
 */
void string_free(string_t* string)
{
    if (*string != NULL)
    {
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
        assert(("string: invalid magic number!" && __string_check_magic_number(*string)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_
        free(*string - sizeof(struct __string_header));
        *string = NULL;
    }
}



/**
 * @brief Create and initialize a new string from C-string
 * 
 * @param c_string C-like string (aka. const char*)
 * @return new string
 */
string_t string_init(const char* c_string)
{
    const size_t string_length = strlen(c_string);
    string_t new_string = __string_allocate(string_length + 1);

    *__string_length_address(new_string) = string_length;
    memcpy(new_string, c_string, string_length + 1);

    return new_string;
}



/**
 * @brief Create a copy of existing string
 * 
 * @param string existing string
 * @return copy of string
 */
string_t string_clone(const string_t string)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(string)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    const size_t length = string_length(string);
    string_t copy = __string_allocate(length + 1);

    memcpy(copy, string, length + 1);
    *__string_length_address(copy) = length;

    return copy;
}



/**
 * @brief Create substring from existing string between 2 positions
 * 
 * @param str existing string
 * @param init_pos initial substring position
 * @param end_pos end substring position
 * @return substing
 */
string_t string_substring_create(const string_t str, const size_t init_pos, const size_t end_pos)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(str)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    if (init_pos >= end_pos)
    {
        return NULL;
    }

    string_t ptr_init = str + init_pos;
    string_t ptr_end = str + end_pos;
    const ptrdiff_t difference = ptr_end - ptr_init;

    string_t substring = __string_allocate(difference + 1);

    if (substring == NULL)
    {
        return NULL;
    }

    memcpy(substring, ptr_init, difference);
    *__string_length_address(substring) = difference;

    return substring;
}



/**
 * @brief Return current length of a string
 * 
 * @param string existing string
 * @return length
 */
inline size_t string_length(const string_t string)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(string)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    return *__string_length_address(string);
}



/**
 * @brief Return current allocated size of a string (include header size)
 * 
 * @param string existing string
 * @return allocated size in bytes 
 */
inline size_t string_allocated_size(const string_t string)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(string)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    return *__string_allocated_size_address(string);
}



/**
 * @brief Compare str1 and str2 strings lexicographically
 * 
 * @param str1 first string
 * @param str2 second string
 * @return 0 if equals, otherwise - other integer values
 */
inline int string_compare(const string_t str1, const string_t str2)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(str1)));
    assert(("string: invalid magic number!" && __string_check_magic_number(str2)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    return strcmp(str1, str2);
}



/**
 * @brief Compare str1 and str2 strings lexicographically
 * 
 * @param str1 first string
 * @param str2 second C-string
 * @return 0 if equals, otherwise - other integer values
 */
inline int string_c_compare(const string_t str1, const char* str2)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(str1)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    return strcmp(str1, str2);
}



/**
 * @brief Copy all elements from string src to dst
 * 
 * Call string_c_copy_n()
 * 
 * @param dst destionation string
 * @param src source string
 * @return destionation string
 */
string_t string_copy(string_t* dst, const string_t src)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(*dst)));
    assert(("string: invalid magic number!" && __string_check_magic_number(src)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    return string_copy_n(dst, src, string_length(src));
}



/**
 * @brief Copy n elements from string src to dst
 * 
 * If allocated memory for dst is not enough - call __string_realloc_if_need() and increase allocated memory block
 * 
 * @param dst destionation string
 * @param src source string
 * @param n count
 * @return destionation string
 */
string_t string_copy_n(string_t* dst, const string_t src, const size_t n)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(*dst)));
    assert(("string: invalid magic number!" && __string_check_magic_number(src)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    if (n == 0)
    {
        return *dst;
    }

    const size_t dst_allocated_size = string_allocated_size(*dst);

    *dst = __string_realloc_if_need(dst, n, dst_allocated_size - sizeof(struct __string_header), dst_allocated_size + n);

    if (*dst == NULL)
    {
        return NULL;
    }

    memcpy(*dst, src, n);
    *__string_length_address(*dst) = n;

    return *dst;
}



/**
 * @brief Copy all elements from string src to dst
 * 
 * Warning! This function requires C-string as 2-d argument!!! NOT string_t!
 * Call string_c_copy_n()
 * 
 * @param dst destionation string
 * @param src source C-string
 * @return destionation string
 */
string_t string_c_copy(string_t* dst, const char* src)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(*dst)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    return string_c_copy_n(dst, src, strlen(src));
}



/**
 * @brief Copy n elements from string src to dst
 * 
 * Warning! This function requires C-string as 2-d argument!!! NOT string_t!
 * If allocated memory for dst is not enough - call __string_realloc_if_need() and increase allocated memory block
 * 
 * @param dst destionation string
 * @param src source C-string
 * @param n count
 * @return destionation string
 */
string_t string_c_copy_n(string_t* dst, const char* src, const size_t n)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(*dst)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    if (n == 0)
    {
        return *dst;
    }

    const size_t dst_allocated_size = string_allocated_size(*dst); 
    
    *dst = __string_realloc_if_need(dst, n, dst_allocated_size - sizeof(struct __string_header), dst_allocated_size + n);

    if (*dst == NULL)
    {
        return NULL;
    }

    memcpy(*dst, src, n);
    *__string_length_address(*dst) = n;

    return *dst;
}



/**
 * @brief Move src to dst and delete src
 * 
 * @param dst pointer to destination string
 * @param src pointer to source string
 * @return destination string
 */
string_t string_move(string_t* dst, string_t* src)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(*dst)));
    assert(("string: invalid magic number!" && __string_check_magic_number(*src)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    string_copy(dst, *src);
    string_free(src);
    return *dst;
}



/**
 * @brief Swap two strings
 * 
 * @param str1 pointer to first string
 * @param str2 pointer to second string
 * @return swapped string
 */
string_t string_swap(string_t* str1, string_t* str2)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(*str1)));
    assert(("string: invalid magic number!" && __string_check_magic_number(*str2)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    string_t tmp = *str1;
    *str1 = *str2;
    *str2 = tmp;

    return *str1;
}



/**
 * @brief Concat two strings
 * 
 * Call string_concat_n()
 * 
 * @param dst pointer to destination string
 * @param src source string
 * @return destination string
 */
string_t string_concat(string_t* dst, const string_t src)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(*dst)));
    assert(("string: invalid magic number!" && __string_check_magic_number(src)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    return string_concat_n(dst, src, string_length(src));
}



/**
 * @brief Concat n elements from src to dst
 * 
 * Reallocate memory for destination string if allocated size of source string >
 * 
 * @param dst pointer to destination string 
 * @param src existing source string
 * @param n amount of elements
 * @return destination string (if all is ok), otherwise - NULL (memory allocation error)
 */
string_t string_concat_n(string_t* dst, const string_t src, const size_t n)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(*dst)));
    assert(("string: invalid magic number!" && __string_check_magic_number(src)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    const size_t dst_allocated_size = string_allocated_size(*dst);

    *dst = __string_realloc_if_need(dst, n, dst_allocated_size - sizeof(struct __string_header), dst_allocated_size + n);

    if (*dst == NULL)
    {
        return NULL;
    }

    string_t dst_p_end = *dst + string_length(*dst);
    memcpy(dst_p_end, src, n);

    *__string_length_address(*dst) += n;

    return *dst;
}



/**
 * @brief Concat string and C-string
 * 
 * Call string_c_concat_n
 * 
 * @param dst pointer to destination string
 * @param src C-string
 * @return destination string
 */
string_t string_c_concat(string_t* dst, const char* src)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(*dst)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    return string_c_concat_n(dst, src, strlen(src));
}



/**
 * @brief Concat n elements from C-string src to dst
 * 
 * Reallocate memory for destination string if allocated size of source string >
 * 
 * @param dst pointer to destination string
 * @param src C-string
 * @param n amount of elements
 * @return destination string (if all is ok), otherwise - NULL (memory allocation error)
 */
string_t string_c_concat_n(string_t* dst, const char* src, const size_t n)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(*dst)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    const size_t dst_allocated_size = string_allocated_size(*dst);

    *dst = __string_realloc_if_need(dst, n, dst_allocated_size - sizeof(struct __string_header), dst_allocated_size + n);

    if (*dst == NULL)
    {
        return NULL;
    }

    string_t dst_p_end = *dst + string_length(*dst);
    memcpy(dst_p_end, src, n);

    *__string_length_address(*dst) += n;

    return *dst;
}



/**
 * @brief Get character position in an existing string
 * 
 * @param string existring string
 * @param ch character to find
 * @return character (if found), otherwise - STRING_NPOS
 */
inline long string_char_pos(const string_t string, const char ch)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(string)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    const char* ptr = strchr(string, ch);
    return (ptr == NULL ? STRING_NPOS : (ptr - string));
}



/**
 * @brief Get pointer to character in an existing string
 * 
 * @param string existring string
 * @param ch character to find
 * @return pointer ot character (if found), otherwise - NULL
 */
char* string_char_ptr(const string_t string, const char ch)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(string)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    return strchr(string, ch);
}



/**
 * @brief Get pointer character in string through index
 * 
 * @param string existring string
 * @param index index
 * @return character at index
 */
inline int string_char_at(const string_t string, const size_t index)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(string)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    const size_t length = string_length(string);
    assert(("string: index out of range!" && index < length));

    return *(string + index);
}



/**
 * @brief Get pointer to character in string through index
 * 
 * @param string existing string
 * @param index index
 * @return pointer to character at index
 */
inline char* string_ptr_at(const string_t string, const size_t index)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(string)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    const size_t length = string_length(string);
    assert(("string: index out of range!" && index < length));

    return (string + index);
}



/**
 * @brief Find substring in an existing string
 * 
 * @param string existing string
 * @param pattern substring to find
 * @return position of substring begin
 */
inline long string_substring_pos(const string_t string, const char* pattern)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(string)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    const char* substr_begin = strstr(string, pattern);
    return (substr_begin == NULL ? STRING_NPOS : substr_begin - string);
}


/**
 * @brief Find substring in an existing string
 * 
 * @param string existing string
 * @param pattern substring to find
 * @return pointer to beginning of substring
 */
inline char* string_substring_ptr(const string_t string, const char* pattern)
{
#ifndef _STRING_F_NO_CHECK_MAGIC_NUMBER_
    assert(("string: invalid magic number!" && __string_check_magic_number(string)));
#endif // _STRING_F_NO_CHECK_MAGIC_NUMBER_

    return strstr(string, pattern);
}

