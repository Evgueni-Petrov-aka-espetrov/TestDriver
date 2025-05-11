#include "testLab.h"
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

static int testN = 0;
static const struct {const char *const in, *const out[16]; int n;} testInOut[] = {
    {"4\n0 0\n3 2\n6 2\n5 0\n0\n3 1", {"IN"}, 1},
    {"4\n0 0\n0 6\n6 6\n6 0\n1\n4\n3 3\n3 5\n5 5\n5 3\n2 2\n4 4", {"IN", "OUT"}, 2},
    {"2\n0 0\n6 6\n1\n4\n3 3\n3 5\n5 5\n5 3\n2 2\n4 4", {"bad input"}, 1},
    {"4\n0 0\n10 0\n10 10\n0 10\n1\n8\n8 5\n8 8\n5 8\n5 7\n7 7\n7 6\n6 6\n6 5\n5 5\n6 6\n7 7\n9 9\n1 1\n11 11\n6 5", {"IN","OUT","OUT","IN","IN","OUT","OUT"}, 7},
    {"", {"bad input"}, 1},
    {"6\n1 2\n3 9\n7 11\n11 9\n9 5\n5 4\n1\n3\n4 5\n7 10\n8 6\n3 1\n2 3\n3 4\n6 6", {"OUT", "IN", "IN", "OUT"}, 4},
    {"3\n2 2\n8 13\n12 2\n1\n8\n6 4\n7 4", {"bad input"}, 1},
    {"4\n0 0\n0 5\n5 5\n5 0\n0\n0 0\n",{"IN"}, 1},
    {"3\n0 0\n2 2\n4 4\n0\n1 1\n",{"IN"}, 1},
    {"4\n0 0\n1 0\n1 1\n0 1\n-5\n",{"bad input"}, 1},
    {"3\n0 0\n1 x\n2 2\n0\n",{"bad input"}, 1},
    {"5\n", {"bad input"}, 1},
    {"5\n0 0\n0 6\n6 6\n6 0\n3 3\n1\n3\n2 2\n4 2\n3 3\n3 3\n3 4\n7 7\n",{"OUT","IN","OUT"}, 3},
    {"0\n1\n3\n0 0\n1 1\n2 2\n1 1\n",{"bad input"}, 1},
    {"6\n0 0\n5 0\n5 5\n3 5\n3 2\n0 2\n0\n2 1\n4 4\n-1 0\n5 2\n",{"IN","IN","OUT","IN"}, 4},
    {"4\n0 0\n0 4\n4 4\n4 0\n1\n4\n0 0\n0 4\n4 4\n4 0\n2 2\n5 5\n",{"OUT","OUT"}, 2},
    {"4\n0 0\n1 0\n1 1\n0 1\n1\n2\n0 0\n1 1\n",{"bad input"}, 1},
    {"4\n-5 -5\n-5 0\n0 0\n0 -5\n1\n3\n-4 -4\n-4 -1\n-1 -1\n-3 -3\n-6 -6\n-2 -2\n",{"OUT","OUT","OUT"}, 3},
    {"6\n0 0\n4 0\n4 2\n2 2\n2 4\n0 4\n0\n1 1\n3 1\n3 3\n1 3\n",{"IN","IN","OUT","IN"}, 4},
    {"3\n2 2\n8 13\n12 2\n1\n8\n6 4\n7 4\n8 5\n8 6\n7 7\n6 7\n5 6\n5 5\n3 2\n6 5\n7 2\n8 8\n13 8\n12 2", {"IN","OUT","IN","IN","OUT","IN"}, 6},
    {"5\n1 1\n4 10\n12 4\n7 6\n5 5\n2\n3\n2 3\n5 6\n4 8\n3\n6 6\n7 7\n5 8\n2 1\n3 5\n4 6\n4 5\n6 7\n10 7\n3 10", {"OUT", "OUT", "OUT", "OUT", "OUT", "OUT", "OUT"},7},
    {"6\n3 3\n1 5\n5 11\n8 7\n8 2\n5 1\n2\n3\n3 7\n6 7\n5 9\n4\n4 3\n6 3\n6 6\n3 6\n6 2\n7 3\n7 6\n8 4\n2 5",{"IN", "IN", "IN", "IN", "IN"}, 5},
    {"7\n6 3\n1 5\n2 9\n1 11\n7 11\n5 8\n4 6\n2\n4\n4 7\n3 8\n3 10\n5 10\n4\n4 4\n2 5\n3 7\n4 5\n3 5\n3 6\n2 8\n2 10\n3 11\n4 9\n3 9\n4 10\n5 1", {"OUT", "OUT","IN","IN","IN","OUT","OUT","OUT","OUT"},9},
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

static int FeederBigSquare(void) {
    FILE *const in = fopen("in.txt", "w");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large text... ");
    fflush(stdout);

    DWORD t = GetTickCount();

    srand((unsigned)time(NULL));

    fprintf(in, "16000\n");
    for (int y = 0; y < 4000; ++y) {
        fprintf(in, "0 %d\n", y);
    }
    for (int x = 0; x < 4000; ++x) {
        fprintf(in, "%d 4000\n", x);
    }
    for (int y = 4000; y > 0; --y) {
        fprintf(in, "4000 %d\n", y);
    }
    for (int x = 4000; x > 0; --x) {
        fprintf(in, "%d 0\n", x);
    }

    fprintf(in, "1000\n");
    const int cell_size = 5;
    const int grid_size = (4000 - 4) / cell_size;
    const int dx[8] = {1, 2, 3, 3, 2, 1, 0, 0};
    const int dy[8] = {0, 0, 1, 2, 3, 3, 2, 1};

    int* used = malloc(sizeof(int) * grid_size * grid_size);
    if (!used) {
        exit(1);
    }

    memset(used, 0, sizeof(used));

    int placed = 0;
    while (placed < 1000) {
        int ix = rand() % grid_size;
        int iy = rand() % grid_size;
        if (used[iy * grid_size + ix]) continue;
        used[iy * grid_size + ix] = 1;
        int x_base = 1 + ix * cell_size;
        int y_base = 1 + iy * cell_size;

        fprintf(in, "8\n");
        for (int k = 0; k < 8; ++k) {
            fprintf(in, "%d %d\n", x_base + dx[k], y_base + dy[k]);
        }
        ++placed;
    }

    for (int i = 0; i < 4000; ++i) {
        int x = (rand() % 6001) - 1000;
        int y = (rand() % 6001) - 1000;
        fprintf(in, "%d %d\n", x, y);
    }

    fclose(in);

    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2... ", (unsigned)(t / 1000));
    fflush(stdout);
    free(used);
    return 0;
}

static int CheckerBigSquare(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }

    const char* status = Pass;

    for (int i = 0; i < 4000; ++i) {
        char line[256];
        if (ScanChars(out, sizeof(line), line) != Pass) {
            status = Fail;
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

static int FeederBigTriangle(void) {
    FILE *const in = fopen("in.txt", "w");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large text... ");
    fflush(stdout);

    DWORD t = GetTickCount();
    srand((unsigned)time(NULL));


    int total_vertices = 16000;
    fprintf(in, "%d\n", total_vertices);
    for (int y = 0; y < 6000; ++y) {
        fprintf(in, "0 %d\n", y);
    }

    for (int i = 1; i <= 2000; ++i) {
        int x = 4 * i;
        int y = 6000 - 3 * i;
        fprintf(in, "%d %d\n", x, y);
    }

    for (int x = 7999; x >= 0; --x) {
        fprintf(in, "%d 0\n", x);
    }

    fprintf(in, "1000\n");
    const int cell_size = 5;
    const int grid_w = (8000 - 4) / cell_size;
    const int grid_h = (6000 - 4) / cell_size;
    const int dx[8] = {1, 2, 3, 3, 2, 1, 0, 0};
    const int dy[8] = {0, 0, 1, 2, 3, 3, 2, 1};
    unsigned char used[grid_w][grid_h];
    memset(used, 0, sizeof(used));
    int placed = 0;
    while (placed < 1000) {
        int ix = rand() % grid_w;
        int iy = rand() % grid_h;
        if (used[ix][iy]) continue;
        used[ix][iy] = 1;
        int xb = 1 + ix * cell_size;
        int yb = 1 + iy * cell_size;
        fprintf(in, "8\n");
        for (int k = 0; k < 8; ++k) {
            fprintf(in, "%d %d\n", xb + dx[k], yb + dy[k]);
        }
        placed++;
    }


    for (int i = 0; i < 4000; ++i) {
        int xq = (rand() % 10001) - 2000;
        int yq = (rand() % 10001) - 2000;
        fprintf(in, "%d %d\n", xq, yq);
    }

    fclose(in);
    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2... ", (unsigned)(t / 1000));
    return 0;
}

static int CheckerBigTriangle(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }

    const char* status = Pass;

    for (int i = 0; i < 4000; ++i) {
        char line[256];
        if (ScanChars(out, sizeof(line), line) != Pass) {
            status = Fail;
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

const TLabTest LabTests[] = {
    {FeedFromArray, CheckFromArray}, //1
    {FeedFromArray, CheckFromArray}, //2
    {FeedFromArray, CheckFromArray}, //3
    {FeedFromArray, CheckFromArray}, //4
    {FeedFromArray, CheckFromArray}, //5
    {FeedFromArray, CheckFromArray}, //6
    {FeedFromArray, CheckFromArray}, //7
    {FeedFromArray, CheckFromArray}, //8
    {FeedFromArray, CheckFromArray}, //9
    {FeedFromArray, CheckFromArray}, //10
    {FeedFromArray, CheckFromArray}, //11
    {FeedFromArray, CheckFromArray}, //12
    {FeedFromArray, CheckFromArray}, //13
    {FeedFromArray, CheckFromArray}, //14
    {FeedFromArray, CheckFromArray}, //15
    {FeedFromArray, CheckFromArray}, //16
    {FeedFromArray, CheckFromArray}, //17
    {FeedFromArray, CheckFromArray}, //18
    {FeedFromArray, CheckFromArray}, //19
    {FeedFromArray, CheckFromArray}, //20
    {FeedFromArray, CheckFromArray}, //21
    {FeedFromArray, CheckFromArray}, //22
    {FeedFromArray, CheckFromArray}, //23
    {FeederBigSquare, CheckerBigSquare}, //24
    {FeederBigTriangle, CheckerBigTriangle} //25
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return sizeof(LabTests)/sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
    return "Lab Ray Casting";
}

int GetTestTimeout(void) {
    return 2000;
}

size_t GetTestMemoryLimit(void) {
    return MIN_PROCESS_RSS_BYTES;
}
