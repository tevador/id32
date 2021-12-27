/* Copyright (c) 2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#include "gf32.h"
#include "id32.h"

#include <stdbool.h>
#include <string.h>

static const char* charset = "ybndrfg8ejkmcpqxot1uwisza345h769";

#define SEPARATOR_STR "-"
#define SEPARATOR SEPARATOR_STR[0]
#define GROUP_SIZE 5
#define GROUP_STR "*****"

#define CHUNK_BITS 60
#define CHUNK_CHARS 12
#define NUM_DIGITS 2*CHUNK_CHARS+1

typedef gf_elem id32_digits[NUM_DIGITS];

static const int charset_rev[] = {
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 18, -1, 25, 26, 27, 30, 29,  7, 31, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
    -1, 24,  1, 12,  3,  8,  5,  6, 28, 21,  9, 10, -1, 11,  2, 16,
    13, 14,  4, 22, 17, 19, -1, 20, 15,  0, 23, -1, -1, -1, -1, -1,
};

static uint64_t load64_le(const uint8_t* ptr) {
    uint64_t val;
#if defined(_M_X64) || defined(__x86_64__) || defined(__aarch64__) || defined(_M_ARM64)
    memcpy(&val, ptr, sizeof(val));
#else
    val = (uint64_t)ptr[0];
    val |= (uint64_t)ptr[1] << 8;
    val |= (uint64_t)ptr[2] << 16;
    val |= (uint64_t)ptr[3] << 24;
    val |= (uint64_t)ptr[4] << 32;
    val |= (uint64_t)ptr[5] << 40;
    val |= (uint64_t)ptr[6] << 48;
    val |= (uint64_t)ptr[7] << 56;
#endif
    return val;
}

static void decode_chunk(const uint8_t* ptr, gf_elem digits[CHUNK_CHARS]) {
    uint64_t val = load64_le(ptr);
    val &= ((UINT64_C(1) << CHUNK_BITS) - 1);
    for (int i = 0; i < CHUNK_CHARS; ++i) {
        digits[i] = val % GF_SIZE;
        val /= GF_SIZE;
    }
    assert(val == 0);
}

static void hash_to_digits(const uint8_t hash[16], id32_digits digits) {
    decode_chunk(&hash[0], &digits[0]);
    decode_chunk(&hash[8], &digits[CHUNK_CHARS]);
}

static bool id_to_digits(const char* id, id32_digits digits) {
    int i;
    int j = 0;
    for (i = 0; i < ID32_SIZE - 1; ++i) {
        if ((i % (GROUP_SIZE + 1)) == GROUP_SIZE) {
            if (id[i] != SEPARATOR) {
                return false;
            }
            continue;
        }
        if (id[i] <= 0 || id[i] > 127) {
            return false;
        }
        int val = charset_rev[id[i]];
        if (val < 0) {
            return false;
        }
        digits[j++] = val;
    }
    if (id[i] != '\0') {
        return false;
    }
    assert(j == NUM_DIGITS);
    return true;
}

const char* id32_charset(void) {
    return charset;
}

const char* id32_pattern(void) {
    return GROUP_STR SEPARATOR_STR GROUP_STR SEPARATOR_STR GROUP_STR
        SEPARATOR_STR GROUP_STR SEPARATOR_STR GROUP_STR;
}

void id32_make(const uint8_t hash[16], char id_out[ID32_SIZE]) {
    assert(hash != NULL);
    assert(id_out != NULL);
    id32_digits digits;
    hash_to_digits(hash, digits);
    gf32_poly_encode(digits, NUM_DIGITS);
    int j = 0;
    for (int i = 0; i < NUM_DIGITS; ++i) {
        if ((j % (GROUP_SIZE + 1)) == GROUP_SIZE) {
            id_out[j++] = SEPARATOR;
        }
        id_out[j++] = charset[digits[i]];
    }
    id_out[j++] = '\0';
    assert(j == ID32_SIZE);
}

id32_result id32_check(const char* id, const uint8_t hash[16]) {
    assert(id != NULL);
    id32_digits digits1;
    if (!id_to_digits(id, digits1)) {
        return ID32_ERR_FORMAT;
    }
    if (!gf32_poly_check(digits1, NUM_DIGITS)) {
        return ID32_ERR_TYPO;
    }
    if (hash != NULL) {
        id32_digits digits2;
        hash_to_digits(hash, digits2);
        for (int i = 0; i < NUM_DIGITS - 1; ++i) {
            if (digits1[i] != digits2[i]) {
                return ID32_ERR_HASH;
            }
        }
    }
    return ID32_OK;
}
