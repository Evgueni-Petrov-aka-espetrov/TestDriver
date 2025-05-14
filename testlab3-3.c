#include "testLab.h"
#include <stdio.h>
#include <string.h>

static const struct {const char *const in, *const out;} testInOut[] = {
    {"0\n", "bad input"},
    {"2000001\n", "bad input"},
    {"-1\n", "bad input"},
    {"1\n2\n", "2"},
    {"2\n-1 -1\n", "-1 && -1 => -1 -1 | -1 -1"},
    {"2\n4 2\n", "4 && 2 => 2 4 | 2 4"},
    {"2\n2 4\n", "2 && 4 => 2 4 | 2 4"},
    {"3\n4 -1 2\n", "4 && -1 => -1 4 | -1 4 && 2 => -1 2 4 | -1 2 4"},
    {"3\n0 0 -1\n", "0 && 0 => 0 0 | 0 0 && -1 => -1 0 0 | -1 0 0"},
    {"3\n-1 2 4\n", "-1 && 2 => -1 2 | -1 2 && 4 => -1 2 4 | -1 2 4"},
    {"3\n-1 4 2\n", "-1 && 4 => -1 4 | -1 4 && 2 => -1 2 4 | -1 2 4"},
    {"3\n2 -1 4\n", "2 && -1 => -1 2 | -1 2 && 4 => -1 2 4 | -1 2 4"},
    {"3\n2 4 -1\n", "2 && 4 => 2 4 | 2 4 && -1 => -1 2 4 | -1 2 4"},
    {"3\n4 2 -1\n", "4 && 2 => 2 4 | 2 4 && -1 => -1 2 4 | -1 2 4"},
    {"3\n-1 -1 2\n", "-1 && -1 => -1 -1 | -1 -1 && 2 => -1 -1 2 | -1 -1 2"},
    {"3\n-1 2 -1\n", "-1 && 2 => -1 2 | -1 2 && -1 => -1 -1 2 | -1 -1 2"},
    {"3\n2 -1 -1\n", "2 && -1 => -1 2 | -1 2 && -1 => -1 -1 2 | -1 -1 2"},
    {"4\n1 2 3 4\n", "1 && 2 => 1 2 | 3 && 4 => 3 4 | 1 2 && 3 4 => 1 2 3 4 | 1 2 3 4"},
    {"4\n1 2 4 3\n", "1 && 2 => 1 2 | 4 && 3 => 3 4 | 1 2 && 3 4 => 1 2 3 4 | 1 2 3 4"},
    {"6\n3 4 0 5 1 2\n", "3 && 4 => 3 4 | 0 && 5 => 0 5 | 1 && 2 => 1 2 | 3 4 && 0 5 => 0 3 4 5 | 0 3 4 5 && 1 2 => 0 1 2 3 4 5 | 0 1 2 3 4 5"},
    {"6\n0 3 4 2 5 1\n", "0 && 3 => 0 3 | 4 && 2 => 2 4 | 5 && 1 => 1 5 | 0 3 && 2 4 => 0 2 3 4 | 0 2 3 4 && 1 5 => 0 1 2 3 4 5 | 0 1 2 3 4 5"},
    {"6\n5 4 3 2 1 0\n", "5 && 4 => 4 5 | 3 && 2 => 2 3 | 1 && 0 => 0 1 | 4 5 && 2 3 => 2 3 4 5 | 2 3 4 5 && 0 1 => 0 1 2 3 4 5 | 0 1 2 3 4 5"},
    {"9\n13 3 8 1 15 2 3 7 4\n","13 && 3 => 3 13 | 8 && 1 => 1 8 | 15 && 2 => 2 15 | 3 && 7 => 3 7 | 3 13 && 1 8 => 1 3 8 13 | 2 15 && 3 7 => 2 3 7 15 | 1 3 8 13 && 2 3 7 15 => 1 2 3 3 7 8 13 15 | 1 2 3 3 7 8 13 15 && 4 => 1 2 3 3 4 7 8 13 15 | 1 2 3 3 4 7 8 13 15"}
};
static int testN = 0;


static int FeedFromArray(void)
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
static int CheckFromArray(void) 
{
    FILE* const out = fopen("out.txt", "rb");
    if (out == NULL) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }

    char buf[256] = {0};
    const char* status = ScanChars(out, sizeof(buf), buf);
    fclose(out);
    for (int i = 0; i < 256; i++) {
        printf("%c", buf[i]);
    }
    if (status == Pass && _strnicmp(testInOut[testN].out, buf, strlen(testInOut[testN].out)) != 0) {
        status = Fail;
    }
    if (status == Pass && HaveGarbageAtTheEnd(out)) {
        status = Fail;
    }
    printf("%s\n", status);
    ++testN;
    return status == Fail;
}

static int LabTimeout;
static size_t LabMemoryLimit;

static int feederBig(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    DWORD t;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large array... ");
    fflush(stdout);
    t = GetTickCount();
    fprintf(in, "%d\n", 2000000);
    for (i = 0; i < 2000000; i++) {
        if (fprintf(in, "%d ", i^0x1234567) == EOF) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fprintf(in, "\n");
    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2*T+3... ", (unsigned)t/1000);
    LabTimeout = (int)t*2+3000;
    fflush(stdout);
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES+4*2000000;
    return 0;
}

static int checkerBig(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i, passed = 1, last = 0;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 0; i < 2000000; i++) {
        int n;
        if (ScanInt(out, &n) != Pass) {
            passed = 0;
            break;
        } else if ((n^0x1234567) < 0 || 2000000 <= (n^0x1234567)) {
            passed = 0;
            printf("wrong output -- ");
            break;
        } else if (i == 0) {
            last = n;
            continue;
        } else if (n <= last ) {
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

static int feederBig2(void)
{
    FILE *const in = fopen("in.txt", "w+");
    int i;
    DWORD t;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large array... ");
    fflush(stdout);
    t = GetTickCount();
    fprintf(in, "%d\n", 2000000);
    for (i = 0; i < 200000; i++) {
        if (fprintf(in, "0 0 0 0 0 0 0 0 0 0 ") == EOF) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fprintf(in, "\n");
    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2*T+3... ", (unsigned)t/1000);
    LabTimeout = (int)t*2+3000;
    fflush(stdout);
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES+4*2000000;
    return 0;
}

static int checkerBig2(void)
{
    FILE *const out = fopen("out.txt", "r");
    int i, passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    for (i = 0; i < 2000000; i++) {
        int n;
        if (ScanInt(out, &n) != Pass) {
            passed = 0;
            break;
        } else if (n != 0) {
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
    {feederBig2, checkerBig2}
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return sizeof(LabTests)/sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
    return "Lab 3-3 Merge Sort";
}

static int LabTimeout = 3000;
int GetTestTimeout(void) {
    return LabTimeout;
}

static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;
size_t GetTestMemoryLimit(void) {
    return LabMemoryLimit;
}
