#include "testLab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

static int testN = 0;

static const struct {const char *const in; const char* const out;} testInOut[] = {              
    {"2 3\n1 1\n6 6\n10 10\n1\n5 5\n", "6 6\n1 1"},
    {"1 1\n3 4\n1\n0 0\n", "3 4"},
    {"5 5\n", "bad input"}, 
    {"1 1\n1 1\n1\nabc 1\n", "bad input"},
    {"1 1\n1 1\n1\n5.5 1\n", "bad input"}, 
    {"1 2\n0 0\n10 10\n2\n1 1\n9 9\n", "0 0\n10 10"}, 
    {"2 2\n1 1\n10 10\n1\n9 9\n", "10 10\n1 1"},
    {"2 3\n-10 -10\n0 0\n-3 -4\n1\n-5 -5\n", "-3 -4\n-10 -10"},  
    {"2 3\n1 2\n7 8\n10 10\n1\n7 8\n", "7 8\n10 10"}, 
    {"3 3\n-5 0\n3 0\n10 0\n1\n0 0\n", "3 0\n-5 0\n10 0"},
    {"1 3\n0 0\n10 0\n0 10\n3\n1 0\n9 0\n0 9\n", "0 0\n10 0\n0 10"},
    {"3 4\n1 0\n0 2\n-3 0\n5 5\n1\n0 0\n", "1 0\n0 2\n-3 0"},
    {"4 5\n0 0\n10 10\n20 20\n90 90\n95 95\n1\n100 100\n", "95 95\n90 90\n20 20\n10 10"},
    {"3 5\n50 50\n51 51\n1 1\n2 2\n-5 -5\n1\n0 0\n", "1 1\n2 2\n-5 -5"},
    {"2 4\n0 0\n10 10\n3 6\n4 8\n1\n3 7\n", "3 6\n4 8"},
    {"3 3\n3 4\n6 0\n0 6\n1\n0 0\n", "3 4\n0 6\n6 0"}, 
    {"4 4\n10 20\n15 5\n8 8\n12 12\n1\n0 0\n", "8 8\n15 5\n12 12\n10 20"},
    {"3 4\n0 100\n101 1\n49 51\n52 48\n1\n50 50\n", "49 51\n52 48\n0 100"}, 
    {"2 4\n-10 -10\n10 -10\n0 0\n-1 2\n1\n-1 1\n", "-1 2\n0 0"},
    {"1 1\n0 3\n1\n0 0\n", "0 3"},
    {"3 3\n5 5\n5 5\n5 5\n1\n5 5\n", "5 5\n5 5\n5 5"},
    {"4 4\n0 0\n1 0\n2 0\n3 0\n1\n1000 0\n", "3 0\n2 0\n1 0\n0 0"},
    {"2 4\n-5 15\n-12 8\n-9 11\n-10 9\n1\n-10 10\n", "-10 9\n-9 11"},
    {"2 3\n1 1\n2 2\n3 3\n2\n0 0\n4 4\n", "1 1\n2 2\n3 3\n2 2"}, 
    {"2 3\n10 10\n20 20\n15 21\n1\n15 20\n", "15 21\n20 20"}
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
        printf("can't open out.txt\n"); 
        ++testN; 
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
} Point;

enum TestType {
    BIG1,
    BIG2,
    BIG3
};

static int ComparePoints(const void* a, const void* b) {
    const Point* pa = (const Point*)a; 
    const Point* pb = (const Point*)b;
    if (pa->d2 != pb->d2) {
        return (pa->d2 > pb->d2) - (pa->d2 < pb->d2);        
    }
    if (pa->x != pb->x) {
        return pa->x - pb->x;
    }
    return pa->y - pb->y;
}

static void MakeDiag(Point* pts, int count, int x0, int y0, int dx, int dy) {
    for (int i = 0; i < count; ++i) {
        pts[i].x = x0 + i * dx;
        pts[i].y = y0 + i * dy;
    }
}

static void FillPoints(Point* pts, int count, enum TestType type, int isTarget) {
    if (type == BIG1) {
        if (isTarget) {
            MakeDiag(pts, count, 0, 0, 10, 10);
        } else {
            MakeDiag(pts, count, 0, 0, 1, 1);
        }
    } else if (type == BIG2) {
        if (isTarget) {
            MakeDiag(pts, count, 0, 0, 5, 5);
        } else {
            int side = (int)sqrt(count);
            int idx = 0;
            for (int i = 0; i < side; ++i) {
                for (int j = 0; j < side; ++j) {
                    if (idx < count) {
                        pts[idx].x = i * 10;
                        pts[idx].y = j * 10;
                        ++idx;
                    }
                }
            }
        }
    } else if (type == BIG3) {
        if (isTarget) {
            MakeDiag(pts, count, 0, 1, 1, 1);
        } else {
            MakeDiag(pts, count, 0, 0, 1, 1);
        }
    }
}

static int WritePointsToFile(FILE* f, const Point* pts, int count) {
    for (int i = 0; i < count; ++i) {
        if (fprintf(f, "%d %d\n", pts[i].x, pts[i].y) == EOF) {
            return -1;
        }
    }
    return 0;
}

static int GenerateAndWrite(FILE* f, int count, enum TestType type, int isTarget) {
    Point* pts = malloc(count * sizeof(Point));
    if (pts == NULL) {
        printf("memory allocation error\n");
        return -1;
    }

    FillPoints(pts, count, type, isTarget);

    if (WritePointsToFile(f, pts, count) == -1) {
        printf("can't write to in.txt. No space on disk?\n");
        free(pts);
        return -1;
    }

    free(pts);
    return 0;
}

static int CommonFeeder(int k, int m, int n, enum TestType type, const char* name) {
    FILE* const in = fopen("in.txt", "w+");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n"); 
        return -1; 
    }

    printf("%s: M=%d, N=%d... ", name, m, n);
    fflush(stdout);
    DWORD t = GetTickCount();

    fprintf(in, "%d %d\n", k, m);
    if (GenerateAndWrite(in, m, type, 0) == -1) {
        fclose(in);
        return -1;
    }

    fprintf(in, "%d\n", n);
    if (GenerateAndWrite(in, n, type, 1) == -1) {
        fclose(in);
        return -1;
    }

    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u s. ", (unsigned)t / 1000);
    fflush(stdout);
    LabTimeout = 4000; 
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 40 * m; 
    return 0;
}

static const int Big1M = 100000;
static const int Big1K = 5;
static const int Big1N = 100;

static int feederBig1(void) {
    return CommonFeeder(Big1K, Big1M, Big1N, BIG1, "Big1");
}

static const int Big2M = 90000;
static const int Big2K = 5;
static const int Big2N = 50;

static int feederBig2(void) {
    return CommonFeeder(Big2K, Big2M, Big2N, BIG2, "Big2");
}

static const int Big3M = 150000;
static const int Big3K = 2;
static const int Big3N = 200;

static int feederBig3(void) {
    return CommonFeeder(Big3K, Big3M, Big3N, BIG3, "Big3");
}

static int RunChecker(int k, int m, int n, enum TestType type, const Point* targets) {
    FILE* const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't create in.txt. No space on disk?\n"); 
        return -1; 
    }

    Point* base = malloc(m * sizeof(Point));
    if (base == NULL) {
        fclose(out);
        printf("memory allocation error\n"); 
        return -1;
    }
    FillPoints(base, m, type, 0); 

    const char* status = Pass;
    for (int q = 0; q < n; ++q) {
        for (int i = 0; i < m; ++i) {
            long long dx = (long long)base[i].x - targets[q].x;
            long long dy = (long long)base[i].y - targets[q].y;
            base[i].d2 = dx * dx + dy * dy;
        }

        qsort(base, m, sizeof(Point), ComparePoints);

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
        if (status == Fail) {
            break;
        }
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

static int checkerBig1(void) {
    Point* targets = malloc(Big1N * sizeof(Point));
    if (targets == NULL) {
        printf("memory allocation error\n"); 
        return -1;
    }
    FillPoints(targets, Big1N, BIG1, 1);

    int result = RunChecker(Big1K, Big1M, Big1N, BIG1, targets);
    free(targets); 
    return result;
}

static int checkerBig2(void) {
    Point* targets = malloc(Big2N * sizeof(Point));
    if (targets == NULL) {
        printf("memory allocation error\n"); 
        return -1;
    }
    FillPoints(targets, Big2N, BIG2, 1);

    int result = RunChecker(Big2K, Big2M, Big2N, BIG2, targets);
    free(targets); 
    return result;
}

static int checkerBig3(void) {
    Point* targets = malloc(Big3N * sizeof(Point));
    if (targets == NULL) {
        printf("memory allocation error\n"); 
        return -1;
    }
    FillPoints(targets, Big3N, BIG3, 1);

    int result = RunChecker(Big3K, Big3M, Big3N, BIG3, targets);
    free(targets); 
    return result;
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
