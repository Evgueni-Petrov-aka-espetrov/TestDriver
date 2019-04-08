#include "testLab.h"
#include <stdio.h>
#include <string.h>

static int testN = 0;
static const struct {const char *const in; int n, out[32];} testInOut[] = {
    {"example\nthis is simple example", 14, {7, 14, 13, 12, 11, 10, 20, 22, 21, 20, 19, 18, 17, 16}},
    {"x\n", 0, {0}},
    {"0123456789abcdef\n0123456789abcdef", 16, {16, 15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1}},
    {"0123\n01230123", 8, {4, 3, 2, 1, 8, 7, 6, 5}},
    {"0123\n0123x0123", 9, {4, 3, 2, 1, 8, 9, 8, 7, 6}},
    {"0123\n012300123", 9, {4, 3, 2, 1, 8, 9, 8, 7, 6}},
    {"0123\n012310123", 9, {4, 3, 2, 1, 8, 9, 8, 7, 6}},
    {"0123\n012320123", 9, {4, 3, 2, 1, 8, 9, 8, 7, 6}},
    {"0123\n012330123", 9, {4, 3, 2, 1, 8, 9, 8, 7, 6}},
    {"0123\n01220123", 6, {4, 5, 8, 7, 6, 5}},
    {"0123\n01210123", 6, {4, 6, 8, 7, 6, 5}},
    {"0123\n01200123", 6, {4, 7, 8, 7, 6, 5}},
    {"рсту\nрстурсту", 8, {4, 3, 2, 1, 8, 7, 6, 5}},
    {"рс ту\nрс турсту", 5, {5, 4, 3, 2, 1}},
    {"рс ту\nрсту\nрс ту", 6, {5, 10, 9, 8, 7, 6}},
    {"0123\n31230123", 8, {4, 3, 2, 1, 8, 7, 6, 5}},
    {"0123\n21230123", 8, {4, 3, 2, 1, 8, 7, 6, 5}},
    {"0123\n11230123", 8, {4, 3, 2, 1, 8, 7, 6, 5}}
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

static int feederBig(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    DWORD t;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large text... ");
    fflush(stdout);
    t = GetTickCount();
    fprintf(in, "0123456789abcdef\n");
    for (i = 1; i < 1024*1024*8; i++)
        if (fprintf(in, "0123456789abcde\n") == EOF) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    fprintf(in, "0123456789abcdef");
    t = (tickDifference(t, GetTickCount())+999)/1000*1000;
    printf("done in T=%d seconds. Starting exe with timeout 2*T... ", t/1000);
    labTimeout = 2*t;
    fflush(stdout);
    fclose(in);
    return 0;
}

static int checkerBig(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i, passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 16; i < 1024*1024*8*16; i += 16) {
        int n, nStatus = fscanf(out, "%d", &n);
        if (EOF == nStatus) {
            passed = 0;
            printf("output too short -- ");
            break;
        } else if (1 != nStatus) {
            passed = 0;
            printf("bad format -- ");
            break;
        } else if (i != n) {
            passed = 0;
            printf("wrong output -- ");
            break;
        }
    }
    if (passed) {
        for (; i > 1024*1024*8*16-16; i--) {
            int n, nStatus = fscanf(out, "%d", &n);
            if (EOF == nStatus) {
                passed = 0;
                printf("output too short -- ");
                break;
            } else if (1 != nStatus) {
                passed = 0;
                printf("bad format -- ");
                break;
            } else if (i != n) {
                passed = 0;
                printf("wrong output -- ");
                break;
            }
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
    {feederBig, checkerBig}
};

const int labNTests = sizeof(labTests)/sizeof(labTests[0]);

const char labName[] = "Lab 1-0 Boyer-Moore";

int labTimeout = 3000;
size_t labOutOfMemory = 1024*1024;

