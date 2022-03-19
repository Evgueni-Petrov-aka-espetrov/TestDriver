#include "testLab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum testConst {
    N_MAX = 2000,
};

static int testN = 0;

static const struct {
    int N, M;
    struct { int a, b; } G[28];
    const char *msg;
} testInOut[] = {
        {3, 2, {{1, 2}, {1, 3}}, NULL},
        {3, 3, {{1, 2}, {2, 3}, {3, 1}}, "impossible to sort"}, // 123
        {4, -1, {{0}}, "bad number of lines"},
        {5, 6, {{2, 1}, {3, 2}, {5, 3}, {4, 5}, {5, 2}, {4, 1}}, NULL},
        {2, 1, {{2, 1}}, NULL},
        {9, 23, {{1, 8}, {5, 3}, {2, 4}, {3, 6}, {3, 8}, {7, 5}, {6, 1}, {4, 7}, {1, 2}, {1, 5}, {6, 4}, {5, 8}, {7, 1}, {3, 7}, {3, 2}, {8, 9}, {2, 7}, {8, 7}, {4, 9}, {1, 9}, {4, 8}, {2, 5}, {5, 9}}, "impossible to sort"},
        {4, 1, {{2, 3}}, NULL},
        {4, 3, {{3, 4}, {4, 1}, {1, 3}}, "impossible to sort"},
        {8, 12, {{6, 7}, {2, 7}, {8, 7}, {5, 8}, {2, 6}, {6, 1}, {6, 4}, {1, 4}, {1, 7}, {6, 5}, {8, 2}, {7, 4}}, "impossible to sort"},
        {8, 13, {{3, 1}, {7, 4}, {8, 6}, {6, 3}, {5, 7}, {3, 7}, {7, 6}, {2, 3}, {8, 3}, {8, 2}, {4, 5}, {5, 2}, {5, 1}}, "impossible to sort"},
        {6, 10, {{5, 4}, {2, 4}, {6, 1}, {5, 2}, {6, 4}, {2, 1}, {5, 6}, {3, 1}, {6, 2}, {5, 1}}, NULL},
        {4, 2, {{4, 3}, {1, 4}}, NULL},
        {4, -1, {{0}}, "bad number of lines"},
        {9, 26, {{1, 8}, {5, 3}, {2, 4}, {3, 6}, {3, 8}, {7, 5}, {6, 1}, {4, 7}, {1, 2}, {1, 5}, {6, 4}, {5, 8}, {7, 1}, {3, 7}, {3, 2}, {8, 9}, {2, 7}, {8, 7}, {4, 9}, {1, 9}, {4, 8}, {2, 5}, {5, 9}}, "bad number of lines"},
        {8, 14, {{8, 3}, {8, 5}, {6, 5}, {5, 4}, {4, 1}, {6, 2}, {8, 6}, {7, 2}, {3, 4}, {3, 2}, {1, 5}, {2, 4}, {8, 7}, {2, 5}}, "impossible to sort"},
        {2, 1, {{1, 2}}, NULL},
        {5, 1, {{5, 3}}, NULL},
        {9, 14, {{4, 7}, {9, 6}, {6, 3}, {4, 2}, {1, 8}, {6, 8}, {5, 2}, {1, 6}, {5, 7}, {7, 2}, {2, 1}, {3, 5}, {4, 1}, {9, 8}}, "impossible to sort"},
        {3, 2, {{1, 3}, {2, 1}}, NULL},
        {4, 4, {{2, 4}, {1, 3}, {4, 3}, {1, 4}}, NULL},
        {5, 7, {{4, 2}, {2, 3}, {2, 1}, {4, 5}, {5, 3}, {5, 2}, {4, 1}}, NULL},
        {2, 4, {{1, 1}, {1, 2}, {2, 1}, {2, 2}}, "bad number of edges"},
        {N_MAX+1, 1, {{1, 1}}, "bad number of vertices"},
        {8, 16, {{6, 7}, {6, 4}, {8, 1}, {8, 3}, {2, 7}, {6, 3}, {2, 6}, {1, 7}, {2, 3}, {3, 7}, {4, 3}, {6, 5}, {7, 5}, {3, 1}, {1, 5}, {4, 2}}, "impossible to sort"},
        {6, 14, {{5, 1}, {5, 2}, {1, 6}, {5, 6}, {2, 6}, {1, 2}, {4, 3}, {6, 4}, {5, 3}, {5, 4}, {4, 1}, {3, 2}, {3, 1}, {6, 3}}, "impossible to sort"},
        {2, 1, {{1, 2}}, NULL},
        {4, 5, {{4, 1}, {2, 4}, {3, 1}, {3, 2}, {2, 1}}, NULL},
        {3, 1, {{1, 2}}, NULL},
        {3, 2, {{1, 2}, {4, 2}}, "bad vertex"},
        {5, 9, {{4, 3}, {4, 2}, {2, 1}, {1, 4}, {1, 3}, {2, 5}, {5, 3}, {3, 2}, {1, 5}}, "impossible to sort"},
        {7, 10, {{6, 1}, {7, 1}, {5, 4}, {7, 6}, {7, 3}, {2, 7}, {7, 4}, {2, 1}, {2, 3}, {1, 4}}, NULL},
        {2, 1, {{1, 2}}, NULL},
        {5, 10, {{2, 4}, {1, 4}, {1, 3}, {5, 2}, {5, 4}, {3, 5}, {2, 1}, {1, 5}, {2, 3}, {3, 4}}, "impossible to sort"},
        {2, 1, {{1, 2}}, NULL},
        {3, 2, {{1, 2}, {2, 4}}, "bad vertex"},
        {3, 2, {{1, 2}, {2, -1}}, "bad vertex"},
        {10, 28, {{3, 7}, {8, 7}, {9, 3}, {1, 4}, {2, 10}, {2, 6}, {10, 4}, {7, 5}, {5, 4}, {2, 9}, {8, 6}, {7, 6}, {1, 9}, {4, 9}, {10, 9}, {2, 3}, {9, 7}, {7, 2}, {2, 5}, {5, 8}, {6, 9}, {5, 3}, {6, 4}, {4, 8}, {2, 1}, {10, 7}, {5, 10}, {6, 10}}, "impossible to sort"},
        {10, 27, {{3, 10}, {2, 9}, {1, 5}, {7, 8}, {4, 3}, {2, 6}, {6, 3}, {2, 1}, {8, 4}, {5, 7}, {5, 4}, {8, 5}, {2, 10}, {8, 2}, {6, 9}, {3, 1}, {5, 9}, {6, 10}, {5, 3}, {1, 6}, {5, 2}, {9, 4}, {9, 8}, {2, 7}, {2, 3}, {9, 7}, {7, 1}}, "impossible to sort"},
        {10, 26, {{8, 6}, {4, 2}, {8, 10}, {5, 8}, {6, 3}, {10, 6}, {3, 4}, {9, 3}, {4, 7}, {3, 1}, {9, 8}, {10, 2}, {5, 1}, {6, 5}, {8, 4}, {9, 7}, {10, 3}, {5, 9}, {2, 1}, {9, 2}, {4, 6}, {5, 7}, {7, 3}, {6, 7}, {9, 6}, {10, 9}}, "impossible to sort"},
        {5, 6, {{4, 3}, {1, 4}, {5, 1}, {3, 5}, {1, 2}, {5, 2}}, "impossible to sort"},
        {8, 8, {{6, 3}, {4, 2}, {7, 4}, {1, 6}, {4, 1}, {2, 1}, {7, 6}, {7, 3}}, NULL},
        {3, 2, {{1, 2}, {-1, 2}}, "bad vertex"},
        {5, 6, {{1, 4}, {4, 5}, {2, 3}, {1, 3}, {5, 3}, {5, 2}}, NULL},
        {3, 2, {{3, 2}, {1, 3}}, NULL},
};


static size_t LabMemoryLimit;

static int FeedFromArray(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    if (testInOut[testN].N >= 0) {
        fprintf(in, "%d\n", testInOut[testN].N);
    }
    if (testInOut[testN].M >= 0) {
        fprintf(in, "%d\n", testInOut[testN].M);
    }
    for (i = 0; i < testInOut[testN].M && testInOut[testN].G[i].a != 0; i++) {
        fprintf(in, "%d %d\n", testInOut[testN].G[i].a, testInOut[testN].G[i].b);
    }
    fclose(in);
    LabMemoryLimit = testInOut[testN].N * 10 + testInOut[testN].M * 2 + MIN_PROCESS_RSS_BYTES;
    return 0;
}


static int job_compare(const void *x, const void *y)
{
    const struct {int job, jobN;} *jobA = x, *jobB = y;
    return jobA->job < jobB->job ? -1 : jobA->job == jobB->job ? 0 : 1;
}

static int CheckFromArray(void)
{
    FILE *const out = fopen("out.txt", "r");
    const char *fact = Pass;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    if (testInOut[testN].msg != NULL) { // test error message
        char bufMsg[128] = {0};
        fact = ScanChars(out, sizeof(bufMsg), bufMsg);
        if (fact == Pass && _strnicmp(testInOut[testN].msg, bufMsg, strlen(testInOut[testN].msg)) != 0) {
            printf("wrong output -- ");
            fact = Fail;
        }
    } else { // test order
        int i, N = testInOut[testN].N;
        struct {int job, jobN;} sorted[N_MAX];
        for (i = 0; i < N; i++) {
            fact = ScanInt(out, &sorted[i].job);
            if (fact == Fail) {
                break;
            }
            if (sorted[i].job < 1 || sorted[i].job > N) {
                printf("wrong output -- ");
                fact = Fail;
                break;
            }
            sorted[i].job--;
            sorted[i].jobN = i;
        }
        if (fact == Pass) {
            qsort(sorted, (size_t)N, sizeof(struct {int job, jobN;}), job_compare);
            for (i = 0; i < N; i++) {
                if (sorted[i].job != i) {
                    printf("wrong output -- ");
                    fact = Fail;
                    break;
                }
            }
        }
        if (fact == Pass) {
            for (i = 0; i < testInOut[testN].M; i++) {
                if (sorted[testInOut[testN].G[i].a-1].jobN >= sorted[testInOut[testN].G[i].b-1].jobN) {
                    printf("wrong output -- ");
                    fact = Fail;
                    break;
                }
            }
        }
    }
    if (fact == Pass && HaveGarbageAtTheEnd(out)) {
        fact = Fail;
    }
    fclose(out);
    printf("%s\n", fact);
    testN++;
    return fact == Fail;
}

static int feederBig(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "%d\n", N_MAX);
    fprintf(in, "%d\n", N_MAX*(N_MAX-1)/2);
    for (i = 0; i+1 < N_MAX; i++) {
        int j;
        for (j = i+1; j < N_MAX; j++) {
            fprintf(in, "%d %d\n", i+1, j+1);
        }
    }
    fclose(in);
    LabMemoryLimit = N_MAX * 10 + N_MAX * (N_MAX-1) + MIN_PROCESS_RSS_BYTES;
    return 0;
}


static int checkerBig(void)
{
    FILE *const out = fopen("out.txt", "r");
    const char *fact = Pass;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    { // test order
        int i, N = N_MAX;
        for (i = 0; i < N; i++) {
            int job;
            fact = ScanInt(out, &job);
            if (fact == Fail) {
                break;
            }
            if (job != i+1) {
                printf("wrong output -- ");
                fact = Fail;
                break;
            }
        }
    }
    if (fact == Pass && HaveGarbageAtTheEnd(out)) {
        fact = Fail;
    }
    fclose(out);
    printf("%s\n", fact);
    testN++;
    return fact == Fail;
}

static int feederBig1(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "%d\n", N_MAX);
    fprintf(in, "%d\n", N_MAX*(N_MAX-1)/2);
    for (i = 0; i+1 < N_MAX; i++) {
        int j;
        for (j = i+1; j < N_MAX; j++) {
            fprintf(in, "%d %d\n", j+1, i+1);
        }
    }
    fclose(in);
    LabMemoryLimit = N_MAX * 10 + N_MAX * (N_MAX-1) + MIN_PROCESS_RSS_BYTES;
    return 0;
}


static int checkerBig1(void)
{
    FILE *const out = fopen("out.txt", "r");
    const char *fact = Pass;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    { // test order
        int i, N = N_MAX;
        for (i = 0; i < N; i++) {
            int job;
            fact = ScanInt(out, &job);
            if (fact == Fail) {
                break;
            }
            if (job != N_MAX-i) {
                printf("wrong output -- ");
                fact = Fail;
                break;
            }
        }
    }
    if (fact == Pass && HaveGarbageAtTheEnd(out)) {
        fact = Fail;
    }
    fclose(out);
    printf("%s\n", fact);
    testN++;
    return fact == Fail;
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
    return "Lab 7 Topological Sort";
}

int GetTestTimeout() {
    return 6000;
}

static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;
size_t GetTestMemoryLimit() {
    return LabMemoryLimit;
}
