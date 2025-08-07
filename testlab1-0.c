#include "testLab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

static int testN = 0;
static const struct {const char *const in; int n, out[32];} testInOut[] = {
    {"example\nthis is simple example", 14, {7, 14, 13, 12, 11, 10, 20, 22, 21, 20, 19, 18, 17, 16}},
    {"x\n", 0, {0}},
    {"x\ny", 1, {1}},
    {"x\ny\n", 2, {1, 2}},
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
    {"\xE0\xE1\xE2\xE3\n\xE0\xE1\xE2\xE3\xE0\xE1\xE2\xE3", 8, {4, 3, 2, 1, 8, 7, 6, 5}}, // абвг\nабвгабвг
    {"\xE0\xE1 \xE2\xE3\n\xE0\xE1 \xE2\xE3\xE0\xE1\xE2\xE3", 5, {5, 4, 3, 2, 1}}, // аб вг\nаб вгабвг
    {"\xE0\xE1 \xE2\xE3\n\xE0\xE1\xE2\xE3\n\xE0\xE1 \xE2\xE3", 6, {5, 10, 9, 8, 7, 6}}, // аб вг\nабвг\nаб вг
    {"0123\n31230123", 8, {4, 3, 2, 1, 8, 7, 6, 5}},
    {"0123\n21230123", 8, {4, 3, 2, 1, 8, 7, 6, 5}},
    {"0123\n11230123", 8, {4, 3, 2, 1, 8, 7, 6, 5}},
    {"\xE0\xE1\xE2\xFF\n\xE0\xE1\xE2\xFF\xE0\xE1\xE2\xFF", 8, {4, 3, 2, 1, 8, 7, 6, 5}}, // абвя\nабвяабвя
};

static int FeedFromArray(void)
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

static int CheckFromArray(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i, passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 0; i < testInOut[testN].n; i++) {
        int n;
        if (ScanInt(out, &n) != Pass) {
            passed = 0;
            break;
        } else if (testInOut[testN].out[i] != n) {
            passed = 0;
            printf("wrong output -- ");
            break;
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

static int LabTimeout;

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
    for (i = 1; i < 1024*1024*8; i++) {
        if (fprintf(in, "0123456789abcde\n") == EOF) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fprintf(in, "0123456789abcdef");
    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2*T... ", (unsigned)t/1000);
    LabTimeout = (int)t*2;
    fflush(stdout);
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
        int n;
        if (ScanInt(out, &n) != Pass) {
            passed = 0;
            break;
        } else if (i != n) {
            passed = 0;
            printf("wrong output -- ");
            break;
        }
    }
    if (passed) {
        for (; i > 1024*1024*8*16-16; i--) {
            int n;
            if (ScanInt(out, &n) != Pass) {
                passed = 0;
            } else if (i != n) {
                passed = 0;
                printf("wrong output -- ");
                break;
            }
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
    for (i = 1; i < 1024*1024*12; i++) {
        if (fprintf(in, "0123456789abcdez") == EOF) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fprintf(in, "0123456789abcdef");
    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2*T... ", (unsigned)t/1000);
    LabTimeout = (int)t*2;
    fflush(stdout);
    return 0;
}

static int checkerBig1(void)
{
    FILE *const out = fopen("out.txt", "r");
    long long int i, passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 16; i < 1024ll*1024*12*16; i += 16) {
        int n;
        if (ScanInt(out, &n) != Pass) {
            passed = 0;
            break;
        } else if (i != n) {
            passed = 0;
            printf("wrong output -- ");
            break;
        }
    }
    if (passed) {
        for (; i > 1024ll*1024*12*16-16; i--) {
            int n;
            if (ScanInt(out, &n) != Pass) {
                passed = 0;
            } else if (i != n) {
                passed = 0;
                printf("wrong output -- ");
                break;
            }
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

static int feederBig2(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i, err;
    DWORD t;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large text... ");
    fflush(stdout);
    t = GetTickCount();
    fprintf(in, "abcdefghijklmnop\n");
    const char sel[] = " _\377";
    const size_t nsel = 1 + strlen(sel); // want use '\0' occasionally
    for (err = 0, i = 1; err >= 0 && i < 16*1000*1000; i++) {
        err = fputc(sel[rand() % nsel], in);
        if (err < 0) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2*T... ", (unsigned)t/1000);
    LabTimeout = (int)t*2;
    fflush(stdout);
    return 0;
}

static int checkerBig2(void)
{
    FILE *const out = fopen("out.txt", "r");
    long long int i, passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 16; i < 1000*1000*16; i += 16) {
        int n;
        if (ScanInt(out, &n) != Pass) {
            passed = 0;
            break;
        } else if (i != n) {
            passed = 0;
            printf("wrong output -- ");
            break;
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
    return "Lab 1-0 Boyer-Moore";
}

static int LabTimeout = 3000;
int GetTestTimeout(void) {
    return LabTimeout;
}

size_t GetTestMemoryLimit(void) {
    return MIN_PROCESS_RSS_BYTES;
}
