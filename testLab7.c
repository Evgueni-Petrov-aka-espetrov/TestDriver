#include "testLab.h"
#include <stdio.h>
#include <string.h>
static int ptr_size(void)
{
    static int x64 = -1;
    if (x64 == -1)
    {
        char *cpu = getenv("PROCESSOR_IDENTIFIER");
        x64 = cpu ? strstr(cpu, "x86") == NULL : 1;
    }
    return sizeof(int)*(x64+1);
}

static int up16(int n)
{
    return (1+(n-1)/16)*16;
}

enum testConst { N_MAX = 1000 };
static int testN = 0;
static const struct {
    int N, M;
    struct { int a, b; } G[8];
    const char *msg;
} testInOut[] = {
    {3, 2, {{1, 2}, {1, 3}}, NULL},
    {3, 3, {{1, 2}, {2, 3}, {3, 1}}, "impossible to sort"}, // 123
    {4, -1, {0}, "bad number of lines"},
    {3, 3, {{1, 2}, {1, 3}, {2, 3}}, NULL}, // 123
    {3, 3, {{1, 3}, {1, 2}, {3, 2}}, NULL}, // 132
    {3, 3, {{2, 1}, {2, 3}, {1, 3}}, NULL}, // 213
    {3, 3, {{2, 3}, {2, 1}, {3, 1}}, NULL}, // 231
    {3, 3, {{3, 1}, {3, 2}, {1, 2}}, NULL}, // 312
    {3, 3, {{3, 2}, {3, 1}, {2, 1}}, NULL}, // 321
    {-1, -1, {0}, "bad number of lines"},
    {3, 3, {{1, 3}, {3, 2}, {2, 1}}, "impossible to sort"}, // 132
    {3, 3, {{2, 1}, {1, 3}, {3, 2}}, "impossible to sort"}, // 213
    {3, 3, {{2, 3}, {3, 1}, {1, 2}}, "impossible to sort"}, // 231
    {3, 3, {{3, 1}, {1, 2}, {2, 3}}, "impossible to sort"}, // 312
    {3, 3, {{3, 2}, {2, 1}, {1, 3}}, "impossible to sort"}, // 321
    {2, 1, {0}, "bad number of lines"},
    {N_MAX, 1, {{N_MAX-1, N_MAX}}, NULL},
    {N_MAX, 0, {0}, NULL},
    {N_MAX+1, 1, {{1, 1}}, "bad number of vertices"},
    {0, 1, {0}, "bad number of edges"},
    {4, 6, {{1, 2}, {1, 3}, {1, 4}, {2, 3}, {2, 4}, {3, 4}}, NULL},
    {4, 4, {{1, 2}, {2, 3}, {1, 3}, {4, 3}}, NULL},
    {2, 4, {{1, 1}, {1, 2}, {2, 1}, {2, 2}}, "bad number of edges"},
    {N_MAX, 1, {{N_MAX, N_MAX}}, "impossible to sort"},
    {4, 4, {{1, 2}, {2, 3}, {3, 4}, {4, 1}}, "impossible to sort"},
    {5, 4, {{1, 2}, {2, 3}, {3, 1}, {4, 3}}, "impossible to sort"},
    {3, 2, {{1, 2}, {2, 4}}, "bad vertex"},
    {3, 2, {{1, 2}, {4, 2}}, "bad vertex"},
    {3, 2, {{1, 2}, {-1, 2}}, "bad vertex"},
    {3, 2, {{1, 2}, {2, -1}}, "bad vertex"},

};

static int feederN(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    if (testInOut[testN].N >= 0)
        fprintf(in, "%d\n", testInOut[testN].N);
    if (testInOut[testN].M >= 0)
        fprintf(in, "%d\n", testInOut[testN].M);
    for (i = 0; i < testInOut[testN].M && testInOut[testN].G[i].a != 0; i++) {
        fprintf(in, "%d %d\n", testInOut[testN].G[i].a, testInOut[testN].G[i].b);
    }
    fclose(in);
    return 0;
}


static int job_compare(const void *x, const void *y)
{
    const struct {int job, jobN;} *jobA = x, *jobB = y;
    return jobA->job < jobB->job ? -1 : jobA->job == jobB->job ? 0 : 1;
}

static int checkerN(void)
{
    FILE *const out = fopen("out.txt", "r");
    static const char pass[] = "PASSED", fail[] = "FAILED";
    const char *fact = pass;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    if (testInOut[testN].msg != NULL) { // test error message
        char bufMsg[128] = {0};
        if (fgets(bufMsg, sizeof(bufMsg), out) == NULL) {
            printf("output too short -- ");
            fact = fail;
        } else {
            if (strchr(bufMsg, '\n'))
                *strchr(bufMsg, '\n') = 0;
            if (strnicmp(testInOut[testN].msg, bufMsg, strlen(testInOut[testN].msg)) != 0) {
                printf("wrong output -- ");
                fact = fail;
            }
        }
    } else { // test order
        int i, N = testInOut[testN].N;
        struct {int job, jobN;} sorted[N_MAX];
        for (i = 0; i < N; i++) {
            const int status = fscanf(out, "%d", &sorted[i].job);
            if (status < 0) {
                printf("output too short -- ");
                fact = fail;
                break;
            } else if (status < 1) {
                printf("bad output format -- ");
                fact = fail;
                break;
            } else if (sorted[i].job < 1 || sorted[i].job > N) {
                printf("wrong output -- ");
                fact = fail;
                break;
            }
            sorted[i].job--;
            sorted[i].jobN = i;
        }
        if (fact == pass) {
            qsort(sorted, N, sizeof(struct {int job, jobN;}), job_compare); 
            for (i = 0; i < N; i++) {
                if (sorted[i].job != i) {
                    printf("wrong output -- ");
                    fact = fail;
                    break;
                }
            }
        }
        if (fact == pass) {
            for (i = 0; i < testInOut[testN].M; i++) {
                if (sorted[testInOut[testN].G[i].a-1].jobN >= sorted[testInOut[testN].G[i].b-1].jobN) {
                    printf("wrong output -- ");
                    fact = fail;
                    break;
                }
            }
        }
    }
    if (fact == pass) {
        while (1) {
            char c;
            int status = fscanf(out, "%c", &c);
            if (status < 0) {
                break;
            }
            if (!strchr(" \t\r\n", c)) {
                printf("garbage at the end -- ");
                fact = fail;
                break;
            }
        }
    }
    fclose(out);
    printf("%s\n", fact);
    testN++;
    return fact == fail;
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
        for (j = i+1; j < N_MAX; j++)
            fprintf(in, "%d %d\n", i+1, j+1);
    }
    fclose(in);
    labOutOfMemory = N_MAX*(N_MAX-1)/2*up16(ptr_size()*2)+1024*1024;
    return 0;
}


static int checkerBig(void)
{
    FILE *const out = fopen("out.txt", "r");
    static const char pass[] = "PASSED", fail[] = "FAILED";
    const char *fact = pass;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    { // test order
        int i, N = N_MAX;
        for (i = 0; i < N; i++) {
            int job, status = fscanf(out, "%d", &job);
            if (status < 0) {
                printf("output too short -- ");
                fact = fail;
                break;
            } else if (status < 1) {
                printf("bad output format -- ");
                fact = fail;
                break;
            } else if (job != i+1) {
                printf("wrong output -- ");
                fact = fail;
                break;
            }
        }
    }
    if (fact == pass) {
        while (1) {
            char c;
            int status = fscanf(out, "%c", &c);
            if (status < 0) {
                break;
            }
            if (!strchr(" \t\r\n", c)) {
                printf("garbage at the end -- ");
                fact = fail;
                break;
            }
        }
    }
    fclose(out);
    printf("%s\n", fact);
    testN++;
    return fact == fail;
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
        for (j = i+1; j < N_MAX; j++)
            fprintf(in, "%d %d\n", j+1, i+1);
    }
    fclose(in);
    labOutOfMemory = N_MAX*(N_MAX-1)/2*up16(ptr_size()*2)+1024*1024;
    return 0;
}


static int checkerBig1(void)
{
    FILE *const out = fopen("out.txt", "r");
    static const char pass[] = "PASSED", fail[] = "FAILED";
    const char *fact = pass;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    { // test order
        int i, N = N_MAX;
        for (i = 0; i < N; i++) {
            int job, status = fscanf(out, "%d", &job);
            if (status < 0) {
                printf("output too short -- ");
                fact = fail;
                break;
            } else if (status < 1) {
                printf("bad output format -- ");
                fact = fail;
                break;
            } else if (job != N_MAX-i) {
                printf("wrong output -- ");
                fact = fail;
                break;
            }
        }
    }
    if (fact == pass) {
        while (1) {
            char c;
            int status = fscanf(out, "%c", &c);
            if (status < 0) {
                break;
            }
            if (!strchr(" \t\r\n", c)) {
                printf("garbage at the end -- ");
                fact = fail;
                break;
            }
        }
    }
    fclose(out);
    printf("%s\n", fact);
    testN++;
    return fact == fail;
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

    {feederBig, checkerBig},
    {feederBig1, checkerBig1},
};

const int labNTests = sizeof(labTests)/sizeof(labTests[0]);

const char labName[] = "Lab 7 Topological Sort";

int labTimeout = 3000;
size_t labOutOfMemory = 1024*1024;
