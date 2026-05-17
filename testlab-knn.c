#include "testLab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

static int testN = 0;

static const struct {const char *const in; const char* const out;} testInOut[] = {              
    {"2 3\n1 1\n6 6\n10 10\n1\n5 5", "6 6\n1 1"},
    {"1 1\n3 4\n1\n0 0", "3 4"},
    {"5 5", "bad input"}, 
    {"1 1\n1 1\n1\nabc 1", "bad input"},
    {"1 1\n1 1\n1\n5.5 1", "bad input"}, 
    {"2 2\n1 1\n10 10\n1\n0 0", "1 1\n10 10"},
    {"2 2\n1 1\n10 10\n1\n9 9", "10 10\n1 1"},
    {"2 3\n-10 -10\n0 0\n-3 -4\n1\n-5 -5", "-3 -4\n-10 -10"},  
    {"2 3\n1 2\n7 8\n10 10\n1\n7 8", "7 8\n10 10"}, 
    {"3 3\n-5 0\n3 0\n10 0\n1\n0 0", "3 0\n-5 0\n10 0"},
    {"2 3\n0 -8\n0 4\n0 12\n1\n0 0", "0 4\n0 -8"},
    {"3 4\n1 0\n0 2\n-3 0\n5 5\n1\n0 0", "1 0\n0 2\n-3 0"},
    {"4 5\n0 0\n10 10\n20 20\n90 90\n95 95\n1\n100 100", "95 95\n90 90\n20 20\n10 10"},
    {"3 5\n50 50\n51 51\n1 1\n2 2\n-5 -5\n1\n0 0", "1 1\n2 2\n-5 -5"},
    {"2 4\n0 0\n10 10\n3 6\n4 8\n1\n3 7", "3 6\n4 8"},
    {"3 3\n3 4\n6 0\n0 6\n1\n0 0", "3 4\n0 6\n6 0"}, 
    {"4 4\n10 20\n15 5\n8 8\n12 12\n1\n0 0", "8 8\n15 5\n12 12\n10 20"},
    {"3 4\n0 100\n101 1\n49 51\n52 48\n1\n50 50", "49 51\n52 48\n0 100"}, 
    {"2 4\n-10 -10\n10 -10\n0 0\n-1 2\n1\n-1 1", "-1 2\n0 0"},
    {"1 1\n0 3\n1\n0 0", "0 3"},
    {"3 3\n5 5\n5 5\n5 5\n1\n5 5", "5 5\n5 5\n5 5"},
    {"4 4\n0 0\n1 0\n2 0\n3 0\n1\n1000 0", "3 0\n2 0\n1 0\n0 0"},
    {"2 4\n-5 15\n-12 8\n-9 11\n-10 9\n1\n-10 10", "-10 9\n-9 11"},
    {"3 4\n0 0\n51 51\n21 21\n30 30\n1\n25 25", "21 21\n30 30\n0 0"}, 
    {"2 3\n10 10\n20 20\n15 21\n1\n15 20", "15 21\n20 20"}
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

    char buffer[4096];
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

static void generate_base(Pt* base, int m, int type) {
    if (type == 1 || type == 3) { 
        for (int i = 0; i < m; ++i) {
            base[i].x = i;
            base[i].y = i;
        }
    } else if (type == 2) { 
        int side = (int)sqrt(m);
        int idx = 0;
        for (int i = 0; i < side; ++i) {
            for (int j = 0; j < side; ++j) {
                base[idx].x = i * 10;
                base[idx].y = j * 10;
                ++idx;
            }
        }
    }
}

static int run_checker(int k, int m, int n, int type, Pt* targets) {
    FILE* const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't create in.txt. No space on disk?\n"); 
        return -1; 
    }

    Pt* base = malloc(m * sizeof(Pt));
    generate_base(base, m, type);

    const char* status = Pass;

    for (int q = 0; q < n; ++q) {
        for (int i = 0; i < m; ++i) {
            long long dx = (long long)base[i].x - targets[q].x;
            long long dy = (long long)base[i].y - targets[q].y;
            base[i].d2 = dx * dx + dy * dy;
        }

        qsort(base, m, sizeof(Pt), cmp_pt);

        for (int i = 0; i < k; ++i) {
            int ax, ay;
            if (fscanf(out, "%d %d", &ax, &ay) != 2) {
                status = Fail;
                break;
            }
            if (ax != base[i].x || ay != base[i].y) {
                status = Fail;
                break;
            }
        }
        if (status == Fail) 
            break;
    }

    if (status == Pass && HaveGarbageAtTheEnd(out)) {
        status = Fail;
    }
    
    free(base);
    fclose(out);
    printf("%s\n", status);
    ++testN;
    return status == Fail;
}

static int feederBig1(void) {
    FILE* const in = fopen("in.txt", "w+");
    DWORD t; 
    const int M = 100000; 
    const int K = 5;      
    const int N = 100;    

    if (!in) {
        printf("can't create in.txt. No space on disk?\n"); 
        return -1; 
    }

    printf("Big1: M=%d, N=%d... ", M, N);
    fflush(stdout);
    t = GetTickCount();
    fprintf(in, "%d %d\n", K, M);

    for (int i = 0; i < M; ++i) {
        if (fprintf(in, "%d %d\n", i, i) == EOF) {
            printf("can't write to in.txt. No space on disk?\n"); 
            fclose(in); 
            return -1;
        }   
    }
    
    fprintf(in, "%d\n", N);
    for (int i = 0; i < N; ++i) {
        if (fprintf(in, "%d %d\n", i * 10, i * 10) == EOF) {
            printf("can't write to in.txt. No space on disk?\n"); 
            fclose(in); 
            return -1;
        }
    }

    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Timeout=100ms... ", (unsigned)t / 1000); 
    fflush(stdout);
    LabTimeout = 3000; 
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 20 * N;
    return 0;
}

static int checkerBig1(void) {
    Pt targets[50];
    for (int i = 0; i < 50; ++i) { 
        targets[i].x = i * 10; 
        targets[i].y = i * 10; 
    }
    return run_checker(5, 100000, 100, 1, targets);
}

static int feederBig2(void) {
    FILE *const in = fopen("in.txt", "w+");
    DWORD t; 
    const int Total = 300; 
    const int K = 5;
    const int N = 50;
    if (!in) { 
        printf("can't create in.txt. No space on disk?\n");
        return -1; 
    }
    printf("Big2: Grid %dx%d, N=%d... ", Total, Total, N);
    fflush(stdout);
    t = GetTickCount();

    fprintf(in, "%d %d\n", K, Total * Total);
    for (int i = 0; i < Total; ++i) {
        for (int j = 0; j < Total; ++j) {
            if (fprintf(in, "%d %d\n", i * 10, j * 10) == EOF) {
                printf("can't write to in.txt. No space on disk?\n"); 
                fclose(in); 
                return -1;
            }
        }
    }

    fprintf(in, "%d\n", N);
    for (int i = 0; i < N; ++i) {
        if (fprintf(in, "%d %d\n", i * 5, i * 5) == EOF) {
            printf("can't write to in.txt. No space on disk?\n"); 
            fclose(in); 
            return -1;
        }
    }

    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Timeout=100ms... ", (unsigned)t / 1000); 
    fflush(stdout);
    LabTimeout = 3000;
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 20 * Total * Total;
    return 0;
} 

static int checkerBig2(void) {
    Pt targets[20];
    for (int i = 0; i < 20; ++i) { 
        targets[i].x = i * 5; 
        targets[i].y = i * 5; 
    }
    return run_checker(5, 90000, 50, 2, targets);
}

static int feederBig3(void) {
    FILE *const in = fopen("in.txt", "w+");
    DWORD t; 
    const int M = 150000;
    const int K = 2;
    const int N = 200; 

    if (!in) { 
        printf("can't create in.txt. No space on disk?\n");
        return -1; 
    }

    printf("Big3: Diagonal M=%d, N=%d... ", M, N);
    fflush(stdout);
    t = GetTickCount();

    fprintf(in, "%d %d\n", K, M);
    for (int i = 0; i < M; ++i) {
        if (fprintf(in, "%d %d\n", i, i) == EOF) {
            printf("can't write to in.txt. No space on disk?\n"); 
            fclose(in); 
            return -1;
        }
    }

    fprintf(in, "%d\n", N);
    for (int i = 0; i < N; ++i) {
        if (fprintf(in, "%d %d\n", i, i + 1) == EOF) {
            printf("can't write to in.txt. No space on disk?\n"); 
            fclose(in); 
            return -1;
        }
    }

    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Timeout=100ms... ", (unsigned)t / 1000); fflush(stdout);
    LabTimeout = 3000; 
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 20 * M;
    return 0;
}

static int checkerBig3(void) {
    Pt targets[100];
    for (int i = 0; i < 100; ++i) { 
        targets[i].x = i + 1; 
        targets[i].y = i; 
    }
    return run_checker(2, 150000, 200, 3, targets);
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