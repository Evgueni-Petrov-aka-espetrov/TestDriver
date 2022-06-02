#include "testLab.h"
#include <stdio.h>
#include <string.h>

static int testN = 0;
static const struct {const char *const in, *const out;} testInOut[] = {
    {"0\n", "bad input"},
    {"11\n", "bad input"},
    {"5\n1 2 3 4\n", "bad input"},
    {"-1\n", "bad input"},
    {"2\n1 1\n", "2"},
    {"5\n1 2 3 4 5\n", "15"},
    {"3\n1 2 3\n", "6"},
    {"4\n-1 1 -1 1\n", "0"},
    {"5\n2 2 2 2 2\n", "10"},
    {"6\n13 12 5 20 10 9\n", "69"},
    {"7\n87 37 657 8 1 87 829\n", "1706"},
    {"9\n98 -192837 89 298 -1 90 382 7 8\n", "-191866"},
    {"9\n-23 -23 -42 -42 987 -90 -75 90 -90\n", "692"},
    {"1\n2147483647\n", "2147483647"},
    {"1\n2147483648\n", "bad input"},
    {"1\n-2147483648\n", "-2147483648"},
    {"1\n-2147483649\n", "bad input"},
    {"2\n2147483647 1\n", "overflow"},
    {"3\n1000000 10000000 10000000000\n", "bad input"},
    {"5\n2000000000 1000000000 1000000000 1 1\n", "overflow"},
    {"3\n-1000000 -10000000 -10000000000\n", "bad input"},
    {"5\n-2000000000 -1000000000 -1000000000 -1 -1\n", "overflow"},
    {"3\n9273490 394839 793432\n", "10461761"},
    {"3\n2034847384 1037584758 1847385768\n", "overflow"},
    {"3\n-1034847384 -1037584758 -1847385768\n", "overflow"},
    {"3\n918237 9283745 293847\n", "10495829"},
    {"3\n1 2 1\n", "4"},
    {"10\n19837 98547 19283 748 93847 293847 983457 98734 837 837\n", "1609974"},
    {"10\n192837 93857 29837 93587 8237 758 2837 457 189273 19283\n", "630963"},
    {"10\n-1892734 -384576 -81273 -4958 -289374 -293847 -895476 -37 -48975 -9178\n", "-3900428"}
};

static int FeedFromArray(void) {
    FILE* const in = fopen("in.txt", "w+");
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
    char buf[128] = {0};
    const char* status = ScanChars(out, sizeof(buf), buf);
    fclose(out);
    if (status == Pass && strchr(buf, '.')) {
        strncpy(buf + strlen(buf), "0000""0000""0000""0000""0000""0000""0000""0000", sizeof(buf) - strlen(buf) - 1);
    }
    if (status == Pass && _strnicmp(testInOut[testN].out, buf, strlen(testInOut[testN].out)) != 0) {
        status = Fail;
    }
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
    return "Lab-begin";
}

int GetTestTimeout() {
    return 3000;
}

size_t GetTestMemoryLimit() {
    return MIN_PROCESS_RSS_BYTES;
}
