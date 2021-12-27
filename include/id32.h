/* Copyright (c) 2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifndef ID32_H
#define ID32_H

/*
Shared/static library definitions
    - define ID32_SHARED when building a shared library
    - define ID32_STATIC when building a static library
    - define ID32_STATIC when linking to the static library
*/
#if defined(_WIN32) || defined(__CYGWIN__)
    #ifdef ID32_SHARED
        #define ID32_API __declspec(dllexport)
    #elif !defined(ID32_STATIC)
        #define ID32_API __declspec(dllimport)
    #else
        #define ID32_API
    #endif
#else
    #ifdef ID32_SHARED
        #define ID32_API __attribute__ ((visibility ("default")))
    #elif defined(ID32_STATIC)
        #define ID32_API __attribute__ ((visibility ("hidden")))
    #else
        #define ID32_API
    #endif
#endif

#include <stdint.h>

#define ID32_SIZE 30

typedef enum id32_result {
    ID32_OK = 0,
    ID32_ERR_FORMAT = 1,
    ID32_ERR_TYPO = 2,
    ID32_ERR_HASH = 3,
} id32_result;

#ifdef __cplusplus
extern "C" {
#endif

/**
 *  @return the ID32 character set: "ybndrfg8ejkmcpqxot1uwisza345h769".
 */
ID32_API const char* id32_charset(void);

/**
 *  @return the ID32 pattern: "*****-*****-*****-*****-*****"
 *          Asterisks in the pattern are placeholders for ID32 characters.
 *          Other characters serve as separators. The pattern may be used to
 *          pre-validate user input.
 */
ID32_API const char* id32_pattern(void);

/**
 * Creates an ID32 from a hash value.
 *
 * @param hash is a buffer that contains the hash value (at least 128 bits).
 *        Must not be NULL.
 * @param id_out is the buffer where the ID will be stored as a null-terminated
 *        string. Must not be NULL.
*/
ID32_API void id32_make(const uint8_t hash[16], char id_out[ID32_SIZE]);

/**
 * Checks the validity of an ID32 string.
 *
 * @param id is a null-terminated ID string. Must not be NULL.
 * @param hash is an optional hash buffer (at least 128 bits). If not NULL,
 *        the ID is compared to the hash value. If NULL, only the ID string
 *        itself is validated.
 *
 * @return ID32_OK if the ID is valid and matches the provided hash value
 *         (or hash was NULL).
 *         ID32_ERR_FORMAT if the ID doesn't have the correct format or
 *         contains invalid characters.
 *         ID32_ERR_TYPO if the ID has the correct format, but the checksum
 *         is invalid.
 *         ID32_ERR_HASH if the ID is well-formed, but doesn't match
 *         the provided hash value (if hash was not NULL).
*/
ID32_API id32_result id32_check(const char* id, const uint8_t hash[16]);

#ifdef __cplusplus
}
#endif

#undef ID32_API

#endif
