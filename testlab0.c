#include "testLab.h"
#include <stdio.h>
#include <string.h>

static int testN = 0;
static const struct {const char *const args[4], *const out;} testInOut[] = {
    {{"2", "2", "111.01", NULL}, "111.010000000000"},
    {{"2", "4", "111.01", NULL}, "13.100000000000"},
    {{"2", "2", "2.01", NULL}, "bad input"},
    {{"4", "2", "13.1", NULL}, "111.010000000000"},
    {{"1", "1", "0.0", NULL}, "bad input"},
    {{"1", "2", "0.0", NULL}, "bad input"},
    {{"2", "1", "0.0", NULL}, "bad input"},
    {{"17", "2", "0.0", NULL}, "bad input"},
    {{"2", "17", "0.0", NULL}, "bad input"},
    {{"258", "2", "0.0", NULL}, "bad input"},
    {{"2", "258", "0.0", NULL}, "bad input"},
    {{"65538", "2", "0.0", NULL}, "bad input"},
    {{"2", "65538", "0.0", NULL}, "bad input"},
    {{"2", "2", "0.", NULL}, "bad input"},
    {{"2", "2", ".0", NULL}, "bad input"},
    {{"2", "2", ".", NULL}, "bad input"},
    {{"2", "2", "0..0", NULL}, "bad input"},
    {{"16", "2", "fffffffffff.0", NULL}, "1111""1111""1111""1111""1111""1111""1111""1111""1111""1111""1111"},
    {{"2", "16", "11111111111.0", NULL}, "7ff"},
    {{"16", "2", "0.fffffffffff", NULL}, "0.111111111111"},
    {{"2", "16", "0.11111111111", NULL}, "0.ffe"},
    {{"16", "2", "f.a", NULL}, "1111.101000000000"},
    {{"2", "16", "1111.101", NULL}, "f.a00000000000"},
    {{"2", "7", "111", NULL}, "10"},
    {{"7", "2", "10", NULL}, "111"},
    {{"16", "2", "0.00001", NULL}, "0.000000000000"},
    {{"10", "2", "0.8", NULL}, "0.110011001100"},
    {{"16", "15", "fEdCbA987654", NULL}, "225e406dad6c9"},
    {{"2", "15", "1", NULL}, "1"},
    {{"2", "15", "0", NULL}, "0"},
    {{"2", "15", "0.(0)", NULL}, "bad input"},
    {{"2", "15", "1=0", NULL}, "bad input"},
    {{"9", "3", "0.3", NULL}, "0.100000000000"},
    {{"9", "3", "0.03", NULL}, "0.001000000000"},
    {{"9", "3", "0.003", NULL}, "0.000010000000"},
    {{"9", "3", "0.0003", NULL}, "0.000000100000"},
    {{"9", "3", "0.00003", NULL}, "0.000000001000"},
    {{"9", "3", "0.000003", NULL}, "0.000000000010"},
    {{"3", "9", "0.10000000001", NULL}, "0.300003000000"},
    {{"16", "2", "effffffffffe", NULL}, "111011111111111111111111111111111111111111111110"},
};

static int FeedFromArray(void) {
    const char *const * args = testInOut[testN].args;
    FILE* const in = fopen("in.txt", "w+");

    if (in == NULL) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    fprintf(in, "%s %s\n%s\n", args[0], args[1], args[2]);

    fclose(in);

    return 0;
}

static const char *const * Args(void) {
    return testInOut[testN].args;
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
    TLabTest labTest = {FeedFromArray, CheckFromArray, Args};
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
