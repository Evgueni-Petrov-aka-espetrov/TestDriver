#include "testLab.h"
#include <stdio.h>

static int testN = 0;
static int seed[] = {1234u, 383u};
static int LabTimeout;

static const struct {const char *const in; int n; int out[64];} testInOut[] = {
    {"example\nthis is simple example", 11, { 0, 0, 0, 0, 0, 0, 1, 14, 1, 16, 7}},
    {"x\n", 1, {0}},
    {"x\noasdkxmnosaxkmxokasmdx", 9, {0,6, 1, 12, 1, 15, 1, 22, 1}},
    {"0123456789abcdef\n0123456789abcdef", 18, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 16}},
    {"0123\n01230123", 8, {0, 0, 0, 0, 1, 4 ,5 ,4}},
    {"0123\n0123x0123", 8, {0, 0, 0, 0, 1, 4 ,6 ,4}},
    {"0123\n012300123", 10, {0, 0, 0, 0, 1, 4 ,5, 1 ,6 ,4}},
    {"012013\n0123101201343", 10, {0, 0, 0, 1, 2, 0, 1, 3, 6, 6}},
    {"abcabc\nabcabcabcabcabc", 14, {0, 0, 0, 1, 2, 3, 1, 6, 4, 6, 7, 6, 10, 6}},
    {"0123456789abcdef\n01234", 16, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}},
    {"0101230101234\n0101010122301341010101230101234", 25, {0, 0, 1, 2, 0, 0, 1, 2, 3, 4, 5, 6,  0, 1, 4, 3, 4, 5, 5, 12, 2, 17, 4, 19, 13}},
    {"\nasdbaf", 0, {0}},
    {"\xE0\xE1\xE2\xE3\n\xE0\xE1\xE2\xE3\xE0\xE1\xE2\xE3",8,{0, 0, 0, 0, 1, 4, 5, 4}}, // абвг\nабвгабвг
    {"\xE0\xE1 \xE2\xE3\n\xE0\xE1 \xE2\xE3\xE0\xE1\xE2\xE3", 7, {0, 0, 0, 0, 0, 1, 5}}, // аб вг\nаб вгабвг
    {"\xE0\xE1 \xE2\xE3\n\xE0\xE1\xE2\xE3\n\xE0\xE1 \xE2\xE3", 9, {0, 0, 0, 0, 0, 1, 2, 6, 5}}, // аб вг\nабвг\nаб вг
    {"000001\n000000000010", 18,{0, 1, 2, 3, 4, 0, 1, 5, 2, 5, 3, 5, 4, 5, 5, 5, 6, 6}}
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
        if (ScanInt(out, &n) != Pass || testInOut[testN].out[i] != n) {
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

static int FeederBigRand1(void)
{
    FILE *const in = fopen("in.txt", "w");
    DWORD t;
    unsigned int bigTestN = testN - sizeof(testInOut) / sizeof(testInOut[0]);
    char pattern[] = "0123456789abcdef";
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large text with seed %u... ", seed[bigTestN]);
    fflush(stdout);
    srand(seed[bigTestN]);
    t = GetTickCount();
    if(fputs("0123456789abcdef\n", in) == EOF) {
        printf("can't write in in.txt. No space on disk?\n");
        return -1;
    }
    unsigned int r;
    for(unsigned int i = 1; i < 1024*1024*8; i++) {
        r = (unsigned int)rand()%17;
        if(r == 0) {
            if(putc(' ', in) == EOF) {
                printf("can't write in in.txt. No space on disk?\n");
                return -1;
            }
        }else {
            if (fwrite(pattern, 1, r, in) != r) {
                printf("can't write in in.txt. No space on disk?\n");
                return -1;
            }
        }

    }
    if(fputs("0123456789abcdef", in) == EOF) {
        printf("can't write in in.txt. No space on disk?\n");
        return -1;
    }
    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 10*T... ", (unsigned)t/1000);
    LabTimeout = (int)t*10;
    fflush(stdout);
    return 0;
}

static int CheckerBigRand1(void)
{
    FILE *const out = fopen("out.txt", "r");
    unsigned int passed = 1;
    unsigned int bigTestN = testN - sizeof(testInOut) / sizeof(testInOut[0]);
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for(unsigned int i = 0; i < 16; i++) {
        int n;
        if(ScanInt(out, &n) != Pass || n != 0) {
            passed = 0;
            printf("wrong output -- ");
            break;
        }
    }
    if(passed) {
        srand(seed[bigTestN]);
        unsigned int number = 1;
        unsigned int a, b;
        for (unsigned int i = 1; i < 1024 * 1024 * 8; i++) {
            unsigned int r = ((unsigned int) rand()) % 17;
            if (r == 0) {
                number++;
            } else {
                if (ScanUintUint(out, &a, &b) != Pass || a != number || b != r) {
                    passed = 0;
                    printf("wrong output -- ");
                    break;
                }
                number += r;
            }
        }
        if (ScanUintUint(out, &a, &b) != Pass) {
            passed = 0;
        } else if (a != number || b != 16) {
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

static int FeederBig(void)
{
    FILE *const in = fopen("in.txt", "w");
    DWORD t;
    char str[] = "abcdabcd\n";
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large text... ");
    fflush(stdout);
    t = GetTickCount();

    if(fputs(str, in) == EOF) {
        printf("can't write in in.txt. No space on disk?\n");
        return -1;
    }
    for(int i = 0; i < 1024*1024*8; i++) {
        if(fwrite(str, 1, 8, in) != 8) {
            printf("can't write in in.txt. No space on disk?\n");
            return -1;
        }
    }
    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 15*T... ", (unsigned)t/1000);
    LabTimeout = (int)t*15;
    fflush(stdout);
    return 0;
}

static int CheckBig(void) {
    FILE *const out = fopen("out.txt", "r");
    unsigned int passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }

    for(unsigned int i = 0; i < 4; i++) {
        int n;
        if(ScanInt(out, &n) != Pass) {
            passed = 0;
            printf("wrong output -- ");
            break;
        }else if(n != 0) {
            passed = 0;
            break;
        }
    }

    for(unsigned int i = 1; i < 5; i++) {
        int n;
        if(ScanInt(out, &n) != Pass || n != i) {
            passed = 0;
            printf("wrong output -- ");
            break;
        }
    }

    if(passed) {
        unsigned int number = 1;
        unsigned int a, b;
        for (unsigned int i = 0; i < 1024 * 1024 * 8 * 2 - 1; i++) {
            if (ScanUintUint(out, &a, &b) != Pass || a != number || b != 8) {
                passed = 0;
                printf("wrong output -- ");
                break;
            }
            number += 4;
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
    {FeederBigRand1, CheckerBigRand1},
    {FeederBigRand1, CheckerBigRand1},
    {FeederBig, CheckBig}
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return sizeof(LabTests)/sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
    return "Lab 1-2 Knuth–Morris–Pratt";
}

static int LabTimeout = 3000;
int GetTestTimeout() {
    return LabTimeout;
}

size_t GetTestMemoryLimit() {
    return MIN_PROCESS_RSS_BYTES;
}
