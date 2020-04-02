#include "testLab.h"
#include <stdio.h>
#include <string.h>

static int testN = 0;
static const struct {const char *const in, *const out1, *const out2, *const out20;} testInOut[] = {
    {"0\n0 0\n0\n", "bad vertex"},
    {"5001\n1 1\n1\n1 1 1\n", "bad number of vertices"},
    {"2\n2 2\n4\n1 1 1\n1 2 1\n2 1 1\n2 2 1\n", "bad number of edges"},
    {"2\n1 2\n1\n1 2 -1\n", "bad length"},
    {"2\n1 2\n0\n", "0 oo", "no path"},
    {"3\n1 3\n2\n1 2 2147483647\n2 3 2147483647\n", "0 2147483647 INT_MAX+", "3 2 1"},
    {"3\n1 3\n3\n1 2 2147483647\n2 3 2147483647\n1 3 1\n", "0 2147483647 1", "3 1"},
    {"4\n1 3\n4\n1 2 2147483647\n2 3 2147483647\n3 4 2147483647\n4 1 2147483647\n", "0 2147483647 INT_MAX+ 2147483647", "overflow"},
    {"2\n1 2\n1\n1 2 2147483648\n", "bad length"},
    {"4\n1 3\n2\n1 2 2147483647\n2 3 2147483647\n", "0 2147483647 INT_MAX+ oo", "3 2 1"},
    {"1\n1 1\n0\n", "0", "1"},
    {"2\n1 1\n1\n1 1 2147483647", "0 oo", "1"},
    {"2\n2 2\n1\n1 1 2147483647", "oo 0", "2"},
    {"4\n1 2\n4\n1 2 2147483647\n2 3 2147483647\n3 4 2147483647\n4 1 2147483647\n", "0 2147483647 INT_MAX+ 2147483647", "2 1"},
    {"4\n3 3\n4\n1 2 2147483647\n2 3 2147483647\n3 4 2147483647\n4 1 2147483647\n", "INT_MAX+ 2147483647 0 2147483647", "3"},

    {"4\n1 1\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "0 1 3 7", "1"},
    {"4\n1 2\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "0 1 3 7", "2 1"},
    {"4\n1 3\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "0 1 3 7", "3 2 1"},
    {"4\n1 4\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "0 1 3 7", "4 3 2 1"},

    {"4\n2 1\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "1 0 2 6", "1 2"},
    {"4\n2 2\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "1 0 2 6", "2"},
    {"4\n2 3\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "1 0 2 6", "3 2"},
    {"4\n2 4\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "1 0 2 6", "4 3 2"},

    {"4\n3 1\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "3 2 0 4", "1 2 3"},
    {"4\n3 2\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "3 2 0 4", "2 3"},
    {"4\n3 3\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "3 2 0 4", "3"},
    {"4\n3 4\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "3 2 0 4", "4 3"},

    {"4\n4 1\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "7 6 4 0", "1 2 3 4"},
    {"4\n4 2\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "7 6 4 0", "2 3 4"},
    {"4\n4 3\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "7 6 4 0", "3 4"},
    {"4\n4 4\n4\n1 2 1\n2 3 2\n3 4 4\n4 1 8\n", "7 6 4 0", "4"},

    {"4\n1 1\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "0 1 2 1", "1"},
    {"4\n1 2\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "0 1 2 1", "2 1"},
    {"4\n1 3\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "0 1 2 1", "3 2 1", "3 4 1"},
    {"4\n1 4\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "0 1 2 1", "4 1"},

    {"4\n2 1\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "1 0 1 2", "1 2"},
    {"4\n2 2\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "1 0 1 2", "2"},
    {"4\n2 3\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "1 0 1 2", "3 2"},
    {"4\n2 4\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "1 0 1 2", "4 3 2", "4 1 2"},

    {"4\n3 1\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "2 1 0 1", "1 2 3", "1 4 3"},
    {"4\n3 2\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "2 1 0 1", "2 3"},
    {"4\n3 3\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "2 1 0 1", "3"},
    {"4\n3 4\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "2 1 0 1", "4 3"},

    {"4\n4 1\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "1 2 1 0", "1 4"},
    {"4\n4 2\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "1 2 1 0", "2 3 4", "2 1 4"},
    {"4\n4 3\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "1 2 1 0", "3 4"},
    {"4\n4 4\n4\n1 2 1\n2 3 1\n3 4 1\n4 1 1\n", "1 2 1 0", "4"},

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
    char pass[] = "PASSED", fail[] = "FAILED", *fact = pass;
    char buf1[128] = {0}, buf2[128] = {0};
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    if (fgets(buf1, sizeof(buf1), out) == NULL) {
        printf("no output -- ");
    }
    if (strchr(buf1, '\n')) {
        *strchr(buf1, '\n') = 0;
    }
    if (_strnicmp(testInOut[testN].out1, buf1, strlen(testInOut[testN].out1)) != 0) {
        fact = fail;
    }
    if (fact == pass && testInOut[testN].out2 != NULL) { // check path
        if (fgets(buf2, sizeof(buf2), out) == NULL) {
            printf("output too short -- ");
        }
        if (strchr(buf2, '\n')) {
            *strchr(buf2, '\n') = 0;
        }
        if (testInOut[testN].out20 == NULL) { // unique shortest path
            if (_strnicmp(testInOut[testN].out2, buf2, strlen(testInOut[testN].out2)) != 0) {
                fact = fail;
            }
        } else { // two shortest paths
            if (_strnicmp(testInOut[testN].out2, buf2, strlen(testInOut[testN].out2)) != 0
                && _strnicmp(testInOut[testN].out20, buf2, strlen(testInOut[testN].out20)) != 0) {
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
    fprintf(in, "5000\n1 5000\n4999\n");
    for (i = 0; i < 4999; i++) {
        fprintf(in, "%d %d 1\n", i+1, i+2);
    }
    fclose(in);
    labOutOfMemory = 5000*5000*4+MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int checkerBig(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i;
    char passed[] = "PASSED", failed[] = "FAILED", *message = passed;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 0; i < 5000; i++) {
        int d, status = fscanf(out, "%d", &d);
        if (status < 0) {
            printf("output too short -- ");
            message = failed;
            break;
        }
        if (status == 0) {
            printf("bad output format -- ");
            message = failed;
            break;
        }
        if (d != i) {
            printf("bad output -- ");
            message = failed;
            break;
        }
    }
    if (message == passed) {
        for (i = 0; i < 5000; i++) {
            int d, status = fscanf(out, "%d", &d);
            if (status < 0) {
                printf("output too short -- ");
                message = failed;
                break;
            }
            if (status == 0) {
                printf("bad output format -- ");
                message = failed;
                break;
            }
            if (d != 5000-i) {
                printf("bad output -- ");
                message = failed;
                break;
            }
        }
    }
    if (message == passed) {
        while (1) {
            char c;
            int status = fscanf(out, "%c", &c);
            if (status < 0) {
                break;
            }
            if (!strchr(" \t\r\n", c)) {
                printf("garbage at the end -- ");
                message = failed;
                break;
            }
        }
    }
    fclose(out);
    printf("%s\n", message);
    testN++;
    return message == failed;
}

static int feederBig1(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "5000\n1 2501\n5000\n");
    for (i = 0; i < 4999; i++) {
        fprintf(in, "%d %d 1\n", i+1, i+2);
    }
    fprintf(in, "5000 1 1\n");
    fclose(in);
    labOutOfMemory = 5000*5000*4+MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int checkerBig1(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i;
    char passed[] = "PASSED", failed[] = "FAILED", *message = passed;
    int p[2501], v1 = 1, v2 = 1; // two shortest paths exist
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 0; i < 5000; i++) {
        int d, status = fscanf(out, "%d", &d);
        if (status < 0) {
            printf("output too short -- ");
            message = failed;
            break;
        }
        if (status == 0) {
            printf("bad output format -- ");
            message = failed;
            break;
        }
        if ((i <= 2500 && d != i) || (i > 2500 && d != 5000-i)) {
            printf("bad output A -- ");
            message = failed;
            break;
        }
    }
    if (message == passed) {
        for (i = 0; i < 2501; i++) {
            int status = fscanf(out, "%d", &p[i]);
            if (status < 0) {
                printf("output too short -- ");
                message = failed;
                break;
            }
            if (status == 0) {
                printf("bad output format -- ");
                message = failed;
                break;
            }
        }
        for (i = 0; i < 2501; i++)
            if (p[i] != 2501-i) { // 2501 2500 2499 ... 1
                v1 = 0;
                break;
            }
        for (i = 0; i < 2500; i++)
            if (p[i] != 2501+i) { // 2501 2502 ... 4999 5000 1
                v2 = 0;
printf("want %d get %d\n",2501+i,p[i]);
                break;
            }
        if (p[2500] != 1) {
printf("want %d get %d\n",1,p[2500]);
            v2 = 0;
        }
        if (v1 == 0 && v2 == 0) {
            printf("bad output B -- ");
            message = failed;
        }
    }
    if (message == passed) {
        while (1) {
            char c;
            int status = fscanf(out, "%c", &c);
            if (status < 0) {
                break;
            }
            if (!strchr(" \t\r\n", c)) {
                printf("garbage at the end -- ");
                message = failed;
                break;
            }
        }
    }
    fclose(out);
    printf("%s\n", message);
    testN++;
    return message == failed;
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
    printf("Creating a large graph...");
    fflush(stdout);
    t = GetTickCount();

    fprintf(in, "5000\n1 2\n%d\n", 5000*4999/2-1);
    for (i = 0; i+1 < 5000; i++) {
        int j;
        for (j = i+1; j < 5000; j++) {
            if ((i == 0 || i == 1 || i == 1984) && (j == 0 || j == 1 || j == 1984))
                continue;
            if (fprintf(in, "%d %d 2\n", i+1, j+1) < 2) {
                printf("can't create in.txt. No space on disk?\n");
                fclose(in);
                return -1;
            }
        }
    }
    fprintf(in, "1 1985 1\n");
    fprintf(in, "2 1985 1\n");
    fflush(NULL);

    t = (tickDifference(t, GetTickCount())+999)/1000*1000;
    printf("done in T=%u seconds. Starting exe with timeout T+6... ", (unsigned)t/1000);
    labTimeout = (int)t+6000;
    fflush(stdout);
    fclose(in);
    labOutOfMemory = 5000*5000*4+MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int checkerBig2(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i;
    char passed[] = "PASSED", failed[] = "FAILED", *message = passed;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 1; i <= 5000; i++) {
        int d, status = fscanf(out, "%d", &d);
        if (status < 0) {
            printf("output too short -- ");
            message = failed;
            break;
        }
        if (status == 0) {
            printf("bad output format -- ");
            message = failed;
            break;
        }
        if (i != 1 && i != 2 && i != 1985 && d != 2) {
            printf("bad output -- ");
            message = failed;
            break;
        }
        if (i == 1 && d != 0) {
            printf("bad output -- ");
            message = failed;
            break;
        }
        if (i == 2 && d != 2) {
            printf("bad output -- ");
            message = failed;
            break;
        }
        if (i == 1985 && d != 1) {
            printf("bad output -- ");
            message = failed;
            break;
        }
    }
    if (message == passed) {
        int d1, d2, d3, status = fscanf(out, "%d %d %d", &d1, &d2, &d3);
        if (status < 0) {
            printf("output too short -- ");
            message = failed;
        } else if (status < 3) {
            printf("bad output format -- ");
            message = failed;
        } else if (d1 != 2 || d2 != 1985 || d3 != 1) {
            printf("bad output -- ");
            message = failed;
        }
    }
    if (message == passed) {
        while (1) {
            char c;
            int status = fscanf(out, "%c", &c);
            if (status < 0) {
                break;
            }
            if (!strchr(" \t\r\n", c)) {
                printf("garbage at the end -- ");
                message = failed;
                break;
            }
        }
    }
    fclose(out);
    printf("%s\n", message);
    testN++;
    return message == failed;
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
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},

    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},
    {feederN, checkerN},

    {feederBig, checkerBig},
    {feederBig1, checkerBig1},
    {feederBig2, checkerBig2},
};

const int labNTests = sizeof(labTests)/sizeof(labTests[0]);

const char labName[] = "Lab 9 Dijkstra Shortest Path";

int labTimeout = 3000;
size_t labOutOfMemory = MIN_PROCESS_RSS_BYTES;
