#include "testLab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum testConst
{
    N_MAX = 2000,
};

static int testN = 0;

static const struct testInOut
{
    int N, M;
    struct edge
    {
        int from, to;
    } G[50];
    const char *msg;
    int cnt;
    int answer[N_MAX];

} testInOut[] = {
    /*1*/ {3, 2, {{1, 2}, {2, 3}}, NULL, 3, {1, 2, 3}},
    /*2*/ {3, 3, {{1, 2}, {2, 3}, {3, 1}}, NULL, 1, {1, 2, 3}},
    /*3*/ {3, 4, {{1, 2}, {2, 3}, {3, 1}, {1, 2}}, "Bad number of edges", 0, {0}},
    /*4*/ {3, 2, {{2, 1}, {3, 2}}, NULL, 3, {3, 2, 1}},
    /*5*/ {4, -1, {{0}}, "Bad number of lines", 0, {0}},
    /*6*/ {3, 2, {{1, 2}, {-1, 2}}, "Bad vertex", 0, {0}},
    /*7*/ {3, 2, {{1, 2}, {2, 4}}, "Bad vertex", 0, {0}},
    /*8*/ {3, 2, {{1, 2}, {2, -1}}, "Bad vertex", 0, {0}},
    /*9*/ {3, 2, {{1, 2}, {4, 2}}, "Bad vertex", 0, {0}},
    /*10*/ {2, 4, {{1, 1}, {1, 2}, {2, 1}, {2, 2}}, "Bad number of edges", 0, {0}},
    /*11*/ {2, 2, {{1, 1}, {1, 2}}, "Bad number of edges", 0, {0}},
    /*12*/ {N_MAX + 1, 1, {{1, 1}}, "Bad number of vertices", 0, {0}},
    /*13*/ {9, 26, {{1, 8}, {5, 3}, {2, 4}, {3, 6}, {3, 8}, {7, 5}, {6, 1}, {4, 7}, {1, 2}, {1, 5}, {6, 4}, {5, 8}, {7, 1}, {3, 7}, {3, 2}, {8, 9}, {2, 7}, {8, 7}, {4, 9}, {1, 9}, {4, 8}, {2, 5}, {5, 9}}, "Bad number of lines", 0, {0}},
    /*14*/ {4, 6, {{1, 2}, {1, 3}, {2, 3}, {3, 4}, {4, 2}, {4, 3}}, NULL, 2, {1, 2, 3, 4}},
    /*15*/ {5, 7, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 3}, {3, 1}, {5, 2}}, NULL, 1, {1, 2, 3, 4, 5}},
    /*16*/ {5, 6, {{1, 2}, {1, 5}, {2, 3}, {3, 4}, {4, 2}, {5, 4}}, NULL, 3, {1, 5, 2, 3, 4}},
    /*17*/ {6, 9, {{1, 2}, {2, 3}, {3, 1}, {1, 4}, {4, 5}, {5, 6}, {6, 4}, {2, 5}, {6, 3}}, NULL, 1, {1, 2, 3, 4, 5, 6}},
    /*18*/ {7, 11, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 6}, {1, 3}, {2, 4}, {5, 7}, {3, 6}}, NULL, 6, {1, 2, 3, 4, 5, 6, 7}},
    /*19*/ {8, 13, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 8}, {8, 6}, {1, 5}, {2, 6}, {3, 7}, {4, 8}, {5, 3}}, NULL, 4, {1, 2, 3, 4, 5, 6, 7, 8}},
    /*20*/ {9, 14, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 8}, {8, 9}, {9, 7}, {1, 3}, {2, 4}, {3, 5}, {4, 6}, {5, 7}}, NULL, 7, {1, 2, 3, 4, 5, 6, 7, 8, 9}},
    //Attention! This graph has more than one connectivity component*/ 
    /*21*/ {12, 13, {{2, 1}, {2, 4}, {2, 6}, {3, 6}, {4, 7}, {5, 2}, {5, 8}, {6, 2}, {7, 11}, {8, 11}, {10, 7}, {11, 9}, {12, 9}}, NULL, 11, {12, 10, 5, 8, 3, 2, 6, 4, 7, 11, 9, 1}},
    //Attention! This graph has more than one connectivity component*/ 
    /*22*/ {12, 20, {{1, 4}, {2, 4}, {2, 5}, {2, 6}, {2, 3}, {3, 6}, {4, 1}, {4, 5}, {4, 8}, {5, 4}, {7, 4}, {7, 11}, {8, 7}, {8, 5}, {8, 6}, {8, 11}, {8, 12}, {10, 7}, {11, 7}, {11, 12}}, NULL, 7, {10, 9, 2, 3, 1, 4, 5, 7, 8, 11, 12, 6}},
    /*23*/ {13, 18, {{1, 2}, {2, 3}, {3, 4}, {4, 5}, {5, 6}, {6, 7}, {7, 8}, {8, 9}, {9, 10}, {10, 11}, {11, 12}, {12, 13}, {13, 11}, {1, 7}, {2, 8}, {3, 9}, {4, 10}, {5, 11}}, NULL, 11, {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}},
    //Attention! This graph has more than one connectivity component*/ 
    /*24*/ {18, 27, {{1, 8}, {2, 9}, {3, 6}, {4, 3}, {6, 9}, {6, 10}, {7, 6}, {7, 4}, {8, 5}, {8, 15}, {9, 2}, {9, 6}, {9, 13}, {10, 9}, {10, 13}, {12, 8}, {12, 13}, {13, 12}, {13, 9}, {13, 10}, {14, 18}, {15, 16}, {16, 15}, {16, 17}, {17, 16}, {17, 15}, {18, 15}}, NULL, 11, {14, 18, 11, 7, 4, 3, 2, 6, 9, 10, 12, 13, 1, 8, 15, 16, 17, 5}},
    //Attention! This graph has more than one connectivity component*/ 
    /*25*/ {18, 28, {{1, 5}, {1, 8}, {1, 15}, {2, 3}, {4, 3}, {4, 7}, {5, 8}, {6, 3}, {6, 5}, {7, 3}, {8, 9}, {9, 6}, {9, 10}, {9, 12}, {10, 7}, {10, 9}, {10, 11}, {11, 4}, {11, 18}, {12, 8}, {13, 12}, {13, 17}, {14, 10}, {16, 13}, {16, 15}, {17, 13}, {17, 14}, {17, 18}}, NULL, 12, {16, 13, 17, 14, 2, 1, 15, 5, 6, 8, 9, 10, 12, 11, 18, 4, 7, 3}},
    //Attention! This graph has more than one connectivity component*/ 
    /*26*/ {18, 32, {{1, 5}, {1, 15}, {2, 5}, {2, 6}, {3, 4}, {4, 7}, {5, 1}, {5, 2}, {5, 8}, {6, 5}, {6, 7}, {8, 5}, {8, 12}, {8, 15}, {9, 10}, {9, 12}, {10, 7}, {11, 7}, {11, 14}, {12, 8}, {13, 9}, {13, 12}, {13, 14}, {13, 16}, {14, 10}, {14, 13}, {15, 8}, {15, 16}, {16, 12}, {17, 16}, {17, 18}, {18, 11}}, NULL, 10, {17, 18, 11, 13, 14, 9, 10, 3, 4, 1, 2, 5, 6, 8, 12, 15, 16, 7}},
    //Attention! This graph has more than one connectivity component*/ 
    /*27*/ {18, 34, {{1, 2}, {2, 5}, {2, 6}, {3, 2}, {3, 4}, {4, 18}, {5, 6}, {6, 7}, {6, 9}, {7, 4}, {7, 6}, {7, 9}, {8, 9}, {8, 12}, {8, 15}, {9, 5}, {9, 12}, {10, 7}, {10, 11}, {10, 13}, {11, 4}, {11, 10}, {11, 18}, {13, 12}, {13, 14}, {14, 13}, {14, 18}, {15, 8}, {15, 16}, {16, 12}, {16, 18}, {17, 13}, {17, 18}, {18, 11}}, NULL, 8, {17, 8, 15, 16, 3, 1, 2, 4, 5, 6, 7, 9, 10, 11, 13, 14, 18, 12}},
    //Attention! This graph has more than one connectivity component*/ 
    /*28*/ {18, 23, {{1, 8}, {2, 3}, {2, 5}, {2, 6}, {3, 6}, {4, 3}, {5, 1}, {6, 5}, {6, 7}, {7, 4}, {7, 6}, {7, 11}, {8, 1}, {8, 12}, {11, 10}, {13, 12}, {13, 16}, {13, 17}, {14, 17}, {14, 18}, {15, 8}, {16, 17}, {17, 16}}, NULL, 13, {15, 14, 18, 13, 16, 17, 9, 2, 3, 4, 6, 7, 11, 10, 5, 1, 8, 12}}};

static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;

static int FeedFromArray(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    if (!in)
    {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    if (testInOut[testN].N >= 0)
    {
        fprintf(in, "%d\n", testInOut[testN].N);
    }
    if (testInOut[testN].M >= 0)
    {
        fprintf(in, "%d\n", testInOut[testN].M);
    }
    for (i = 0; i < testInOut[testN].M && testInOut[testN].G[i].from != 0; i++)
    {
        fprintf(in, "%d %d\n", testInOut[testN].G[i].from, testInOut[testN].G[i].to);
    }
    fclose(in);
    LabMemoryLimit = testInOut[testN].N * 10 + testInOut[testN].M * 2 + MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int CheckFromArray(void)
{
    FILE *const out = fopen("out.txt", "r");
    const char *fact = Pass;

    if (!out)
    {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }

    if (testInOut[testN].msg != NULL)
    { // test error message
        char bufMsg[128] = {0};
        fact = ScanChars(out, sizeof(bufMsg), bufMsg);
        if (fact == Pass && _strnicmp(testInOut[testN].msg, bufMsg, strlen(testInOut[testN].msg)) != 0)
        {
            printf("wrong output -- ");
            fact = Fail;
        }
    }
    else
    { // test order
        int i, N = testInOut[testN].N, cnt = testInOut[testN].cnt;
        struct
        {
            int job, jobN;
        } answer[N_MAX];

        int check_cnt;
        fact = ScanInt(out, &check_cnt);
        if (check_cnt != cnt)
        {
            printf("wrong output -- ");
            fact = Fail;
        }
        else
        {
            for (i = 0; i < N; i++)
            {
                fact = ScanInt(out, &answer[i].job);

                if (fact == Fail)
                    break;

                if (answer[i].job < 1 || answer[i].job > N)
                {
                    printf("wrong output -- ");
                    fact = Fail;
                    break;
                }

                if (answer[i].job != testInOut[testN].answer[i])
                {
                    printf("wrong output -- ");
                    fact = Fail;
                    break;
                }

                answer[i].job--;
                answer[i].jobN = i;
            }
        }
    }

    if (fact == Pass && HaveGarbageAtTheEnd(out))
        fact = Fail;

    fclose(out);
    printf("%s\n", fact);
    testN++;
    return fact == Fail;
}

static int feederBig(void)
{
    FILE *const in = fopen("in.txt", "w+");

    if (!in)
    {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    fprintf(in, "%d\n", N_MAX);
    fprintf(in, "%d\n", N_MAX * (N_MAX - 1) / 2);

    for (int i = 0; i + 1 < N_MAX; i++)
    {
        for (int j = i + 1; j < N_MAX; j++)
            fprintf(in, "%d %d\n", i + 1, j + 1);
    }

    fclose(in);

    LabMemoryLimit = N_MAX * 10 + N_MAX * (N_MAX - 1) + MIN_PROCESS_RSS_BYTES;

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
        int N = N_MAX, check_cnt;
        fact = ScanInt(out, &check_cnt);

        if (check_cnt != N)
        {
            printf("wrong output -- ");
            fact = Fail;
        }
        else
        {
            for (int i = 1; i <= N; i++) {

                int job;
                fact = ScanInt(out, &job);

                if (fact == Fail)
                    break;

                if (job != i) {
                    printf("wrong output -- ");
                    fact = Fail;
                    break;
                }
            }
        }
    }

    if (fact == Pass && HaveGarbageAtTheEnd(out))
        fact = Fail;

    fclose(out);
    
    printf("%s\n", fact);

    testN++;

    return fact == Fail;
}

static int feederBig1(void)
{
    FILE *const in = fopen("in.txt", "w+");

    if (!in)
    {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    fprintf(in, "%d\n", N_MAX);
    fprintf(in, "%d\n", N_MAX * (N_MAX - 1) / 2);

    for (int i = 0; i + 1 < N_MAX; i++)
    {
        for (int j = i + 1; j < N_MAX; j++)
            fprintf(in, "%d %d\n", j + 1, i + 1);
    }

    fclose(in);

    LabMemoryLimit = N_MAX * 10 + N_MAX * (N_MAX - 1) + MIN_PROCESS_RSS_BYTES;

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
        int N = N_MAX, check_cnt;
        fact = ScanInt(out, &check_cnt);

        if (check_cnt != N)
        {
            printf("wrong output -- ");
            fact = Fail;
        }
        else
        {
            for (int i = N; i > 0; i--) {

                int job;
                fact = ScanInt(out, &job);

                if (fact == Fail)
                    break;

                if (job != i) {
                    printf("wrong output -- ");
                    fact = Fail;
                    break;
                }
            }
        }
    }

    if (fact == Pass && HaveGarbageAtTheEnd(out))
        fact = Fail;

    fclose(out);
    
    printf("%s\n", fact);

    testN++;

    return fact == Fail;
}

const TLabTest LabTests[] = {
    {FeedFromArray, CheckFromArray}, // 1
    {FeedFromArray, CheckFromArray}, // 2
    {FeedFromArray, CheckFromArray}, // 3
    {FeedFromArray, CheckFromArray}, // 4
    {FeedFromArray, CheckFromArray}, // 5
    {FeedFromArray, CheckFromArray}, // 6
    {FeedFromArray, CheckFromArray}, // 7
    {FeedFromArray, CheckFromArray}, // 8
    {FeedFromArray, CheckFromArray}, // 9
    {FeedFromArray, CheckFromArray}, // 10
    {FeedFromArray, CheckFromArray}, // 11
    {FeedFromArray, CheckFromArray}, // 12
    {FeedFromArray, CheckFromArray}, // 13
    {FeedFromArray, CheckFromArray}, // 14
    {FeedFromArray, CheckFromArray}, // 15
    {FeedFromArray, CheckFromArray}, // 16
    {FeedFromArray, CheckFromArray}, // 17
    {FeedFromArray, CheckFromArray}, // 18
    {FeedFromArray, CheckFromArray}, // 19
    {FeedFromArray, CheckFromArray}, // 20
    {FeedFromArray, CheckFromArray}, // 21
    {FeedFromArray, CheckFromArray}, // 22
    {FeedFromArray, CheckFromArray}, // 23
    {FeedFromArray, CheckFromArray}, // 24
    {FeedFromArray, CheckFromArray}, // 25
    {FeedFromArray, CheckFromArray}, // 26
    {FeedFromArray, CheckFromArray}, // 27
    {FeedFromArray, CheckFromArray}, // 28
    {feederBig, checkerBig},         // 29
    {feederBig1, checkerBig1}        // 30
};

TLabTest GetLabTest(int testIdx)
{
    return LabTests[testIdx];
}

int GetTestCount(void)
{
    return sizeof(LabTests) / sizeof(LabTests[0]);
}

const char *GetTesterName(void)
{
    return "Lab Kosorajo";
}

int GetTestTimeout(void)
{
    return 2000;
}

size_t GetTestMemoryLimit(void)
{
    return LabMemoryLimit;
}
