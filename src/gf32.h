/* Copyright (c) 2021 tevador <tevador@gmail.com> */
/* License: BSD-3-Clause */

/*
    This is a simple but powerful check digit algorithm for base-32 encoded
    data. Despite being just a single digit, it can detect:

    * all single substitution errors (e.g. `a` -> `s`)
    * all single adjacent transpositions (e.g. `we` -> `ew`)
    * all single jump transpositions if the transposed characters are no more
      than 30 characters apart (e.g. `wer` -> `rew`)
    * all single twin errors (e.g. `dd` -> `ff`)
    * all single jump twin errors if the twin characters are no more than
      30 characters apart (e.g. `dfd` -> `gfg`)
    * about 97% of other errors
*/

#ifndef GF32_H
#define GF32_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define GF_SIZE 32

typedef uint8_t gf_elem; /* finite field element 0-31 */

static const gf_elem mul2_table[] = {
    0, 2, 4, 6, 8, 10, 12, 14, 16, 18, 20, 22, 24, 26, 28, 30,
    9, 11, 13, 15, 1, 3, 5, 7, 25, 27, 29, 31, 17, 19, 21, 23,
};

static gf_elem gf32_poly_eval(const gf_elem digits[], size_t num_digits) {
    /* Horner's method at x = 2 */
    gf_elem result = digits[0];
    for (unsigned i = 1; i < num_digits; ++i) {
        assert(result < GF_SIZE);
        result = mul2_table[result] ^ digits[i];
    }
    return result;
}

static bool gf32_poly_check(const gf_elem digits[], size_t num_digits) {
    return gf32_poly_eval(digits, num_digits) == 0;
}

static void gf32_poly_encode(gf_elem digits[], size_t num_digits) {
    digits[num_digits - 1] = 0;
    digits[num_digits - 1] = gf32_poly_eval(digits, num_digits);
}

#endif
