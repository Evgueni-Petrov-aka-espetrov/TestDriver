#include "testLab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int testN = 0;
static const struct {const char *const in; int n;} testInOut[] = {
    {"0\n\n", 0},
    {"1\n25\n", 2},
    {"2\n17 25\n", 2},
    {"3\n2 -1 4\n", 2},
    {"3\n2 4 -1\n", 2},
    {"3\n4 2 -1\n", 2},
    {"3\n4 -1 2\n", 2},
    {"3\n-1 4 2\n", 2},
    {"3\n-1 2 4\n", 2},
    {"3\n-1 -1 2\n", 2},
    {"3\n-1 2 -1\n", 2},
    {"3\n2 -1 -1\n", 2},
    {"3\n-1 -1 -1\n", 2},
    {"2\n-1 -1\n", 2},
    {"2\n4 2\n", 2},
    {"2\n2 4\n", 2},
    {"4\n1 2 3 4\n", 3},
    {"4\n1 3 2 4\n", 3},
    {"4\n3 1 2 4\n", 3},
    {"4\n3 2 1 4\n", 3},
    {"5\n1 2 3 5 4\n", 3},
    {"5\n1 2 5 3 4\n", 3},
    {"5\n1 2 5 4 3\n", 3},
    {"5\n1 3 2 5 4\n", 3},
    {"5\n1 5 2 3 4\n", 3},
    {"5\n1 5 2 4 3\n", 3},
    {"5\n5 1 2 3 4\n", 3},
    {"5\n5 1 2 4 3\n", 3},
    {"5\n5 2 1 4 3\n", 3},
    {"6\n1 2 3 4 5 6\n", 3},
    {"10\n1 2 3 4 5 6 7 8 9 10\n", 4},
    {"22\n1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22\n", 5},
    {"38\n1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38\n", 5},
    {"7\n1 3 2 5 4 7 6\n", 3},
    {"7\n7 1 2 3 4 5 6\n", 3},
    {"7\n7 2 1 4 3 6 5\n", 3},
    {"8\n1 2 8 5 7 4 3 6\n", 3},
    {"12\n10 18 7 15 16 30 20 40 60 2 1 70\n", 4},
};

static int FeedFromArray(void)
{
    FILE *const in = fopen("in.txt", "w+");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    if (fprintf(in, "%s", testInOut[testN].in) < 0) {
        printf("can't create in.txt. No space on disk?\n");
        fclose(in);
        return -1;
    }
    fclose(in);
    return 0;
}

static int CheckFromArray(void)
{
    FILE *const out = fopen("out.txt", "r");
    int passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    {
        int n;
        if (ScanInt(out, &n) != Pass) {
            passed = 0;
        } else if (testInOut[testN].n != n) {
            passed = 0;
            printf("wrong output -- ");
        }
    }
    if (passed) {
        passed = !HaveGarbageAtTheEnd(out);
    }
    fclose(out);
    if (passed) {
        printf("PASSED\n");
        testN++;
        return 0;
    } else {
        printf("FAILED\n");
        testN++;
        return 1;
    }
}

static size_t LabMemoryLimit;

static int feederBig(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "2000000\n");
    for (i = 0; i < 2000000; i++) {
        if (fprintf(in, "%d ", i) < 0) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fprintf(in, "\n");
    fclose(in);
    LabMemoryLimit = 2000000*(sizeof(int)+sizeof(int)+2*GetLabPointerSize())+MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int checkerBig(void)
{
    FILE *const out = fopen("out.txt", "r");
    int passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    {
        int n;
        if (ScanInt(out, &n) != Pass) {
            passed = 0;
        } else if (21 != n) {
            passed = 0;
            printf("wrong output -- ");
        }
    }
    if (passed) {
        passed = !HaveGarbageAtTheEnd(out);
    }
    fclose(out);
    if (passed) {
        printf("PASSED\n");
        testN++;
        return 0;
    } else {
        printf("FAILED\n");
        testN++;
        return 1;
    }
}

static int feederBig1(void)
{
    FILE *const in = fopen("in.txt", "w+");
    unsigned i;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "2000000\n");
    for (i = 0; i < 2000000; i++) {
        if (fprintf(in, "%d ", i^0xcafecafe) < 0) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fprintf(in, "\n");
    fclose(in);
    LabMemoryLimit = 2000000*(sizeof(int)+sizeof(int)+2*GetLabPointerSize())+MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int checkerBig1(void)
{
    FILE *const out = fopen("out.txt", "r");
    int passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    {
        int n;
        if (ScanInt(out, &n) != Pass) {
            passed = 0;
        } else if (19 != n) {
            passed = 0;
            printf("wrong output -- ");
        }
    }
    if (passed) {
        passed = !HaveGarbageAtTheEnd(out);
    }
    fclose(out);
    if (passed) {
        printf("PASSED\n");
        testN++;
        return 0;
    } else {
        printf("FAILED\n");
        testN++;
        return 1;
    }
}

const TLabTest LabTests[] = {
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {feederBig, checkerBig},
    {feederBig1, checkerBig1},
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return sizeof(LabTests)/sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
    return "Lab 6-1 red-black trees";
}

int GetTestTimeout(void) {
    return 6000;
}

static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;
size_t GetTestMemoryLimit(void) {
    return LabMemoryLimit;
}
