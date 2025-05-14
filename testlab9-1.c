#include "testLab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static int testN = 0;
static size_t LabMemoryLimit;

static const struct {
    const char* in;
    const char* out;
} tests[] = {
        {"3 3\n1 1 1\n1 1 1\n1 1 1\n0 0\n2 2\n", "4"},
        {"2 2\n1 1\n1 1\n0 0\n0 1\n", "1"},
        {"1 1\n1\n0 0\n0 0\n", "0"},
        {"2 2\n1 -1\n-1 1\n0 0\n1 1\n", "No path found"},

        {"", "Invalid data format"},
        {"-5 3\n", "Invalid grid size"},
        {"2 2\n1 1\n1 1\n5 5\n0 0\n", "Invalid coordinates"},
        {"3 3\n1 1 X\n1 1 1\n1 1 1\n0 0\n2 2\n", "Invalid data format"},
        {"1001 1001\n", "Invalid grid size"},

        {"5 5\n1 -1 -1 -1 1\n1 -1 1 -1 1\n1 -1 -1 -1 1\n1 1 1 1 1\n1 1 1 1 1\n0 0\n4 4\n", "8"},
        {"3 3\n1 -1 1\n1 -1 1\n1 -1 1\n0 0\n2 2\n", "No path found"},
        {"4 4\n1 -1 1 1\n1 1 -1 1\n1 -1 1 1\n1 1 1 1\n0 0\n3 3\n", "6"},

        {"6 6\n1 1 1 1 1 1\n1 -1 -1 -1 -1 1\n1 -1 1 1 -1 1\n1 -1 1 1 -1 1\n1 -1 -1 -1 -1 1\n1 1 1 1 1 1\n0 0\n5 5\n", "10"},
        {"5 5\n1 -1 1 1 1\n1 1 1 -1 1\n1 -1 -1 -1 1\n1 -1 1 1 1\n1 1 1 -1 1\n0 0\n4 4\n", "10"},
        {"7 7\n1 1 1 -1 1 1 1\n1 -1 1 -1 1 -1 1\n1 -1 1 -1 1 -1 1\n1 -1 1 -1 1 -1 1\n1 -1 1 -1 1 -1 1\n1 -1 -1 -1 -1 -1 1\n1 1 1 1 1 1 1\n0 0\n6 6\n", "12"},
        {"5 5\n1 1 1 1 1\n1 1 1 1 1\n1 1 1 1 1\n1 1 1 1 1\n1 1 1 1 1\n2 2\n2 2\n", "0"},

        {"10 10\n1 1 1 -1 1 1 1 1 1 1\n1 -1 1 -1 1 -1 -1 -1 1 1\n1 -1 1 1 1 1 1 -1 1 1\n1 -1 -1 -1 -1 -1 1 -1 1 1\n1 1 1 1 1 1 1 -1 1 1\n1 -1 -1 -1 -1 -1 -1 -1 1 1\n1 1 1 1 1 1 1 1 1 1\n1 1 1 -1 -1 -1 -1 -1 -1 1\n1 -1 1 1 1 1 1 1 1 1\n1 1 1 -1 1 1 1 1 1 1\n0 0\n9 9\n", "18"},
        {"15 15\n1 -1 1 1 1 1 1 1 1 1 1 1 1 1 1\n1 -1 1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 -1 1\n1 -1 1 -1 1 1 1 1 1 1 1 1 1 -1 1\n1 -1 1 -1 1 -1 -1 -1 -1 -1 -1 -1 1 -1 1\n1 -1 1 -1 1 -1 1 1 1 1 1 -1 1 -1 1\n1 -1 1 -1 1 -1 1 -1 -1 -1 1 -1 1 -1 1\n1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1\n1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1\n1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1\n1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1\n1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1\n1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1\n1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1\n1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1 -1 1\n1 1 1 1 1 1 1 1 1 1 1 1 1 1 1\n0 0\n14 14\n", "28"}
};

static int FeedFromArray(void) {
    FILE* const in = fopen("in.txt", "w");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "%s", tests[testN].in);
    fclose(in);
    return 0;
}

static int CheckFromArray(void) {
    FILE* const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    char buf[256] = {0};
    fgets(buf, sizeof(buf), out);
    char* res = strtok(buf, "\n");
    int ok = res && !strcmp(res, tests[testN].out);
    int c;
    while ((c = fgetc(out)) != EOF) {
        if (!isspace(c)) {
            ok = 0;
            break;
        }
    }
    fclose(out);
    printf("%s\n", ok ? "PASS" : "FAIL");
    testN++;
    return ok ? 0 : -1;
}

static int feederBig1(void) {
    FILE* const in = fopen("in.txt", "w");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "1000 1000\n");

    for (int i = 0; i < 1000; ++i) {
        for (int j = 0; j < 1000; ++j) {
            if (j == 999) {
                fprintf(in, "1\n");
            } else {
                fprintf(in, "1 ");
            }
        }
    }
    fprintf(in, "0 0\n999 999\n");
    LabMemoryLimit = 1000 * 1000 * 8 + 2 * 1000 * 1000 * 4 + 10 * 1024 * 1024;
    fclose(in);
    return 0;
}


static int checkerBig1(void) {
    FILE* const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    int d;
    fscanf(out, "%d", &d);
    fclose(out);
    int ok = d == 1998;
    printf("%s\n", ok ? "PASS" : "FAIL");
    testN++;
    return ok ? 0 : -1;
}

static int feederBig2(void) {
    FILE* const in = fopen("in.txt", "w");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "500 500\n");
    for (int i = 0; i < 500; ++i) {
        for (int j = 0; j < 500; ++j)
            fprintf(in, (j == 499) ? "-1" : "1 ");
        fprintf(in, "\n");
    }
    fprintf(in, "0 0\n499 499\n");
    LabMemoryLimit = 500*500*8 + 2*500*500*4 + 10*1024*1024;
    fclose(in);
    return 0;
}

static int checkerBig2(void) {
    FILE* const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    char buf[256];
    fgets(buf, sizeof(buf), out);
    int ok = strcmp(strtok(buf, "\n"), "No path found") == 0;
    printf("%s\n", ok ? "PASS" : "FAIL");
    testN++;
    return ok ? 0 : -1;
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

        {feederBig1, checkerBig1},
        {feederBig2, checkerBig2}
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return (int)sizeof(LabTests)/sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
    return "Lab 9-1 Lee Algorithm";
}

int GetTestTimeout(void) {
    return 3000;
}

size_t GetTestMemoryLimit(void) {
    return LabMemoryLimit;
}
