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
    const char* out2;
} tests[] = {
        {"3 3\n1 1 1\n1 1 1\n1 1 1\n0 0\n2 2\n","4", "(0,0), (0,1), (0,2), (1,2), (2,2)"},
        {"3 3\n1 1 1\n1 -2 1\n1 1 1\n0 0\n2 2\n","4", "(0,0), (1,0), (2,0), (2,1), (2,2)"},
        {"1 1\n1\n0 0\n0 0\n", "0", "(0,0)"},
        {"2 2\n1 -1\n-1 1\n0 0\n1 1\n", "No path found", NULL},
        {"", "Invalid input", NULL},
        {"-5 3", "Invalid grid dimensions", NULL},
        {"3 3\n1 1 1\n1 -3 1\n1 1 1\n0 0\n2 2\n","Invalid cell value", NULL},
        {"2 2\n1 1\n1 1\n5 5\n0 0\n","Invalid start or end coordinate", NULL},
        {"3 3\n1 1\n1 1 1\n1 1 1\n0 0\n2 2\n","Input short", NULL},
        {"3 3\n1 1 X\n1 1 1\n1 1 1\n0 0\n2 2\n","Invalid input", NULL},
        {"1001 1001\n", "Invalid grid size", NULL},
        {"5 5\n1 -2 -2 -2 1\n1 -2 1 -2 1\n1 -2 -2 -2 1\n1 1 1 1 1\n0 0\n4 4\n","8", "(0,0), (0,1), (0,2), (0,3), (0,4), (1,4), (2,4), (3,4), (4,4)"},
        {"3 3\n1 -2 1\n1 -2 1\n1 -2 1\n0 0\n2 2\n","No path found", NULL},
        {"5 5\n1 1 1 1 1\n1 -2 -2 -2 1\n1 -2 1 -2 1\n1 -2 -2 -2 1\n1 1 1 1 1\n2 2\n4 4\n","4", "(2,2), (3,2), (4,2), (4,3), (4,4)"},
        {"4 4\n1 -2 1 1\n1 1 -2 1\n1 -2 1 1\n1 1 1 1\n0 0\n3 3\n","6", "(0,0), (1,0), (2,0), (3,0), (3,1), (3,2), (3,3)"},
        {"3 3\n-2 -2 -2\n-2 1 -2\n-2 -2 -2\n1 1\n1 1\n","Start position is trap", NULL},
        {"5 5\n1 1 1 1 1\n1 1 1 1 1\n1 1 1 1 1\n1 1 1 1 1\n1 1 1 1 1\n0 0\n4 4\n","8", "(0,0), (1,0), (2,0), (3,0), (4,0), (4,1), (4,2), (4,3), (4,4)"},
        {"4 4\n1 1 1 1\n1 -2 1 1\n1 1 -2 1\n1 1 1 1\n0 0\n3 3\n","6", "(0,0), (0,1), (0,2), (0,3), (1,3), (2,3), (3,3)"},
        {"3 3\n1 1 1\n1 1 1\n1 1 1\n2 2\n0 0\n","4", "(2,2), (2,1), (2,0), (1,0), (0,0)"}
};

static int FeedFromArray(void) {
    FILE* const in = fopen("in.txt", "w");
    if (!in) {
        return -1;
    }
    fprintf(in, "%s", tests[testN].in);
    fclose(in);
    return 0;
}

static int CheckFromArray(void) {
    FILE* const out = fopen("out.txt", "r");
    if (!out) {
        testN++;
        return -1;
    }

    const char* result = Pass;
    char buf[4096] = {0};
    int c;

    if (!fgets(buf, sizeof(buf), out) || strcmp(strtok(buf, "\n"), tests[testN].out1) != 0) {
        printf("wrong output -- ");
        result = Fail;
    }

    if (result == Pass && tests[testN].out2 != NULL) {
        if (!fgets(buf, sizeof(buf), out) || strcmp(strtok(buf, "\n"), tests[testN].out2) != 0) {
            printf("wrong output -- ");
            result = Fail;
        }
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
        testN++;
        return -1;
    }

    const char* result = Pass;
    int distance;
    char path[4096];
    int c;

    if (fscanf(out, "%d", &distance) != 1 || distance != 1998) {
        printf("wrong output -- ");
        result = Fail;
    }

    if (result == Pass && (!fgets(path, sizeof(path), out) || !strstr(path, "(999,999)"))) {
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
        return -1;
    }
    const int size = 500;
    fprintf(in, "%d %d\n", size, size);
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            fprintf(in, "%s ", (j == size-1) ? "-2" : "1");
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
        testN++;
        return -1;
    }

    const char* result = Pass;
    char buf[4096];
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
    return 30000;
}

size_t GetTestMemoryLimit(void) {
    return LabMemoryLimit;
}
          
