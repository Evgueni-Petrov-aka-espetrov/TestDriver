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
    {"2 15\n0\n", "0"} // 29
};

static int feederN(void)
{
    FILE *const in = fopen("in.txt", "w+");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "%s", testInOut[testN].in);
    fclose(in);
    return 0;
}

static int checkerN(void)
{
    FILE *const out = fopen("out.txt", "r");
    char buf[128] = {0};
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    if (fgets(buf, sizeof(buf), out) == NULL) {
        printf("no output -- ");
    }
    fclose(out);
    if (strchr(buf, '\n'))
        *strchr(buf, '\n') = 0;
    if (strchr(buf, '.'))
        strncpy(buf+strlen(buf), "0000""0000""0000""0000""0000""0000""0000""0000", sizeof(buf)-strlen(buf)-1);
    //printf("%s %s\n", testInOut[testN].out, buf);
    if (strnicmp(testInOut[testN].out, buf, strlen(testInOut[testN].out)) == 0) {
        printf("PASSED\n");
        testN++;
        return 0;
    } else {
        printf("FAILED\n");
        testN++;
        return 1;
    }
}

const struct labFeedAndCheck labTests[] = {
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN}
};

const int labNTests = sizeof(labTests)/sizeof(labTests[0]);

const char labName[] = "Lab 0 Number Systems";

int labTimeout = 3000;
size_t labOutOfMemory = 1024*1024;
