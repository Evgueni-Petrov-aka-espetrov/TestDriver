#include "testLab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int testN = 0;
static const struct {const char *const in; int n;} testInOut[] = {
    {"2\n0\n\n", 0},
    {"2\n1\n25\n", 1},
    {"2\n3\n1 2 3\n", 1},
    {"2\n4\n1 2 3 4\n", 2},
    {"3\n5\n1 2 3 4 5\n", 1},
    {"2000\n5\n1 2 3 4 5\n", 1},
    {"2\n10\n0 1 2 3 4 -1 -2 -3 -4 -5\n", 2},
    {"2\n10\n1 -2 3 -4 -5 0 -1 2 -3 4\n", 3},
    {"2\n10\n0 -1 -2 -3 -4 -5 1 2 3 4\n", 3},
    {"2\n10\n4 3 2 1 0 -1 -2 -3 -4 -5\n", 3},
    {"2\n10\n0 4 3 2 1 -5 -4 -3 -2 -1\n", 3},
    {"3\n10\n1 -2 3 -4 -5 0 -1 2 -3 4\n", 2},
    {"4\n10\n0 -1 -2 -3 -4 -5 1 2 3 4\n", 2},
    {"5\n10\n0 1 2 3 4 -1 -2 -3 -4 -5\n", 2},
    {"6\n10\n4 3 2 1 0 -1 -2 -3 -4 -5\n", 1},
    {"7\n10\n0 4 3 2 1 -5 -4 -3 -2 -1\n", 1},
    {"2\n3\n1 3 2\n", 1},
    {"2\n3\n2 3 1\n", 1},
    {"2\n3\n2 1 3\n", 1},
    {"2\n3\n3 1 2\n", 1},
    {"2\n3\n3 2 1\n", 1},
    {"2\n4\n4 3 2 1\n", 2},
    {"2\n4\n2 3 1 4\n", 2},
    {"2\n4\n2 1 3 4\n", 2},
    {"3\n5\n1 3 1 4 5\n", 1},
    {"3\n5\n1 4 2 3 5\n", 1},
    {"3\n5\n5 1 2 3 4\n", 1},
    {"3\n5\n4 1 2 3 5\n", 1},
    {"3\n5\n3 1 2 4 5\n", 1},
    {"3\n5\n2 1 3 4 5\n", 1},
    {"3\n5\n5 4 3 2 1\n", 1},
    {"3\n5\n4 3 2 1 5\n", 1},
    {"3\n5\n3 2 1 5 4\n", 1},
    {"3\n5\n2 1 5 4 3\n", 1},
    {"3\n5\n1 5 4 3 2\n", 1},
    {"3\n6\n1 2 3 4 5 6\n", 2},
    {"3\n6\n6 5 4 3 2 1\n", 2},
    {"3\n6\n4 5 6 1 2 3\n", 2},
    {"3\n6\n3 2 1 6 5 4\n", 2},
    {"3\n7\n-3 -2 -1 0 1 2 3\n", 2},
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
    fprintf(in, "2\n");
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
    LabMemoryLimit = 20000000*(sizeof(int)+sizeof(int)+2*GetLabPointerSize())+MIN_PROCESS_RSS_BYTES;
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
        } else if (20 != n) {
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
    fprintf(in, "1000\n");
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
    LabMemoryLimit = 2000000*(sizeof(int)+sizeof(int)+sizeof(int)+2*GetLabPointerSize())+MIN_PROCESS_RSS_BYTES;
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
        } else if (2 != n) {
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

static void genTree(FILE *in, int height, int min, int max)
{
    int mid = (min*21+max*34)/55;
    if (height == 0) {
        return;
    }
    if (height == 1) {
        if (fprintf(in, "%d ", min) < 0) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            exit(1);
        }
        return;
    }
    if (height == 2) {
        if (min == max) {
            printf("Internal error: generation failed, h = %d, min = %d, max = %d\n", height, min, max);
            fclose(in);
            exit(1);
        }
        if (fprintf(in, "%d %d ", max, min) < 0) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            exit(1);
        }
        return;
    }
    if (mid <= min || max <= mid) {
        printf("Internal error: generation failed, h = %d, min = %d, max = %d\n", height, min, max);
        fclose(in);
        exit(1);
    }
    genTree(in, height-1, min, mid-1);
    genTree(in, height-2, mid+1, max);
    if (fprintf(in, "%d ", mid) < 0) {
        printf("can't create in.txt. No space on disk?\n");
        fclose(in);
        exit(1);
    }
}

static int feederBig2(void)
{
    FILE *const in = fopen("in.txt", "w+");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "2000\n");
    fprintf(in, "1346269\n");
    genTree(in, 30, 0, 4000000);
    fprintf(in, "\n");
    fclose(in);
    LabMemoryLimit = 1346269*(sizeof(int)+sizeof(int)+2*GetLabPointerSize())+MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int checkerBig2(void)
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
        } else if (2 != n) {
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
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {feederBig, checkerBig},
    {feederBig1, checkerBig1},
    {feederBig2, checkerBig2},
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return sizeof(LabTests)/sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
    return "Lab 6-2 B trees";
}

int GetTestTimeout() {
    return 6000;
}

static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;
size_t GetTestMemoryLimit() {
    return LabMemoryLimit;
}
