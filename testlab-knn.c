#include "testLab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

static int testN = 0;

static const struct {const char *const in; const char* const out;} testInOut[] = {              
    {"5 5 2\n3\n1 1\n6 6\n10 10", "6 6\n1 1"},
    {"0 0 1\n1\n3 4", "3 4"},
    {"5 5", "bad input"}, 
    {"5 5 1\nabc\n1 1", "bad input"},
    {"5.5 5 1\n1\n1 1", "bad input"}, 
    {"0 0 2\n2\n1 1\n10 10", "1 1\n10 10"},
    {"9 9 2\n2\n1 1\n10 10", "10 10\n1 1"},
    {"-5 -5 2\n3\n-10 -10\n0 0\n-3 -4", "-3 -4\n-10 -10"},  
    {"7 8 2\n3\n1 2\n7 8\n10 10", "7 8\n10 10"}, 
    {"0 0 3\n3\n-5 0\n3 0\n10 0", "3 0\n-5 0\n10 0"},
    {"0 0 2\n3\n0 -8\n0 4\n0 12", "0 4\n0 -8"},
    {"0 0 3\n4\n1 0\n0 2\n-3 0\n5 5", "1 0\n0 2\n-3 0"},
    {"100 100 4\n5\n0 0\n10 10\n20 20\n90 90\n95 95", "95 95\n90 90\n20 20\n10 10"},
    {"0 0 3\n5\n50 50\n51 51\n1 1\n2 2\n-5 -5", "1 1\n2 2\n-5 -5"},
    {"3 7 2\n4\n0 0\n10 10\n3 6\n4 8", "3 6\n4 8"},
    {"0 0 3\n3\n3 4\n6 0\n0 6", "3 4\n6 0\n0 6"}, 
    {"0 0 4\n4\n10 20\n15 5\n8 8\n12 12", "8 8\n15 5\n12 12\n10 20"},
    {"50 50 3\n4\n0 100\n101 1\n49 51\n52 48", "49 51\n52 48\n0 100"}, 
    {"-1 1 2\n4\n-10 -10\n10 -10\n0 0\n-1 2", "-1 2\n0 0"},
    {"0 0 1\n1\n0 3", "0 3"},
    {"5 5 3\n3\n5 5\n5 5\n5 5", "5 5\n5 5\n5 5"},
    {"1000 0 4\n4\n0 0\n1 0\n2 0\n3 0", "3 0\n2 0\n1 0\n0 0"},
    {"-10 10 2\n4\n-5 15\n-12 8\n-9 11\n-10 9", "-10 9\n-9 11"},
    {"25 25 3\n4\n0 0\n51 51\n21 21\n30 30", "21 21\n30 30\n0 0"}, 
    {"15 20 2\n3\n10 10\n20 20\n15 21", "15 21\n20 20"}
};

static int LabTimeout = 3000; 
static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;

static int FeedFromArray(void) {
    FILE* const in = fopen("in.txt", "w+");
    if (!in) { 
        printf("can't create in.txt. No space on disk?\n"); 
        return -1; 
    }

    fprintf(in, "%s", testInOut[testN].in);
    fclose(in);
    return 0;
}

static int CheckFromArray(void) {
    FILE* const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n"); ++testN; 
        return -1; 
    }

    char buffer[1024];
    size_t idx = 0;
    int c;
    while (idx < sizeof(buffer) - 1 && (c = fgetc(out)) != EOF) {
        buffer[idx++] = (char)c;
    }
    buffer[idx] = '\0';

    while (idx > 0 && (buffer[idx-1] == '\n' || buffer[idx-1] == '\r')) {
        buffer[--idx] = '\0';
    }

    const char* status = Pass;
    if (strcmp(buffer, testInOut[testN].out) != 0) {
        status = Fail;
    }
    if (status == Pass && HaveGarbageAtTheEnd(out)) {
        status = Fail;
    }

    fclose(out);
    printf("%s\n", status);
    ++testN;
    return status == Fail;
}

typedef struct { 
    int x, y; 
    long long d2; 
} Pt;

static int cmp_pt(const void* a, const void* b) {
    const Pt* pa = (const Pt*)a; 
    const Pt* pb = (const Pt*)b;
    if (pa->d2 != pb->d2) {
        return (pa->d2 > pb->d2) - (pa->d2 < pb->d2);        
    }
    if (pa->x != pb->x) {
        return pa->x - pb->x;
    }
    return pa->y - pb->y;
}

static int feederBig1(void) {
    FILE* const in = fopen("in.txt", "w+");
    DWORD t; 
    const int N = 200000;
    const int K = 50;

    if (!in) {
        printf("can't create in.txt. No space on disk?\n"); 
        return -1; 
    }

    printf("Creating large dataset with one close point (k=%d)... ", K); 
    fflush(stdout);
    t = GetTickCount();
    fprintf(in, "0 0 %d\n%d\n1 1\n", K, N);
    for (int i = 1; i < N; i++) {
        if (fprintf(in, "%d %d\n", 100000 + i, 100000 + i) == EOF) {
            printf("can't write to in.txt. No space on disk?\n"); 
            fclose(in); 
            return -1;
        }
    }
    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Timeout=100ms... ", (unsigned)t / 1000); fflush(stdout);
    LabTimeout = 3000; 
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 24 * N;
    return 0;
}

static int checkerBig1(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n"); 
        ++testN; 
        return -1; 
    }
    const int K = 50; 
    Pt actual[50], expected[50]; 
    const char* status = Pass; 
    int i;
    for (i = 0; i < K; i++) {
        if (fscanf(out, "%d %d", &actual[i].x, &actual[i].y) != 2) {
            status = Fail; 
            break;
        }
        actual[i].d2 = (long long)actual[i].x * actual[i].x + (long long)actual[i].y * actual[i].y;
    }
    if (status == Pass) {
        qsort(actual, K, sizeof(Pt), cmp_pt);
        expected[0] = (Pt){1, 1, 2};
        for (i = 1; i < K; ++i) { 
            int v = 100000 + i;
            expected[i] = (Pt){v, v, 2LL * v * v}; 
        }
        qsort(expected, K, sizeof(Pt), cmp_pt);
        for (i = 0; i < K; i++) {
            if (actual[i].x != expected[i].x || actual[i].y != expected[i].y) {
                status = Fail; 
                break;
            }
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
    const int K = 100;
    if (!in) { 
        printf("can't create in.txt. No space on disk?\n");
        return -1; 
    }
    printf("Creating large grid (k=%d)... ", K); 
    fflush(stdout);
    t = GetTickCount();
    int total = 445 * 445;
    fprintf(in, "%d %d %d\n%d\n", 100000, 100000, K, total);
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
    printf("done in T=%u seconds. Timeout=100ms... ", (unsigned)t / 1000); 
    fflush(stdout);
    LabTimeout = 3000;
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 24 * total;
    return 0;
} 

static int checkerBig2(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) { 
        printf("can't open out.txt\n"); 
        ++testN;
        return -1; 
    }
    const int K = 100; 
    Pt actual[100], candidates[1000]; 
    const char* status = Pass; 
    int i, cnt, di, dj, x, y; 
    long long dx, dy;
    for (i = 0; i < K; ++i) {
        if (fscanf(out, "%d %d", &actual[i].x, &actual[i].y) != 2) {
            status = Fail;
            break;
        }
        dx = (long long)actual[i].x - 100000; 
        dy = (long long)actual[i].y - 100000;
        actual[i].d2 = dx * dx + dy * dy;
    }
    if (status == Pass) {
        qsort(actual, K, sizeof(Pt), cmp_pt);
        cnt = 0;
        for (di = -10; di <= 10; di++) {
            for (dj = -10; dj <= 10; dj++) {
                x = (100 + di) * 1000; 
                y = (100 + dj) * 1000;
                dx = (long long)x - 100000; 
                dy = (long long)y - 100000;
                candidates[cnt++] = (Pt){x, y, dx*dx + dy*dy};
            }
        }
        qsort(candidates, cnt, sizeof(Pt), cmp_pt);
        for (i = 0; i < K; i++) {
            if (actual[i].x != candidates[i].x || actual[i].y != candidates[i].y) { 
                status = Fail; 
                break; 
            }
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
    const int K = 200;
    if (!in) { 
        printf("can't create in.txt. No space on disk?\n");
        return -1; 
    }
    printf("Creating large diagonal dataset (k=%d)... ", K); 
    fflush(stdout);
    t = GetTickCount();
    fprintf(in, "50001 50000 %d\n%d\n", K, 150000);
    for (int i = 0; i < 150000; ++i) {
        if (fprintf(in, "%d %d\n", i, i) == EOF) {
            printf("can't write to in.txt. No space on disk?\n"); 
            fclose(in); 
            return -1;
        }
    }
    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Timeout=100ms... ", (unsigned)t / 1000); fflush(stdout);
    LabTimeout = 3000; 
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 24 * 150000;
    return 0;
}

static int checkerBig3(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) { 
        printf("can't open out.txt\n"); 
        ++testN; 
        return -1; 
    }
    const int K = 200; 
    Pt actual[200], candidates[1000]; 
    const char* status = Pass; 
    int i, cnt; 
    long long dx, dy;
    for (i = 0; i < K; i++) {
        if (fscanf(out, "%d %d", &actual[i].x, &actual[i].y) != 2) { 
            status = Fail; 
            break; 
        }
        dx = 50001LL - actual[i].x; 
        dy = 50000LL - actual[i].y;
        actual[i].d2 = dx * dx + dy * dy;
    }
    if (status == Pass) {
        qsort(actual, K, sizeof(Pt), cmp_pt);
        cnt = 0;
        for (i = 49850; i <= 50150; i++) {
            dx = 50001LL - i; 
            dy = 50000LL - i;
            candidates[cnt++] = (Pt){i, i, dx*dx + dy*dy};
        }
        qsort(candidates, cnt, sizeof(Pt), cmp_pt);
        for (i = 0; i < K; i++) {
            if (actual[i].x != candidates[i].x || actual[i].y != candidates[i].y) { 
                status = Fail; 
                break; 
            }
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

/* ========== Регистрация ========== */
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
    {feederBig3, checkerBig3}
};

TLabTest GetLabTest(int testIdx) { 
    return LabTests[testIdx]; 
}
int GetTestCount(void) { 
    return sizeof(LabTests) / sizeof(LabTests[0]); 
}
const char* GetTesterName(void) {
    return "Lab K-Nearest-Neighbours"; 
}
int GetTestTimeout(void) { 
    return LabTimeout; 
}
size_t GetTestMemoryLimit(void) { 
    return LabMemoryLimit; 
}