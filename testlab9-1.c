#include "testLab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

static int testN = 0;
static size_t LabMemoryLimit;

static const struct {
    const char* in;
    const char* out1;
} tests[] = {
        {"3 3\n1 1 1\n1 1 1\n1 1 1\n0 0\n2 2\n", "4"},
        {"1 1\n1\n0 0\n0 0\n", "0"},
        {"2 2\n1 -1\n-1 1\n0 0\n1 1\n", "No path found"},
        {"", "Invalid input"},
        {"-5 3", "Invalid grid dimensions"},
        {"3 3\n1 1 1\n1 2 1\n1 1 1\n0 0\n2 2\n", "Invalid cell value"},
        {"2 2\n1 1\n1 1\n5 5\n0 0\n", "Invalid start or end coordinate"},
        {"3 3\n1 1\n1 1 1\n1 1 1\n0 0\n2 2\n", "Input short"},
        {"3 3\n1 1 X\n1 1 1\n1 1 1\n0 0\n2 2\n", "Invalid input"},
        {"1001 1001\n", "Invalid grid size"},
        {"5 5\n1 -1 -1 -1 1\n1 -1 1 -1 1\n1 -1 -1 -1 1\n1 1 1 1 1\n0 0\n4 4\n", "8"},
        {"3 3\n1 -1 1\n1 -1 1\n1 -1 1\n0 0\n2 2\n", "No path found"},
        {"5 5\n1 1 1 1 1\n1 -1 -1 -1 1\n1 -1 1 -1 1\n1 -1 -1 -1 1\n1 1 1 1 1\n2 2\n4 4\n", "No path found"},
        {"4 4\n1 -1 1 1\n1 1 -1 1\n1 -1 1 1\n1 1 1 1\n0 0\n3 3\n", "6"},
        {"3 3\n-1 -1 -1\n-1 1 -1\n-1 -1 -1\n1 1\n1 1\n", "Start position is wall"},
        {"5 5\n1 1 1 1 1\n1 1 1 1 1\n1 1 1 1 1\n1 1 1 1 1\n1 1 1 1 1\n0 0\n4 4\n", "8"},
        {"4 4\n1 1 1 1\n1 -1 1 1\n1 1 -1 1\n1 1 1 1\n0 0\n3 3\n", "6"},
        {"3 3\n1 1 1\n1 1 1\n1 1 1\n2 2\n0 0\n", "4"},
        {"5 5\n1 1 1 1 1\n1 -1 -1 -1 1\n1 1 1 -1 1\n1 -1 -1 -1 1\n1 1 1 1 1\n0 0\n4 4\n", "12"},
        {"4 4\n1 -1 1 1\n1 1 -1 1\n1 -1 -1 1\n1 1 1 1\n0 0\n3 3\n", "8"},
        {"5 5\n1 1 1 -1 1\n1 -1 1 -1 1\n1 -1 1 -1 1\n1 -1 1 1 1\n1 -1 -1 -1 1\n0 0\n4 4\n", "12"},
        {"6 6\n1 -1 1 1 1 1\n1 -1 1 -1 -1 1\n1 -1 1 -1 1 1\n1 -1 -1 -1 1 1\n1 1 1 1 1 1\n0 0\n5 5\n", "14"},

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

    const char* result = Pass;
    char buf[256] = {0};
    int c;

    if (!fgets(buf, sizeof(buf), out) || strcmp(strtok(buf, "\n"), tests[testN].out1) != 0) {
        printf("wrong output -- ");
        result = Fail;
    }

    while ((c = fgetc(out)) != EOF) {
        if (!isspace(c)) {
            printf("wrong output -- ");
            result = Fail;
            break;
        }
    }

    fclose(out);
    printf("%s\n", result);
    testN++;
    return result == Fail ? -1 : 0;
}

static int feederBig1(void) {
    FILE* const in = fopen("in.txt", "w");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    const int size = 1000;
    fprintf(in, "%d %d\n", size, size);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            fprintf(in, "1%c", " \n"[j == size-1]);
        }
    }
    fprintf(in, "0 0\n999 999\n");
    LabMemoryLimit = (size_t)(size * size * 4) + (10 * 1024 * 1024);
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

    const char* result = Pass;
    int distance;
    int c;

    if (fscanf(out, "%d", &distance) != 1 || distance != 1998) {
        printf("wrong output -- ");
        result = Fail;
    }

    while ((c = fgetc(out)) != EOF) {
        if (!isspace(c)) {
            printf("wrong output -- ");
            result = Fail;
            break;
        }
    }

    fclose(out);
    printf("%s\n", result);
    testN++;
    return result == Fail ? -1 : 0;
}

static int feederBig2(void) {
    FILE* const in = fopen("in.txt", "w");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    const int size = 500;
    fprintf(in, "%d %d\n", size, size);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            fprintf(in, "%s ", (j == size-1) ? "-1" : "1");
        }
        fprintf(in, "\n");
    }
    fprintf(in, "0 0\n499 499\n");
    LabMemoryLimit = (size_t)(size * size * 4) + (10 * 1024 * 1024);
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

    const char* result = Pass;
    char buf[256];
    int c;

    if (!fgets(buf, sizeof(buf), out) || strcmp(strtok(buf, "\n"), "No path found") != 0) {
        printf("wrong output -- ");
        result = Fail;
    }

    while ((c = fgetc(out)) != EOF) {
        if (!isspace(c)) {
            printf("wrong output -- ");
            result = Fail;
            break;
        }
    }

    fclose(out);
    printf("%s\n", result);
    testN++;
    return result == Fail ? -1 : 0;
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
