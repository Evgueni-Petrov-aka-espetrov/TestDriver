#include "testLab.h"
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

enum testConst { N_MAX = 5000 };
static int testN = 0;
struct edge {int a, b; int64_t abL;};
static const struct {
    int N, M;
    struct edge G[8];
    const char *msg;
    int64_t L;
} testInOut[] = {
    {3, 3, {{1, 2, 10}, {2, 3, 5}, {3, 1, 5}}, NULL, 10},
    {3, 1, {{1, 2, 10}}, "no spanning tree"},
    {2, 1, {{0}}, "bad number of lines"},

    {0, 0, {{0}}, "no spanning tree"},
    {N_MAX+1, 1, {{1, 1, 1}}, "bad number of vertices"},
    {2, 4, {{1, 1, 1}, {1, 2, 1}, {2, 1, 1}, {2, 2, 1}}, "bad number of edges"},
    {2, 1, {{1, 2, -1}}, "bad length"},

    {2, 0, {{0}}, "no spanning tree"},
    {2, 1, {{1, 2, (int64_t)4*INT_MAX}}, "bad length"},
    {4, 2, {{1, 2, INT_MAX}, {2, 3, INT_MAX}}, "no spanning tree"},
    {2, 1, {{1, 1, INT_MAX}}, "no spanning tree"},

    {1, 0, {{0}}, NULL, 0},
    {4, 4, {{1, 2, 1}, {2, 3, 2}, {3, 4, 4}, {4, 1, 8}}, NULL, 7},
    {3, 2, {{1, 2, INT_MAX}, {2, 3, INT_MAX}}, NULL, (int64_t)2*INT_MAX},
    {3, 3, {{1, 2, INT_MAX}, {2, 3, INT_MAX}, {1, 3, 1}}, NULL, (int64_t)1+INT_MAX},

    {4, 4, {{1, 2, INT_MAX}, {2, 3, INT_MAX}, {3, 4, INT_MAX}, {4, 1, INT_MAX}}, NULL, (int64_t)3*INT_MAX},
    {4, 4, {{1, 2, 1}, {2, 3, 1}, {3, 4, 1}, {4, 1, 1}}, NULL, 3},
    {5, 4, {{1, 2, 1}, {2, 3, 1}, {3, 1, 1}, {4, 3, 1}}, "no spanning tree"},
    {4, 6, {{1, 2, 1}, {1, 3, 2}, {1, 4, 4}, {2, 3, 8}, {2, 4, 16}, {3, 4, 32}}, NULL, 7},
    
    {3, 2, {{1, 2, 1}, {2, 4, 1}}, "bad vertex"},
    {3, 2, {{1, 2, 1}, {4, 2, 1}}, "bad vertex"},
    {3, 2, {{1, 2, 1}, {-1, 2, 1}}, "bad vertex"},
    {3, 2, {{1, 2, 1}, {2, -1, 1}}, "bad vertex"},

    {4, 4, {{1, 2, 1}, {2, 3, 2}, {1, 3, 3}, {4, 3, 4}}, NULL, 7},
    {4, 3, {{1, 2, 1}, {3, 4, 2}, {2, 4, 3}}, NULL, 6},
    {6, 6, {{1, 2, 1}, {2, 3, 2}, {4, 5, 3}, {5, 6, 4}, {3, 4, 5}, {1, 6, 6}}, NULL, 15},

    {6, 6, {{1, 2, 1}, {3, 4, 2}, {5, 6, 3}, {2, 3, 4}, {4, 5, 5}, {1, 6, 6}}, NULL, 15},
};

static int feederN(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "%d\n%d\n", testInOut[testN].N, testInOut[testN].M);
    for (i = 0; i < testInOut[testN].M && testInOut[testN].G[i].a != 0; i++) {
        fprintf(in, "%d %d %" PRIi64 "\n",
            testInOut[testN].G[i].a, testInOut[testN].G[i].b, testInOut[testN].G[i].abL);
    }
    fclose(in);
    return 0;
}


static int find(int a, int b, int M, const struct edge G[])
{
    int i;
    for (i = 0; i < M; i++) {
        if (G[i].a == a && G[i].b == b) {
            return i;
        }
        if (G[i].a == b && G[i].b == a) {
            return i;
        }
    }
    return -1;
}

static int root(int v, const int parent[N_MAX])
{
    while (1) {
        const int xv = v;
        v = parent[v];
        if (v == xv) {
            return v;
        }
    }
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
            if (_strnicmp(testInOut[testN].msg, bufMsg, strlen(testInOut[testN].msg)) != 0) {
                printf("wrong output -- ");
                fact = fail;
            }
        }
    } else { // test spanning tree
        int i, N = testInOut[testN].N;
        int parent[N_MAX];
        int64_t L = 0;
        for (i = 0; i < N; i++) parent[i] = i;
        for (i = 0; i < N-1; i++) {
            int a, b, status = fscanf(out, "%d%d", &a, &b);
            if (status < 0) {
                printf("output too short -- ");
                fact = fail;
                break;
            } else if (status < 2) {
                printf("bad output format -- ");
                fact = fail;
                break;
            } else {
                const int indexAB = find(a, b, testInOut[testN].M, testInOut[testN].G);
                const int rootA = root(a-1, parent), rootB = root(b-1, parent);
                if (indexAB < 0 || rootA == rootB) {
                    printf("wrong output -- ");
                    fact = fail;
                    break;
                }
                parent[rootA] = rootB;
                L += testInOut[testN].G[indexAB].abL;
            }
        }
        if (fact == pass) {
            int nRoot = 0; 
            for (i = 0; i < N; i++) {
                if (parent[i] == i) {
                    nRoot++;
                }
            }
            if (nRoot != 1 || L > testInOut[testN].L) {
                printf("wrong output -- ");
                fact = fail;
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
    fprintf(in, "%d\n%d\n", N_MAX, N_MAX);
    for (i = 1; i <= N_MAX/2; i++) {
        fprintf(in, "%d %d %d\n", 2*i-1, 2*i, i);
    }
    for (i = 1; i+1 <= N_MAX/2; i++) {
        fprintf(in, "%d %d %d\n", 2*i, 2*i+1, N_MAX/2+i);
    }
    fprintf(in, "%d %d %d\n", N_MAX, 1, N_MAX);
    fclose(in);
    labOutOfMemory = N_MAX*N_MAX*4+1024*1024;
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
    { // test spanning tree
        int i, N = N_MAX;
        int parent[N_MAX];
        int64_t L = 0;
        for (i = 0; i < N; i++) parent[i] = i;
        for (i = 0; i < N-1; i++) {
            int a, b, status = fscanf(out, "%d%d", &a, &b);
            if (status < 0) {
                printf("output too short -- ");
                fact = fail;
                break;
            } else if (status < 2) {
                printf("bad output format -- ");
                fact = fail;
                break;
            } else if (a < 1 || N_MAX < a || b < 1 || N_MAX < b) {
                printf("wrong output -- ");
                fact = fail;
                break;
            } else if (abs(a-b) != 1 && !(a == 1 && b == N_MAX) && !(b == 1 && a == N_MAX)) {
                printf("wrong output -- ");
                fact = fail;
                break;
            } else {
                const int rootA = root(a-1, parent), rootB = root(b-1, parent);
                L += a%2 == 1 ? b/2 : a < N_MAX ? b : N_MAX;
                if (rootA == rootB) {
                    printf("wrong output -- ");
                    fact = fail;
                    break;
                }
                parent[rootA] = rootB;
            }
        }
        if (fact == pass) {
            int nRoot = 0; 
            for (i = 0; i < N; i++) {
                if (parent[i] == i) {
                    nRoot++;
                }
            }
            if (nRoot != 1 || L > N_MAX*(N_MAX-1)/2) {
                printf("wrong output -- ");
                fact = fail;
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
    fprintf(in, "%d\n%d\n", N_MAX, N_MAX-2+N_MAX-3+1);
    for (i = 1; i <= N_MAX-2; i++) { // N_MAX not reached
        fprintf(in, "%d %d %d\n", i, i+1, i);
    }
    for (i = 3; i <= N_MAX-1; i++) { // many edges for kruskal to delete, N_MAX not reached
        fprintf(in, "%d %d %d\n", 1, i, N_MAX-2+i);
    }
    fprintf(in, "%d %d %d\n", N_MAX-1, N_MAX, 2*N_MAX-2); // reach 5000
    fclose(in);
    labOutOfMemory = N_MAX*N_MAX*4+1024*1024;
    return 0;
}

static int feederBig10(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "%d\n%d\n", N_MAX, N_MAX-2+N_MAX-3+1);
    for (i = 1; i <= N_MAX-2; i++) { // N_MAX not reached
        fprintf(in, "%d %d %d\n", i+1, i, i);
    }
    for (i = 3; i <= N_MAX-1; i++) { // many edges for kruskal to delete, N_MAX not reached
        fprintf(in, "%d %d %d\n", i, 1, N_MAX-2+i);
    }
    fprintf(in, "%d %d %d\n", N_MAX, N_MAX-1, 2*N_MAX-2); // reach 5000
    fclose(in);
    labOutOfMemory = N_MAX*N_MAX*4+1024*1024;
    return 0;
}

static int subG1(int a, int b, int * abL)
{
    const int minAB = a < b ? a : b, maxAB = b+a-minAB;
    if (1 <= minAB && minAB <= N_MAX-2 && maxAB == minAB+1) {
        *abL = minAB;
    } else {
        *abL = -1;
    }
    return *abL >= 0;
}

static int subG2(int a, int b, int * abL)
{
    const int minAB = a < b ? a : b, maxAB = b+a-minAB;
    if (minAB == 1 && 3 <= maxAB && maxAB <= N_MAX-1) {
        *abL = N_MAX-2+maxAB;
    } else {
        *abL = -1;
    }
    return *abL >= 0;
}

static int subG3(int a, int b, int * abL)
{
    const int minAB = a < b ? a : b, maxAB = b+a-minAB;
    if (minAB == N_MAX-1 && maxAB == N_MAX) {
        *abL = 2*N_MAX-2;
    } else {
        *abL = -1;
    }
    return *abL >= 0;
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
    { // test spanning tree
        int i, N = N_MAX;
        int parent[N_MAX];
        int64_t L = 0;
        for (i = 0; i < N; i++) parent[i] = i;
        for (i = 0; i < N-1; i++) {
            int a, b, status = fscanf(out, "%d%d", &a, &b), abL;
            if (status < 0) {
                printf("output too short -- ");
                fact = fail;
                break;
            } else if (status < 2) {
                printf("bad output format -- ");
                fact = fail;
                break;
            } else if (a < 1 || N_MAX < a || b < 1 || N_MAX < b) {
                printf("wrong output -- ");
                fact = fail;
                break;
            } else if (!subG1(a, b, &abL) && !subG2(a, b, &abL) && !subG3(a, b, &abL)) {
                printf("wrong output -- ");
                fact = fail;
                break;
            } else {
                const int rootA = root(a-1, parent), rootB = root(b-1, parent);
                if (rootA == rootB) {
                    printf("wrong output -- ");
                    fact = fail;
                    break;
                }
                L += abL;
                parent[rootA] = rootB;
            }
        }
        if (fact == pass) {
            int nRoot = 0; 
            for (i = 0; i < N; i++) {
                if (parent[i] == i) {
                    nRoot++;
                }
            }
            if (nRoot != 1 || L > (N_MAX-1)*(N_MAX-2)/2+2*N_MAX-2) {
                printf("wrong output -- ");
                fact = fail;
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

static int feederBig2(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i, j;
    DWORD tStart;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large text... ");
    fflush(stdout);
    tStart = GetTickCount();
    fprintf(in, "%d\n%d\n", N_MAX, N_MAX-2+((N_MAX-1)/5-2)*((N_MAX-1-3+1)+(N_MAX-1-(N_MAX-1)/5+1))/2+1);
    for (i = 1; i <= N_MAX-2; i++) { // N_MAX not reached
        fprintf(in, "%d %d %d\n", i, i+1, i);
    }
    for (j = 1; j+2 <= (N_MAX-1)/5; j++) { // many edges for kruskal to delete, N_MAX not reached
        for (i = j+2; i <= N_MAX-1; i++) {
            if (fprintf(in, "%d %d %d\n", j, i, N_MAX-2+1) < 3) {
                printf("can't create in.txt. No space on disk?\n");
                fclose(in);
                return -1;
            }
        }
    }
    fprintf(in, "%d %d %d\n", N_MAX-1, N_MAX, N_MAX); // reach N_MAX
    tStart = (tickDifference(tStart, GetTickCount())+999)/1000*1000;
    printf("done in T=%u seconds. Starting exe with timeout T+3... ", (unsigned)tStart/1000);
    labTimeout = (int)tStart+3000;
    fflush(stdout);
    fclose(in);
    labOutOfMemory = N_MAX*N_MAX*4+1024*1024;
    return 0;
}

static int checkerBig2(void)
{
    FILE *const out = fopen("out.txt", "r");
    static const char pass[] = "PASSED", fail[] = "FAILED";
    const char *fact = pass;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    { // test spanning tree
        int i, N = N_MAX;
        int parent[N_MAX];
        int64_t L = 0;
        for (i = 0; i < N; i++) parent[i] = i;
        for (i = 0; i < N-1; i++) {
            int a, b, status = fscanf(out, "%d%d", &a, &b);
            if (status < 0) {
                printf("output too short -- ");
                fact = fail;
                break;
            } else if (status < 2) {
                printf("bad output format -- ");
                fact = fail;
                break;
            } else if (a < 1 || N_MAX < a || b < 1 || N_MAX < b) {
                printf("wrong output -- ");
                fact = fail;
                break;
            } else if ((a == N_MAX && b != N_MAX-1) || (b == N_MAX && a != N_MAX-1)) {
                printf("wrong output -- ");
                fact = fail;
                break;
            } else {
                const int rootA = root(a-1, parent), rootB = root(b-1, parent);
                int abL = a+1 == b ? a
                    : b+1 == a ? b
                    : a == N_MAX || b == N_MAX ? N_MAX
                    : N_MAX-2+1;
                if (rootA == rootB) {
                    printf("wrong output -- ");
                    fact = fail;
                    break;
                }
                L += abL;
                parent[rootA] = rootB;
            }
        }
        if (fact == pass) {
            int nRoot = 0; 
            for (i = 0; i < N; i++) {
                if (parent[i] == i) {
                    nRoot++;
                }
            }
            if (nRoot != 1 || L > (N_MAX-1)*(N_MAX-2)/2+N_MAX) {
                printf("wrong output -- ");
                fact = fail;
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

    {feederBig, checkerBig},
    {feederBig1, checkerBig1},
    {feederBig10, checkerBig1},
    {feederBig2, checkerBig2},
};

const int labNTests = sizeof(labTests)/sizeof(labTests[0]);

const char labName[] = "Lab 8-x Kruskal or Prim Shortest Spanning Tree";

int labTimeout = 3000;
size_t labOutOfMemory = 1024*1024;

