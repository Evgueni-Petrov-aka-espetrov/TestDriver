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
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    const char* fact = Pass;
    char buf1[128] = {0};
    fact = ScanChars(out, sizeof(buf1), buf1);
    if (fact == Pass && _strnicmp(testInOut[testN].out1, buf1, strlen(testInOut[testN].out1)) != 0) {
        fact = Fail;
    }
    if (fact == Pass && testInOut[testN].out2 != NULL) { // check path
        char buf2[128] = {0};
        fact = ScanChars(out, sizeof(buf2), buf2);
        if (fact == Pass && _strnicmp(testInOut[testN].out2, buf2, strlen(testInOut[testN].out2)) != 0) {
            if (testInOut[testN].out20 == NULL || _strnicmp(testInOut[testN].out20, buf2, strlen(testInOut[testN].out20)) != 0) {
                printf("wrong output -- ");
                fact = Fail;
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

static int LabTimeout;
static size_t LabMemoryLimit;

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
    LabMemoryLimit = 5000*5000*4+MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int checkerBig(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i;
    const char* message = Pass;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 0; i < 5000; i++) {
        int d;
        message = ScanInt(out, &d);
        if (message != Pass) {
            break;
        }
        if (d != i) {
            printf("wrong output -- ");
            message = Fail;
            break;
        }
    }
    if (message == Pass) {
        for (i = 0; i < 5000; i++) {
            int d;
            message = ScanInt(out, &d);
            if (message != Pass) {
                break;
            }
            if (d != 5000-i) {
                printf("wrong output -- ");
                message = Fail;
                break;
            }
        }
    }
    if (message == Pass && HaveGarbageAtTheEnd(out)) {
        message = Fail;
    }
    fclose(out);
    printf("%s\n", message);
    testN++;
    return message == Fail;
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
    LabMemoryLimit = 5000*5000*4+MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int checkerBig1(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i;
    const char* message = Pass;
    int p[2501], v1 = 1, v2 = 1; // two shortest paths exist
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 0; i < 5000; i++) {
        int d;
        message = ScanInt(out, &d);
        if (message != Pass) {
            break;
        }
        if ((i <= 2500 && d != i) || (i > 2500 && d != 5000-i)) {
            printf("wrong output -- ");
            message = Fail;
            break;
        }
    }
    if (message == Pass) {
        for (i = 0; i < 2501; i++) {
            message = ScanInt(out, &p[i]);
            if (message != Pass) {
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
                break;
            }
        if (p[2500] != 1) {
            v2 = 0;
        }
        if (v1 == 0 && v2 == 0) {
            printf("wrong output -- ");
            message = Fail;
        }
    }
    if (message == Pass && HaveGarbageAtTheEnd(out)) {
        message = Fail;
    }
    fclose(out);
    printf("%s\n", message);
    testN++;
    return message == Fail;
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
            if ((i == 0 || i == 1 || i == 1984) && (j == 0 || j == 1 || j == 1984)) {
                continue;
            }
            if (fprintf(in, "%d %d 2\n", i+1, j+1) < 2) {
                printf("can't create in.txt. No space on disk?\n");
                fclose(in);
                return -1;
            }
        }
    }
    fprintf(in, "1 1985 1\n");
    fprintf(in, "2 1985 1\n");
    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout T+6... ", (unsigned)t/1000);
    LabTimeout = (int)t+6000;
    fflush(stdout);
    LabMemoryLimit = 5000*5000*4+MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int checkerBig2(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i;
    const char* message = Pass;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 1; i <= 5000; i++) {
        int d;
        message = ScanInt(out, &d);
        if (message != Pass) {
            break;
        }
        if (i != 1 && i != 2 && i != 1985 && d != 2) {
            printf("wrong output -- ");
            message = Fail;
            break;
        }
        if (i == 1 && d != 0) {
            printf("wrong output -- ");
            message = Fail;
            break;
        }
        if (i == 2 && d != 2) {
            printf("wrong output -- ");
            message = Fail;
            break;
        }
        if (i == 1985 && d != 1) {
            printf("wrong output -- ");
            message = Fail;
            break;
        }
    }
    if (message == Pass) {
        int d1, d2, d3;
        if (ScanIntInt(out, &d1, &d2) != Pass || ScanInt(out, &d3) != Pass) {
            message = Fail;
        } else if (d1 != 2 || d2 != 1985 || d3 != 1) {
            printf("wrong output -- ");
            message = Fail;
        }
    }
    if (message == Pass && HaveGarbageAtTheEnd(out)) {
        message = Fail;
    }
    fclose(out);
    printf("%s\n", message);
    testN++;
    return message == Fail;
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
    return "Lab 9 Dijkstra Shortest Path";
}

static int LabTimeout = 3000;
int GetTestTimeout() {
    return LabTimeout;
}

static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;
size_t GetTestMemoryLimit() {
    return LabMemoryLimit;
}
