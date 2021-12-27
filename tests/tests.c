/* Copyright (c) 2021 tevador <tevador@gmail.com> */
/* See LICENSE for licensing information */

#ifdef NDEBUG
#undef NDEBUG
#endif

#include <id32.h>

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define RUN_TEST(x) run_test(#x, &x)

typedef bool test_func(void);

static int g_test_no = 0;
static const char* g_charset;
static const char* g_pattern;
static char g_id1[ID32_SIZE];
static char g_id2[ID32_SIZE];
static char g_id3[ID32_SIZE];

static const uint8_t g_hash1[] = {
    0xb9, 0x7f, 0x51, 0xa6, 0x35, 0xab, 0x1a, 0x58,
    0xe8, 0xbc, 0x05, 0xb8, 0x4e, 0xec, 0x05, 0xf1,
};

static const uint8_t g_hash2[] = {
    0x81, 0x95, 0x3c, 0xf3, 0xf9, 0xb4, 0x1a, 0x64,
    0xe0, 0xf9, 0x10, 0x87, 0x48, 0x1e, 0xcd, 0x67,
};

static const uint8_t g_hash3[] = {
    0x24, 0x36, 0x9a, 0xde, 0x1c, 0x69, 0x5b, 0x42,
    0xda, 0x3e, 0xf2, 0xd3, 0xb6, 0xfc, 0xbd, 0x28,
};

static const char* g_hash1_id = "379nf-usgmi-goe8x-myh4j-cxbng";
static const char* g_hash2_id = "bcf3u-389wi-geyx6-btdnj-6euxf";
static const char* g_hash3_id = "rtpwj-xudj5-sr4sx-r9j5s-hxxtz";

static const char* g_invalid1 = "";

static const char* g_invalid2 =
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"
    "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

static const char* g_uppercase = "379NF-USGMI-GOE8X-MYH4J-CXBNG";
static const char* g_no_separator = "379nfusgmigoe8xmyh4jcxbng";

static void run_test(const char* name, test_func* func) {
    printf("[%2i] %-40s ... ", ++g_test_no, name);
    printf(func() ? "PASSED\n" : "SKIPPED\n");
}

static void check_pattern(const char* id) {
    assert(strlen(id) == ID32_SIZE - 1);
    int i;
    for (i = 0; i < ID32_SIZE; ++i) {
        if (g_pattern[i] == '*') {
            assert(strchr(g_charset, id[i]) != NULL);
        }
        else {
            assert(id[i] == g_pattern[i]);
        }
    }
    assert(id[i] == '\0');
}

static void check_checksum_subs(char id[], int pos) {
    char cc = id[pos];
    for (int i = 0; i < strlen(g_charset); ++i) {
        char c = g_charset[i];
        if (c == cc) {
            continue;
        }
        id[pos] = c;
        id32_result check = id32_check(id, NULL);
        assert(check == ID32_ERR_TYPO);
    }
    id[pos] = cc;
}

static void check_checksum_tran(char id[]) {
    for (int span = 1; span < 29; ++span) {
        for (int i = 0; i < ID32_SIZE - 1 - span; ++i) {
            if (id[i] == '-') continue;
            int j = i + span;
            if (id[j] == '-') j++;
            if (id[i] == id[j]) continue;
            char tmp = id[i];
            id[i] = id[j];
            id[j] = tmp;
            id32_result check = id32_check(id, NULL);
            assert(check == ID32_ERR_TYPO);
            id[j] = id[i];
            id[i] = tmp;
        }
    }
}

static void check_checksum_twin(char id[]) {
    for (int pos = 0; pos < ID32_SIZE - 2; ++pos) {
        char c = id[pos];
        if (id[pos + 1] != c) continue;
        for (int i = 0; i < 32; ++i) {
            if (g_charset[i] == c) continue;
            id[pos] = g_charset[i];
            id[pos + 1] = g_charset[i];
            id32_result check = id32_check(id, NULL);
            assert(check == ID32_ERR_TYPO);
        }
        id[pos] = c;
        id[pos + 1] = c;
    }
}

static void check_checksum(char id[]) {
    check_checksum_subs(id, 0);
    check_checksum_subs(id, 1);
    check_checksum_subs(id, 2);
    check_checksum_subs(id, 26);
    check_checksum_subs(id, 27);
    check_checksum_subs(id, 28);
    check_checksum_tran(id);
    check_checksum_twin(id);
}

static void test_make(const uint8_t hash[], char id[]) {
    id32_make(hash, id);
    check_pattern(id);
    id32_result check1 = id32_check(id, hash);
    assert(check1 == ID32_OK);
    id32_result check2 = id32_check(id, NULL);
    assert(check2 == ID32_OK);
}

static bool test_charset(void) {
    g_charset = id32_charset();
    assert(g_charset != NULL);
    assert(strlen(g_charset) == 32);
    return true;
}

static bool test_pattern(void) {
    g_pattern = id32_pattern();
    assert(g_pattern != NULL);
    assert(strlen(g_pattern) == ID32_SIZE - 1);
    return true;
}

static bool test_make1(void) {
    test_make(g_hash1, g_id1);
    return true;
}

static bool test_make2(void) {
    test_make(g_hash2, g_id2);
    return true;
}

static bool test_make3(void) {
    test_make(g_hash3, g_id3);
    return true;
}

static bool test_id1(void) {
    assert(strcmp(g_id1, g_hash1_id) == 0);
    return true;
}

static bool test_id2(void) {
    assert(strcmp(g_id2, g_hash2_id) == 0);
    return true;
}

static bool test_id3(void) {
    assert(strcmp(g_id3, g_hash3_id) == 0);
    return true;
}

static bool test_mismatch1(void) {
    id32_result check = id32_check(g_id1, g_hash2);
    assert(check == ID32_ERR_HASH);
    return true;
}

static bool test_mismatch2(void) {
    id32_result check = id32_check(g_id2, g_hash3);
    assert(check == ID32_ERR_HASH);
    return true;
}

static bool test_mismatch3(void) {
    id32_result check = id32_check(g_id3, g_hash1);
    assert(check == ID32_ERR_HASH);
    return true;
}

static bool test_invalid1(void) {
    id32_result check = id32_check(g_invalid1, NULL);
    assert(check == ID32_ERR_FORMAT);
    return true;
}

static bool test_invalid2(void) {
    id32_result check = id32_check(g_invalid2, NULL);
    assert(check == ID32_ERR_FORMAT);
    return true;
}

static bool test_uppercase(void) {
    id32_result check = id32_check(g_uppercase, NULL);
    assert(check == ID32_ERR_FORMAT);
    return true;
}


static bool test_no_separator(void) {
    id32_result check = id32_check(g_no_separator, NULL);
    assert(check == ID32_ERR_FORMAT);
    return true;
}

static bool test_checksum1(void) {
    check_checksum(g_id1);
    return true;
}

static bool test_checksum2(void) {
    check_checksum(g_id2);
    return true;
}

static bool test_checksum3(void) {
    check_checksum(g_id3);
    return true;
}

int main() {
    RUN_TEST(test_charset);
    RUN_TEST(test_pattern);
    RUN_TEST(test_make1);
    RUN_TEST(test_make2);
    RUN_TEST(test_make3);
    RUN_TEST(test_id1);
    RUN_TEST(test_id2);
    RUN_TEST(test_id3);
    RUN_TEST(test_mismatch1);
    RUN_TEST(test_mismatch2);
    RUN_TEST(test_mismatch3);
    RUN_TEST(test_invalid1);
    RUN_TEST(test_invalid2);
    RUN_TEST(test_uppercase);
    RUN_TEST(test_no_separator);
    RUN_TEST(test_checksum1);
    RUN_TEST(test_checksum2);
    RUN_TEST(test_checksum3);

    printf("\nAll tests were successful\n");

    return 0;
}
