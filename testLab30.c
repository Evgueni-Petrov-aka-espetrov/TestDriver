#include "testLab.h"
#include <stdio.h>
#include <string.h>

static int testN = 0;
static const struct {const char *const in; int n, out[32];} testInOut[] = {
    {"3\n4 -1 2\n", 3, {-1, 2, 4}},
    {"3\n0 0 -1\n", 3, {-1, 0, 0}},
    {"3\n-1 2 4\n", 3, {-1, 2, 4}},
    {"3\n-1 4 2\n", 3, {-1, 2, 4}},
    {"3\n2 -1 4\n", 3, {-1, 2, 4}},
    {"3\n2 4 -1\n", 3, {-1, 2, 4}},
    {"3\n4 2 -1\n", 3, {-1, 2, 4}},
    {"3\n-1 -1 2\n", 3, {-1, -1, 2}},
    {"3\n-1 2 -1\n", 3, {-1, -1, 2}},
    {"3\n2 -1 -1\n", 3, {-1, -1, 2}},
    {"2\n-1 -1\n", 2, {-1, -1}},
    {"2\n4 2\n", 2, {2, 4}},
    {"2\n2 4\n", 2, {2, 4}},
    {"1\n2\n", 1, {2}},
    {"0\n\n", 0, {0}},
    {"6\n3 4 0 5 1 2\n", 6, {0, 1, 2, 3, 4, 5}},
    {"4\n1 2 3 4\n", 4, {1, 2, 3, 4}},
    {"4\n1 2 4 3\n", 4, {1, 2, 3, 4}},
    {"4\n1 3 2 4\n", 4, {1, 2, 3, 4}},
    {"4\n1 3 4 2\n", 4, {1, 2, 3, 4}},
    {"4\n1 4 2 3\n", 4, {1, 2, 3, 4}},
    {"4\n1 4 3 2\n", 4, {1, 2, 3, 4}},
    {"4\n2 1 3 4\n", 4, {1, 2, 3, 4}},
    {"4\n2 1 4 3\n", 4, {1, 2, 3, 4}},
    {"4\n2 3 1 4\n", 4, {1, 2, 3, 4}},
    {"4\n2 3 4 1\n", 4, {1, 2, 3, 4}},
    {"4\n2 4 1 3\n", 4, {1, 2, 3, 4}},
    {"4\n2 4 3 1\n", 4, {1, 2, 3, 4}},
    {"4\n3 1 2 4\n", 4, {1, 2, 3, 4}},
    {"4\n3 1 4 2\n", 4, {1, 2, 3, 4}},
    {"4\n3 2 1 4\n", 4, {1, 2, 3, 4}},
    {"4\n3 2 4 1\n", 4, {1, 2, 3, 4}},
    {"4\n3 4 1 2\n", 4, {1, 2, 3, 4}},
    {"4\n3 4 2 1\n", 4, {1, 2, 3, 4}},
    {"4\n4 1 2 3\n", 4, {1, 2, 3, 4}},
    {"4\n4 1 3 2\n", 4, {1, 2, 3, 4}},
    {"4\n4 2 1 3\n", 4, {1, 2, 3, 4}},
    {"4\n4 2 3 1\n", 4, {1, 2, 3, 4}},
    {"4\n4 3 1 2\n", 4, {1, 2, 3, 4}},
    {"4\n4 3 2 1\n", 4, {1, 2, 3, 4}},
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
    int i, passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 0; i < testInOut[testN].n; i++) {
        int n, nStatus = fscanf(out, "%d", &n);
        if (EOF == nStatus) {
            passed = 0;
            printf("output too short -- ");
            break;
        } else if (1 != nStatus) {
            passed = 0;
            printf("bad format -- ");
            break;
        } else if (testInOut[testN].out[i] != n) {
            passed = 0;
            printf("wrong output -- ");
            break;
        }
    }
    if (i >= testInOut[testN].n) {
        while (1) {
            char ignored;
            if (fscanf(out, "%c", &ignored) == EOF)
                break;
            if (strchr("\n\t ", ignored))
                continue;
            passed = 0;
            printf("output is too long -- ");
            break;
        }
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

static int feederBig(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    DWORD t;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large array... ");
    fflush(stdout);
    t = GetTickCount();
    fprintf(in, "%d\n", 2000000);
    for (i = 0; i < 2000000; i++) {
        if (fprintf(in, "%d ", i^0x1234567) == EOF) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fprintf(in, "\n");
    fclose(in);
    t = (tickDifference(t, GetTickCount())+999)/1000*1000;
    printf("done in T=%d seconds. Starting exe with timeout 2*T+3... ", t/1000);
    labTimeout = (int)t*2+3000;
    fflush(stdout);
    labOutOfMemory = 1024*1024+4*2000000;
    return 0;
}

static int checkerBig(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i, passed = 1, last = 0;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 0; i < 2000000; i++) {
        int n, nStatus = fscanf(out, "%d", &n);
        if (EOF == nStatus) {
            passed = 0;
            printf("output too short -- ");
            break;
        } else if (1 != nStatus) {
            passed = 0;
            printf("bad format -- ");
            break;
        } else if ((n^0x1234567) < 0 || 2000000 <= (n^0x1234567)) {
            passed = 0;
            printf("wrong output -- ");
            break;
        } else if (i == 0) {
            last = n;
            continue;
        } else if (n <= last ) {
            passed = 0;
            printf("wrong output -- ");
            break;
        }
    }
    if (passed) {
        while (1) {
            char ignored;
            if (fscanf(out, "%c", &ignored) == EOF)
                break;
            if (strchr("\n\t ", ignored))
                continue;
            passed = 0;
            printf("output is too long -- ");
            break;
        }
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

static int feederBig2(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    DWORD t;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large array... ");
    fflush(stdout);
    t = GetTickCount();
    fprintf(in, "%d\n", 2000000);
    for (i = 0; i < 200000; i++) {
        if (fprintf(in, "0 0 0 0 0 0 0 0 0 0 ") == EOF) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fprintf(in, "\n");
    fclose(in);
    t = (tickDifference(t, GetTickCount())+999)/1000*1000;
    printf("done in T=%d seconds. Starting exe with timeout 2*T+3... ", t/1000);
    labTimeout = (int)t*2+3000;
    fflush(stdout);
    labOutOfMemory = 1024*1024+4*2000000;
    return 0;
}

static int checkerBig2(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i, passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 0; i < 2000000; i++) {
        int n, nStatus = fscanf(out, "%d", &n);
        if (EOF == nStatus) {
            passed = 0;
            printf("output too short -- ");
            break;
        } else if (1 != nStatus) {
            passed = 0;
            printf("bad format -- ");
            break;
        } else if (n != 0) {
            passed = 0;
            printf("wrong output -- ");
            break;
        }
    }
    if (passed) {
        while (1) {
            char ignored;
            if (fscanf(out, "%c", &ignored) == EOF)
                break;
            if (strchr("\n\t ", ignored))
                continue;
            passed = 0;
            printf("output is too long -- ");
            break;
        }
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
    {feederBig, checkerBig},
    {feederBig2, checkerBig2}
};

const int labNTests = sizeof(labTests)/sizeof(labTests[0]);

const char labName[] = "Lab 3-x Quick Sort or Heap Sort";

int labTimeout = 3000;

size_t labOutOfMemory = 1024*1024;
