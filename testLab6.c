#include "testLab.h"
#include <stdio.h>
#include <string.h>

static int ptr_size(void )
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

static int testN = 0;
static const struct {const char *const in; int n;} testInOut[] = {
    {"0\n\n", 0},
    {"1\n25\n", 1},
    {"2\n17 25\n", 2},
    {"3\n2 -1 4\n", 2},
    {"3\n2 4 -1\n", 2},
    {"3\n4 2 -1\n", 2},
    {"3\n4 -1 2\n", 2},
    {"3\n-1 4 2\n", 2},
    {"3\n-1 2 4\n", 2},
    {"3\n-1 -1 2\n", 2},
    {"3\n-1 2 -1\n", 2},
    {"3\n2 -1 -1\n", 2},
    {"3\n-1 -1 -1\n", 2},
    {"2\n-1 -1\n", 2},
    {"2\n4 2\n", 2},
    {"2\n2 4\n", 2},
    {"4\n1 2 3 4\n", 3},
    {"4\n1 2 4 3\n", 3},
    {"4\n1 3 2 4\n", 3},
    {"4\n1 3 4 2\n", 3},
    {"4\n1 4 2 3\n", 3},
    {"4\n1 4 3 2\n", 3},
    {"4\n2 1 3 4\n", 3},
    {"4\n2 1 4 3\n", 3},
    {"4\n2 3 1 4\n", 3},
    {"4\n2 3 4 1\n", 3},
    {"4\n2 4 1 3\n", 3},
    {"4\n2 4 3 1\n", 3},
    {"4\n3 1 2 4\n", 3},
    {"4\n3 1 4 2\n", 3},
    {"4\n3 2 1 4\n", 3},
    {"4\n3 2 4 1\n", 3},
    {"4\n3 4 1 2\n", 3},
    {"4\n3 4 2 1\n", 3},
    {"4\n4 1 2 3\n", 3},
    {"4\n4 1 3 2\n", 3},
    {"4\n4 2 1 3\n", 3},
    {"4\n4 2 3 1\n", 3},
    {"4\n4 3 1 2\n", 3},
    {"4\n4 3 2 1\n", 3},
};

static int feederN(void)
{
    FILE *const in = fopen("in.txt", "w+");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    if (fprintf(in, "%s", testInOut[testN].in) < 0) {
        printf("can't create in.txt. No space on disk?\n");
        fclose(in);
        return -1;
    }
    fclose(in);
    return 0;
}

static int checkerN(void)
{
    FILE *const out = fopen("out.txt", "r");
    int passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    {
        int n, nStatus = fscanf(out, "%d", &n);
        if (EOF == nStatus) {
            passed = 0;
            printf("output too short -- ");
        } else if (1 != nStatus) {
            passed = 0;
            printf("bad format -- ");
        } else if (testInOut[testN].n != n) {
            passed = 0;
            printf("wrong output -- ");
        }
    }
    while (1) {
        char ignored;
        if (fscanf(out, "%c", &ignored) == EOF)
            break;
        if (strchr("\n\t ", ignored))
            continue;
        passed = 0;
        printf("output is too long -- ");
        break;
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

static int feederBig(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "2000000\n");
    for (i = 0; i < 2000000; i++) {
        if (fprintf(in, "%d ", i) < 0) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fprintf(in, "\n");
    fclose(in);
    {
        labOutOfMemory = 2000000*up16(sizeof(int)+sizeof(int)+2*ptr_size())+1024*1024;
    }
    return 0;
}

static int checkerBig(void)
{
    FILE *const out = fopen("out.txt", "r");
    int passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    {
        int n, nStatus = fscanf(out, "%d", &n);
        if (EOF == nStatus) {
            passed = 0;
            printf("output too short -- ");
        } else if (1 != nStatus) {
            passed = 0;
            printf("bad format -- ");
        } else if (21 != n) {
            passed = 0;
            printf("wrong output -- ");
        }
    }
    while (1) {
        char ignored;
        if (fscanf(out, "%c", &ignored) == EOF)
            break;
        if (strchr("\n\t ", ignored))
            continue;
        passed = 0;
        printf("output is too long -- ");
        break;
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

static int feederBig1(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "2000000\n");
    for (i = 0; i < 2000000; i++) {
        if (fprintf(in, "%d ", i^0xcafecafe) < 0) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fprintf(in, "\n");
    fclose(in);
    labOutOfMemory = 2000000*up16(sizeof(int)+sizeof(int)+2*ptr_size())+1024*1024;
    return 0;
}

static int checkerBig1(void)
{
    FILE *const out = fopen("out.txt", "r");
    int passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    {
        int n, nStatus = fscanf(out, "%d", &n);
        if (EOF == nStatus) {
            passed = 0;
            printf("output too short -- ");
        } else if (1 != nStatus) {
            passed = 0;
            printf("bad format -- ");
        } else if (23 != n) {
            passed = 0;
            printf("wrong output -- ");
        }
    }
    while (1) {
        char ignored;
        if (fscanf(out, "%c", &ignored) == EOF)
            break;
        if (strchr("\n\t ", ignored))
            continue;
        passed = 0;
        printf("output is too long -- ");
        break;
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

static void genTree(FILE *in, int height, int min, int max)
{
    int mid = (min*21+max*34)/55;
    if (height == 0)
        return;
    if (height == 1) {
        if (fprintf(in, "%d ", min) < 0) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            exit(1);
        }
        return;
    }
    if (height == 2) {
        if (min == max) {
            printf("Internal error: generation failed, h = %d, min = %d, max = %d\n", height, min, max);
            fclose(in);
            exit(1);
        }
        if (fprintf(in, "%d %d ", max, min) < 0) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            exit(1);
        }
        return;
    }
    if (mid <= min || max <= mid) {
        printf("Internal error: generation failed, h = %d, min = %d, max = %d\n", height, min, max);
        fclose(in);
        exit(1);
    }
    genTree(in, height-1, min, mid-1);
    genTree(in, height-2, mid+1, max);
    if (fprintf(in, "%d ", mid) < 0) {
        printf("can't create in.txt. No space on disk?\n");
        fclose(in);
        exit(1);
    }
}

static int feederBig2(void)
{
    FILE *const in = fopen("in.txt", "w+");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "1346269\n");
    genTree(in, 30, 0, 4000000);
    fprintf(in, "\n");
    fclose(in);
    labOutOfMemory = 1346269*up16(sizeof(int)+sizeof(int)+2*ptr_size())+1024*1024;
    return 0;
}

static int checkerBig2(void)
{
    FILE *const out = fopen("out.txt", "r");
    int passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    {
        int n, nStatus = fscanf(out, "%d", &n);
        if (EOF == nStatus) {
            passed = 0;
            printf("output too short -- ");
        } else if (1 != nStatus) {
            passed = 0;
            printf("bad format -- ");
        } else if (29 != n) {
            passed = 0;
            printf("wrong output -- ");
        }
    }
    while (1) {
        char ignored;
        if (fscanf(out, "%c", &ignored) == EOF)
            break;
        if (strchr("\n\t ", ignored))
            continue;
        passed = 0;
        printf("output is too long -- ");
        break;
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
    {feederBig, checkerBig},
    {feederBig1, checkerBig1},
    {feederBig2, checkerBig2},
};

const int labNTests = sizeof(labTests)/sizeof(labTests[0]);

const char labName[] = "Lab 6 AVL trees";

int labTimeout = 6000;
size_t labOutOfMemory = 1024*1024;
