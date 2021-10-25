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
    {"024\n5\n", {"042", "204", "240", "402", "420"}, 5},
    {"101\n1\n", {"bad input"}, 1},
    {"987654321098765432109876543210\n1\n", {"bad input"}, 1},
    {"012 3\n1\n", {"bad input"}, 1},
    {"89\n2000000000\n", {"98"}, 1},
    {"625431\n1\n", {"631245"}, 1}
};

static int FeedFromArray(void) {
    FILE *const in = fopen("in.txt", "w+");
    if (in == NULL) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "%s", testInOut[testN].in);
    fclose(in);
    return 0;
}

static int CheckFromArray(void) {
    FILE* const out = fopen("out.txt", "r");
    if (out == NULL) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    const char* status = Pass;
    for (int i = 0; i < testInOut[testN].n; ++i) {
        char perm[32];
        status = ScanChars(out, sizeof(perm), perm);
        const size_t refLen = strlen(testInOut[testN].out[i]);
        if (status == Pass && strncmp(testInOut[testN].out[i], perm, refLen) != 0) {
            status = Fail;
            printf("wrong output -- ");
            break;
        }
    }
    if (status == Pass && HaveGarbageAtTheEnd(out)) {
        status = Fail;
    }
    fclose(out);
    printf("%s\n", status);
    ++testN;
    return status == Fail;
}

TLabTest GetLabTest(int testIdx) {
    (void)testIdx;
    TLabTest labTest = {FeedFromArray, CheckFromArray};
    return labTest;
}

int GetTestCount(void) {
    return sizeof(testInOut) / sizeof(testInOut[0]);
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
