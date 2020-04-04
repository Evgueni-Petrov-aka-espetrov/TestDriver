#include "testLab.h"
#include <stdio.h>
#include <string.h>

static int testN = 0;
static const struct {const char *const in, *const out[16]; int n;} testInOut[] = {
    {"214\n2\n", {"241", "412"}, 2},
    {"111\n1\n", {"bad input"}, 1},
    {"321\n1\n", {""}, 0},
    {"0x\n1\n", {"bad input"}, 1},
    {"0\n0\n", {""}, 0},
    {"0\n1\n", {""}, 0},
    {"0\n1000\n", {""}, 0},
    {"0123456789\n0\n", {""}, 0},
    {"0123456789\n1\n", {"0123456798"}, 1},
    {"9876543210\n1000\n", {""}, 0},
    {"012\n1000\n", {"021", "102", "120", "201", "210"}, 5},
    {"012\n5\n", {"021", "102", "120", "201", "210"}, 5},
    {"024\n5\n", {"042", "204", "240", "402", "420"}, 5}
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
    int i, passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 0; i < testInOut[testN].n; i++) {
        char perm[32];
        const size_t refLen = strlen(testInOut[testN].out[i]);
        if (!fgets(perm, sizeof(perm), out)) {
            passed = 0;
            printf("output too short -- ");
            break;
        } else if (strncmp(testInOut[testN].out[i], perm, refLen)) {
            passed = 0;
            printf("wrong output -- ");
            break;
        } else if (perm[refLen] != 0 && perm[refLen] != '\n') {
            passed = 0;
            printf("wrong output -- ");
            break;
        }
    }
    if (passed) {
        passed = !HaveGarbageAtTheEnd(out);
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
    {FeedFromArray, CheckFromArray}
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return sizeof(LabTests)/sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
    return "Lab 2 Deijkstra permutations";
}

int GetTestTimeout() {
    return 3000;
}

size_t GetTestMemoryLimit() {
    return MIN_PROCESS_RSS_BYTES;
}
