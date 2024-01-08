#include "testLab.h"
#include <stdio.h>
#include <string.h>

static int testN = 0;
static const struct {const char *const in, *const out;} testInOut[] = {
    {"2 2\n111.01\n", "111.010000000000"},
    {"2 4\n111.01\n", "13.100000000000"},
    {"2 2\n2.01\n", "bad input"},
    {"4 2\n13.1\n", "111.010000000000"},
    {"1 1\n0.0\n", "bad input"},
    {"1 2\n0.0\n", "bad input"},
    {"2 1\n0.0\n", "bad input"},
    {"17 2\n0.0\n", "bad input"},
    {"2 17\n0.0\n", "bad input"},
    {"258 2\n0.0\n", "bad input"},
    {"2 258\n0.0\n", "bad input"},
    {"65538 2\n0.0\n", "bad input"},
    {"2 65538\n0.0\n", "bad input"},
    {"2 2\n0.\n", "bad input"},
    {"2 2\n.0\n", "bad input"},
    {"2 2\n.\n", "bad input"},
    {"2 2\n0..0\n", "bad input"},
    {"16 2\nfffffffffff.0\n", "1111""1111""1111""1111""1111""1111""1111""1111""1111""1111""1111"},
    {"2 16\n11111111111.0\n", "7ff"},
    {"16 2\n0.fffffffffff\n", "0.111111111111"},
    {"2 16\n0.11111111111\n", "0.ffe"},
    {"16 2\nf.a\n", "1111.101000000000"},
    {"2 16\n1111.101\n", "f.a00000000000"},
    {"2 7\n111\n", "10"},
    {"7 2\n10\n", "111"},
    {"16 2\n0.00001\n", "0.000000000000"},
    {"10 2\n0.8\n", "0.110011001100"},
    {"16 15\nfEdCbA987654\n", "225e406dad6c9"},
    {"2 15\n1\n", "1"},
    {"2 15\n0\n", "0"},
    {"2 15\n0.(0)\n", "bad input"},
    {"2 15\n1=0\n", "bad input"},
    {"9 3\n0.3\n     ", "0.100000000000"},
    {"9 3\n0.03\n    ", "0.001000000000"},
    {"9 3\n0.003\n   ", "0.000010000000"},
    {"9 3\n0.0003\n  ", "0.000000100000"},
    {"9 3\n0.00003\n ", "0.000000001000"},
    {"9 3\n0.000003\n", "0.000000000010"},
};

static int FeedFromArray(void) {
    FILE* const in = fopen("in.txt", "w+");
    if (in == NULL) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "%s", testInOut[testN].in);
    fclose(in);
    return 0;
}

static int CheckFromArray(void) {
    FILE* const out = fopen("out.txt", "r");
    if (out == NULL) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    char buf[128] = {0};
    const char* status = ScanChars(out, sizeof(buf), buf);
    fclose(out);
    if (status == Pass && strchr(buf, '.')) {
        strncpy(buf + strlen(buf), "0000""0000""0000""0000""0000""0000""0000""0000", sizeof(buf) - strlen(buf) - 1);
    }
    if (status == Pass && _strnicmp(testInOut[testN].out, buf, strlen(testInOut[testN].out)) != 0) {
        status = Fail;
    }
    printf("%s\n", status);
    ++testN;
    return status == Fail;
}

TLabTest GetLabTest(int testIdx) {
    (void)testIdx;
    TLabTest labTest = {FeedFromArray, CheckFromArray};
    return labTest;
}

int GetTestCount(void) {
    return sizeof(testInOut) / sizeof(testInOut[0]);
}

const char* GetTesterName(void) {
    return "Lab 0 Number Systems";
}

int GetTestTimeout(void) {
    return 3000;
}

size_t GetTestMemoryLimit(void) {
    return MIN_PROCESS_RSS_BYTES;
}
