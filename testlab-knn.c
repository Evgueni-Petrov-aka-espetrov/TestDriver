#include "testLab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

static int testN = 0;
static const struct {const char *const in; const char* const out;} testInOut[] = {              
    {"5 5\n3\n1 1\n6 6\n10 10", "6 6"},
    {"0 0\n1\n3 4", "3 4"},
    {"5 5", "bad input"}, 
    {"5 5\nabc\n1 1", "bad input"},
    {"5.5\n1\n1 1", "bad input"}, 
    {"0 0\n2\n1 1\n10 10", "1 1"},
    {"9 9\n2\n1 1\n10 10", "10 10"},
    {"-5 -5\n3\n-10 -10\n0 0\n-3 -4", "-3 -4"},  
    {"7 8\n3\n1 2\n7 8\n10 10", "7 8"},
    {"0 0\n3\n-5 0\n3 0\n10 0", "3 0"},
    {"0 0\n3\n0 -8\n0 4\n0 12", "0 4"},
    {"0 0\n4\n1 0\n0 2\n-3 0\n5 5", "1 0"},
    {"100 100\n5\n0 0\n10 10\n20 20\n90 90\n95 95", "95 95"},
    {"0 0\n5\n50 50\n51 51\n1 1\n2 2\n-5 -5", "1 1"},
    {"3 7\n4\n0 0\n10 10\n3 6\n4 8", "3 6"},
    {"0 0\n3\n3 4\n6 0\n0 6", "3 4"},
    {"0 0\n4\n10 20\n15 5\n8 8\n12 12", "8 8"},
    {"50 50\n4\n0 100\n100 0\n49 51\n52 48", "49 51"},
    {"-1 1\n4\n-10 -10\n10 -10\n0 0\n-1 2", "-1 2"},
    {"0 0\n1\n0 3", "0 3"},
    {"5 5\n3\n5 5\n5 5\n5 5", "5 5"},
    {"1000 0\n4\n0 0\n1 0\n2 0\n3 0", "3 0"},
    {"-10 10\n4\n-5 15\n-12 8\n-9 11\n-10 9", "-10 9"},
    {"25 25\n4\n0 0\n50 50\n21 21\n30 30", "21 21"},
    {"15 20\n3\n10 10\n20 20\n15 21", "15 21"}
};

static int LabTimeout = 3000;
static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;

static int FeedFromArray(void) {
    FILE *const in = fopen("in.txt", "w+");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    fprintf(in, "%s", testInOut[testN].in);
    fclose(in);
    return 0;
}

static int CheckFromArray(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }

    char buffer[128];
    const char* status = ScanChars(out, sizeof(buffer), buffer);

    if (status == Pass) {
        if (strcmp(buffer, testInOut[testN].out) != 0) {
            status = Fail;
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

static int feederBig1(void) {
    FILE *const in = fopen("in.txt", "w+");
    DWORD t;
    const int N = 200000;  
    
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    printf("Creating large dataset with one close point... ");
    fflush(stdout);
    t = GetTickCount();
    
    fprintf(in, "0 0\n");
    
    fprintf(in, "%d\n", N);
    
    fprintf(in, "1 1\n");

    for (int i = 1; i < N; i++) {
        if (fprintf(in, "%d %d\n", 100000 + i, 100000 + i) == EOF) {
            printf("can't write to in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fclose(in);
    
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2*T+3... ", (unsigned)t / 1000);
    LabTimeout = (int)t * 2 + 3000;
    fflush(stdout);
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 20 * N;
    return 0;
}

static int checkerBig1(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        ++testN;
        return -1;
    }
    
    int x, y;
    const char* status = ScanIntInt(out, &x, &y);
    
    if (status == Pass) {
        if (x != 1 || y != 1) {
            printf("wrong output -- expected (1, 1), got (%d, %d) -- ", x, y);
            status = Fail;
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

static int feederBig2(void) {
    FILE *const in = fopen("in.txt", "w+");
    DWORD t;
    
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    printf("Creating large grid... ");
    fflush(stdout);
    t = GetTickCount();
    
    fprintf(in, "%d %d\n", 100000, 100000);
    
    int total = 445 * 445;
    fprintf(in, "%d\n", total);
    
    for (int i = 0; i < 445; ++i) {
        for (int j = 0; j < 445; ++j) {
            if (fprintf(in, "%d %d\n", i*1000, j*1000) == EOF) {
                printf("can't write to in.txt. No space on disk?\n");
                fclose(in);
                return -1;
            }
        }
    }
    fclose(in);
    
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2*T+3... ", (unsigned)t / 1000);
    LabTimeout = (int)t * 2 + 3000;
    fflush(stdout);
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 20 * total;
    return 0;
}

static int checkerBig2(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        ++testN;
        return -1;
    }
    
    int x, y;
    const char* status = ScanIntInt(out, &x, &y);
    
    if (status == Pass) {
        if (x != 100000 || y != 100000) {
            printf("wrong output -- expected (100000, 100000), got (%d, %d) -- ", x, y);
            status = Fail;
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

static int feederBig3(void) {
    FILE *const in = fopen("in.txt", "w+");
    DWORD t;
    
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large diagonal dataset... ");
    fflush(stdout);
    t = GetTickCount();
    
    fprintf(in, "50001 50000\n");
    
    fprintf(in, "%d\n", 150000);
    
    for (int i = 0; i < 150000; ++i) {
        if (fprintf(in, "%d %d\n", i, i) == EOF) {
            printf("can't write to in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fclose(in);
    
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2*T+3... ", (unsigned)t / 1000);
    LabTimeout = (int)t * 2 + 3000;
    fflush(stdout);
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 20 * 150000;
    return 0;
}

static int checkerBig3(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    
    int x, y;
    const char* status = ScanIntInt(out, &x, &y);
    
    if (status == Pass) {
        long long dx = 50001LL - x;
        long long dy = 50000LL - y;
        long long dist_sq = dx * dx + dy * dy;
        
        if (dist_sq != 1) {
            printf("wrong output -- distance^2=%lld (expected 1) -- ", dist_sq);
            status = Fail;
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
    {feederBig1, checkerBig1},  
    {feederBig2, checkerBig2},          
    {feederBig3, checkerBig3},  
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return sizeof(LabTests) / sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
    return "Lab 1-Nearest-Neighbour";
}

int GetTestTimeout(void) {
    return LabTimeout;
}

size_t GetTestMemoryLimit(void) {
    return LabMemoryLimit;
}