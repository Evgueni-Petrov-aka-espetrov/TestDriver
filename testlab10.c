#include "testLab.h"
#include <limits.h>
#include <stdio.h>
#include <string.h>
#define MAX_POINT_COUNT 100000

typedef struct 
{
    int x, y;
} TPoint;

static int testN = 0;
static const struct { const char *const in, *const outError; int pointCount; TPoint outPoints[8]; } testInOut[] = {
    { "4\n0 0\n2 1\n4 0\n0 4\n", NULL, 3, { {0, 0}, {4, 0}, {0, 4} } }, // 1
    { "4\n2 9\n2 1\n2 -6\n2 4\n", NULL, 2, { {2, -6}, {2, 9} } }, // 2
    { "100001\n", "bad number of points", 0, { {0, 0} } }, // 3
    { "5\n1 1\n4 5\n", "bad number of lines", 0, { {0, 0} } }, // 4
    { "-1\n", "bad number of points", 0, { {0, 0} } }, // 5
    { "3\n1 2\n4 3\n5\n", "bad number of lines", 0, { {0, 0} } }, // 6
    { "0\n", NULL, 0, { {0, 0} } }, // 7
    { "1\n2 3\n", NULL, 1, { {2, 3} } }, // 8
    { "2\n1 1\n4 5\n", NULL, 2, { {1, 1}, {4, 5} } }, // 9
    { "4\n4 4\n2 3\n4 0\n0 4\n", NULL, 3, { {4, 4}, {4, 0}, {0, 4} } }, // 10
    { "4\n4 0\n2 1\n0 0\n0 4\n", NULL, 3, { {0, 0}, {0, 4}, {4, 0} } }, // 11
    { "5\n0 0\n2 3\n4 0\n0 4\n4 4\n", NULL, 4, { {0, 0}, {4, 0}, {0, 4}, {4, 4} } }, //12
    { "8\n7 0\n4 0\n0 0\n3 0\n2 0\n1 0\n5 0\n6 0\n", NULL, 2, { {0, 0}, { 7, 0 } } }, // 13
    { "8\n0 7\n0 4\n0 0\n0 3\n0 2\n0 1\n0 5\n0 6\n", NULL, 2, { {0, 0}, {0, 7} } }, // 14
    { "8\n3 3\n-1 -1\n5 5\n-4 -4\n1 1\n2 2\n-2 -2\n3 3\n", NULL, 2, { {-4, -4}, {5, 5} } }, // 15
    { "8\n2 4\n-2 4\n0 0\n-3 9\n1 1\n-1 1\n3 9\n0 -6\n", NULL, 3, { {3, 9}, {-3, 9}, {0, -6} } }, // 16
    { "8\n1 -1\n-1 -1\n2 -4\n-3 -9\n-2 -4\n0 0\n3 -9\n0 6\n", NULL, 3, { {3, -9}, {-3, -9}, {0, 6} } }, // 17
    { "6\n-4 -1\n2 2\n1 4\n-2 -2\n-1 -4\n4 1\n", NULL, 4, { {4, 1}, {1, 4}, {-4, -1}, {-1, -4} } }, // 18
    { "6\n3 3\n1 4\n-1 -4\n-4 -1\n4 1\n-3 -3\n", NULL, 6, { {4, 1}, {1, 4}, {-4, -1}, {-1, -4}, {3, 3}, {-3, -3} } }, // 19
    { "8\n4 -3\n3 4\n-3 -4\n-4 3\n4 3\n3 -4\n-4 -3\n-3 4\n", NULL, 8, { {-3, -4}, {3, -4}, {4, -3}, {4, 3}, {3, 4}, {-3, 4}, {-4, 3}, {-4, -3} } }, // 20
    { "4\n-2147483648 0\n0 2147483647\n0 -2147483648\n2147483647 0\n", NULL, 4, { {0, INT_MIN}, {INT_MAX, 0}, {0, INT_MAX}, {INT_MIN, 0} } }, // 21
    { "5\n-2147483648 0\n0 2147483647\n0 -2147483648\n2147483647 0\n0 0\n", NULL, 4, { {0, INT_MIN}, {INT_MAX, 0}, {0, INT_MAX}, {INT_MIN, 0} } }, // 22
    { "5\n2147483647 0\n-2147483648 0\n-2147483647 1\n2147483647 2\n-2147483648 2\n", NULL, 4, { {INT_MAX, 0}, {INT_MAX, 2}, {INT_MIN, 2}, {INT_MIN, 0} } }, // 23
    { "8\n-2147483648 0\n0 2147483647\n0 -2147483648\n2147483647 0\n-2147483648 -2147483648\n-2147483648 2147483647\n2147483647 -2147483648\n2147483647 2147483647\n", NULL, 4, { {INT_MIN, INT_MIN}, {INT_MAX, INT_MIN}, {INT_MAX, INT_MAX}, {INT_MIN, INT_MAX} } }, // 24
    { "3\n0 2147483647\n0 -2147483648\n1 2147483647\n", NULL, 3, { {0, INT_MAX}, {1, INT_MAX}, {0, INT_MIN} } }, // 25
    { "3\n0 2147483647\n0 -2147483648\n-1 2147483647\n", NULL, 3, { {0, INT_MAX}, {-1, INT_MAX}, {0, INT_MIN} } }, // 26
    { "3\n2147483647 0\n-2147483648 0\n2147483647 -1\n", NULL, 3, { {INT_MAX, 0}, {INT_MAX, -1}, {INT_MIN, 0} } }, // 27
    { "3\n2147483647 0\n-2147483648 0\n2147483647 1\n", NULL, 3, { {INT_MAX, 0}, {INT_MAX, 1}, {INT_MIN, 0} } }, // 28
    { "4\n2147483647 0\n-2147483648 0\n0 1\n-2147483648 3\n", NULL, 3, { {INT_MAX, 0}, {INT_MIN, 3}, {INT_MIN, 0} } }, // 29
    { "4\n2147483647 2147483647\n2147483647 2147483644\n-2147483648 -2147483648\n2147483646 2147483645\n", NULL, 3, { {INT_MAX, INT_MAX - 3}, {INT_MAX, INT_MAX}, {INT_MIN, INT_MIN} } }, // 30
    { "4\n2147483647 -2147483648\n-2147483648 2147483647\n-2147483648 2147483644\n-2147483647 2147483645\n", NULL, 3, { {INT_MAX, INT_MIN}, {INT_MIN, INT_MAX - 3}, {INT_MIN, INT_MAX} } }, // 31
};

const char* ScanPoint(FILE* file, TPoint* point)
{
    return ScanIntInt(file, &point->x, &point->y);
}

static int FeedFromArray(void)
{
    FILE *const in = fopen("in.txt", "w");
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
    int passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        return -1;
    }

    if (testInOut[testN].pointCount != 0) {
        int found[8] = {0};
        int foundCount = 0;

        for (int i = 0; i < testInOut[testN].pointCount; ++i) {
            TPoint point = { 0, 0 };
            if (ScanPoint(out, &point) == Fail) {
                fclose(out);
                printf("short output -- %s\n", Fail);
                return 1;
            }
            
            for (int j = 0; j < testInOut[testN].pointCount; ++j) {
                if (point.x == testInOut[testN].outPoints[j].x && point.y == testInOut[testN].outPoints[j].y) {
                    if (found[j] == 1) {
                        fclose(out);
                        printf("wrong output -- %s\n", Fail);
                        return 1;
                    }
                    
                    ++found[j];
                    ++foundCount;
                    break;
                }
            }
        }
        
        if (foundCount != testInOut[testN].pointCount) {
            fclose(out);
            printf("wrong output -- %s\n", Fail);
            return 1;
        }
    }   
    else if (testInOut[testN].outError != NULL) {
        char error[128] = {0};
        const char* status = ScanChars(out, sizeof(error), error);
        
        if (status == Pass &&  _strnicmp(testInOut[testN].outError, error, strlen(error)) != 0) {
            status = Fail;
        }
        
        if (status == Fail) {
            fclose(out);
            printf("wrong output -- ");
            return 1;
        }
    }
    
    passed = !HaveGarbageAtTheEnd(out);
    ++testN;
    fclose(out);
    printf("%s\n", (passed) ? Pass : Fail);
    return !passed;
}

static int LabTimeout;
static size_t LabMemoryLimit;

static int bigSegmentsN = 1;
static int FeederBigSegment(void)
{
    FILE *const in = fopen("in.txt", "w");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    printf("Creating big segment... ");
    fflush(stdout);
    DWORD t = GetTickCount();
    fprintf(in, "%d\n", MAX_POINT_COUNT);

    for (int i = MAX_POINT_COUNT - 1; i >= 0; --i) {
        int x = 0;
        int y = 0;

        if (bigSegmentsN != 2) {
            x = i;
        }
        if (bigSegmentsN != 1) {
            y = i;
        }
        
        if (fprintf(in, "%d %d\n", x, y) < 2) {
                fclose(in);
                printf("%s\n", "can't create in.txt. No space on disk?\n");
                return -1;
            }
    }

    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2*T+2... ", (unsigned)t/1000);
    LabTimeout = (int)t*2+2000;
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 3*sizeof(TPoint)*MAX_POINT_COUNT;
    fflush(stdout);
    fclose(in);

    return 0;    
}

static int CheckerBigSegment(void)
{
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        return -1;
    }

    TPoint pointOut[2] = { { 0, 0 }, {MAX_POINT_COUNT - 1, MAX_POINT_COUNT - 1} };
    if (bigSegmentsN == 1) {
        pointOut[1].y = 0;
    }
    else if (bigSegmentsN == 2) {
        pointOut[1].x = 0;
    }

    int found[2] = { 0 };
    int foundCount = 0;
    for (int i = 0; i < 2; ++i) {
        TPoint point = { 0, 0 };
        if (ScanPoint(out, &point) == Fail) {
            fclose(out);
            printf("short output -- %s\n", Fail);
            return 1;
        }  

        for (int j = 0; j < 2; ++j) {
            if (point.x == pointOut[j].x && point.y == pointOut[j].y) {
                if (found[j] == 1) {
                    fclose(out);
                    printf("wrong output -- %s\n", Fail);
                    return 1;
                }
                
                ++found[j];
                ++foundCount;
                break;
            }
        }
    }
    
    if (foundCount != 2) {
        fclose(out);
        printf("wrong output -- %s\n", Fail);
        return 1;
    }

    int passed = !HaveGarbageAtTheEnd(out);

    ++bigSegmentsN;
    fclose(out);
    printf("%s\n", (passed) ? Pass : Fail);
    return !passed;
}

static int bigTrianglesN = 1;
static int FeederBigTriangle(void)
{
    FILE *const in = fopen("in.txt", "w");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    printf("Creating big triangle... ");
    fflush(stdout);
    DWORD t = GetTickCount();
    fprintf(in, "%d\n", MAX_POINT_COUNT);
    fprintf(in, "%d %d\n", INT_MIN, INT_MIN);

    for (int i = 0; i < MAX_POINT_COUNT - 1; ++i) {
        int x = INT_MAX;
        int y = INT_MAX;
        if (bigTrianglesN == 1) {
            y -= i;
        }
        else if (bigTrianglesN == 2) {
            x -= i;
        }

        if (fprintf(in, "%d %d\n", x, y) < 2) {
            fclose(in);
            printf("can't create in.txt. No space on disk?\n");
            return -1;
        }
    }

    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2*T+2... ", (unsigned)t/1000);
    LabTimeout = (int)t*2+2000;
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 3*sizeof(TPoint)*MAX_POINT_COUNT;
    fflush(stdout);
    fclose(in);

    return 0;    
}

static int CheckerBigTriangle(void)
{
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        return -1;
    }

    TPoint pointOut[3] =  { { INT_MIN, INT_MIN }, { INT_MAX, INT_MAX }, { INT_MAX, INT_MAX } };
    if (bigTrianglesN == 1) {
        pointOut[2].y = INT_MAX - MAX_POINT_COUNT + 2;
    }
    else if (bigTrianglesN == 2) {
        pointOut[2].x = INT_MAX - MAX_POINT_COUNT + 2;
    } 

    int found[3] = { 0 };
    int foundCount = 0;
    for (int i = 0; i < 3; ++i) {
        TPoint point = { 0, 0 };
        if (ScanPoint(out, &point) == Fail) {
            fclose(out);
            printf("short output -- %s\n", Fail);
            return 1;
        }

        for (int j = 0; j < 3; ++j) {
            if (point.x == pointOut[j].x && point.y == pointOut[j].y) {
                if (found[j] == 1) {
                    fclose(out);
                    printf("wrong output -- %s\n", Fail);
                    return 1;
                }
                
                ++found[j];
                ++foundCount;
                break;
            }
        }
    }
    
    if (foundCount != 3) {
        fclose(out);
        printf("wrong output -- %s\n", Fail);
        return 1;
    }

    int passed = !HaveGarbageAtTheEnd(out);

    ++bigTrianglesN;
    fclose(out);
    printf("%s\n", (passed) ? Pass : Fail);
    return !passed;
}

static int bigParabolsN = 1;
static int FeederBigParabol(void)
{
    FILE *const in = fopen("in.txt", "w");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    printf("Creating big parabol... ");
    fflush(stdout);
    DWORD t = GetTickCount();
    fprintf(in, "%d\n", MAX_POINT_COUNT);

    for (long long i = 1; i <= MAX_POINT_COUNT / 4; ++i) {
        int printCount = 0;
        if (bigParabolsN == 1) {
            printCount += fprintf(in, "%lld %lld\n", i, i*i + INT_MIN);
            printCount += fprintf(in, "%lld %lld\n", -i, i*i + INT_MIN);
            i += MAX_POINT_COUNT / 4;
            printCount += fprintf(in, "%lld %lld\n", i, i*i + INT_MIN);
            printCount += fprintf(in, "%lld %lld\n", -i, i*i + INT_MIN);
            i -= MAX_POINT_COUNT / 4;
        }
        else if (bigParabolsN == 2) {
            printCount += fprintf(in, "%lld %lld\n", i, -i*i + INT_MAX);
            printCount += fprintf(in, "%lld %lld\n", -i, -i*i + INT_MAX);
            i += MAX_POINT_COUNT / 4;
            printCount += fprintf(in, "%lld %lld\n", i, -i*i + INT_MAX);
            printCount += fprintf(in, "%lld %lld\n", -i, -i*i + INT_MAX);
            i -= MAX_POINT_COUNT / 4;
        }
        else if (bigParabolsN == 3) {
            printCount += fprintf(in, "%lld %lld\n", i*i + INT_MIN, i);
            printCount += fprintf(in, "%lld %lld\n", i*i + INT_MIN, -i);
            i += MAX_POINT_COUNT / 4;
            printCount += fprintf(in, "%lld %lld\n", i*i + INT_MIN, i);
            printCount += fprintf(in, "%lld %lld\n", i*i + INT_MIN, -i);
            i -= MAX_POINT_COUNT / 4;
        }
        else if (bigParabolsN == 4) {
            printCount += fprintf(in, "%lld %lld\n", i, i*i);
            printCount += fprintf(in, "%lld %lld\n", -i, i*i);
            printCount += fprintf(in, "%lld %lld\n", i, -i*i);
            printCount += fprintf(in, "%lld %lld\n", -i, -i*i);
        }

        if (printCount < 4) {
            fclose(in);
            printf("can't create in.txt. No space on disk?\n");
            return -1;
        }
    }

    t = RoundUptoThousand(GetTickCount() - t);
    printf("done in T=%u seconds. Starting exe with timeout 2*T+2... ", (unsigned)t/1000);
    LabTimeout = (int)t*2+2000;
    LabMemoryLimit = MIN_PROCESS_RSS_BYTES + 3*sizeof(TPoint)*MAX_POINT_COUNT;
    fflush(stdout);
    fclose(in);

    return 0;    
}

static int CheckerBigParabol(void)
{
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        return -1;
    }

    if(bigParabolsN == 1 || bigParabolsN == 2 || bigParabolsN == 3) {
        int found[MAX_POINT_COUNT + 1] = { 0 };
        int foundCount = 0;

        for (int i = 0; i < MAX_POINT_COUNT; ++i) {
            TPoint point = { 0, 0 };
            if (ScanPoint(out, &point) == Fail) {
                fclose(out);
                printf("short output -- %s\n", Fail);
                return 1;
            }

            int index = MAX_POINT_COUNT / 2;
            index += (bigParabolsN == 3) ? point.y : point.x;
            if (found[index] == 1) {
                fclose(out);
                printf("wrong output -- %s\n", Fail);
                return 1;
            }

            ++found[index];
            ++foundCount;
        }

        if (foundCount != MAX_POINT_COUNT) {
            fclose(out);
            printf("wrong output -- %s\n", Fail);
            return 1;
        }
    }
    else if (bigParabolsN == 4){
        int x = MAX_POINT_COUNT / 4;
        TPoint pointOut[4] =  { {x, x*x}, {x, -x*x}, {-x, x*x}, {-x, -x*x} };

        int found[4] = { 0 };
        int foundCount = 0;
        for (int i = 0; i < 4; ++i) {
            TPoint point = { 0, 0 };
            if (ScanPoint(out, &point) == Fail) {
                fclose(out);
                printf("short output -- %s\n", Fail);
                return 1;
            }

            for (int j = 0; j < 4; ++j) {
                if (point.x == pointOut[j].x && point.y == pointOut[j].y) {
                    if (found[j] == 1) {
                        fclose(out);
                        printf("wrong output -- %s\n", Fail);
                        return 1;
                    }
                    
                    ++found[j];
                    ++foundCount;
                    break;
                }
            }
        }

        if (foundCount != 4) {
        fclose(out);
        printf("wrong output -- %s\n", Fail);
        return 1;
        }
    }

    int passed = !HaveGarbageAtTheEnd(out);

    ++bigParabolsN;
    fclose(out);
    printf("%s\n", (passed) ? Pass : Fail);
    return !passed;
}

const TLabTest LabTests[] = {
    {FeedFromArray, CheckFromArray}, // 1
    {FeedFromArray, CheckFromArray}, // 2
    {FeedFromArray, CheckFromArray}, // 3
    {FeedFromArray, CheckFromArray}, // 4
    {FeedFromArray, CheckFromArray}, // 5
    {FeedFromArray, CheckFromArray}, // 6
    {FeedFromArray, CheckFromArray}, // 7
    {FeedFromArray, CheckFromArray}, // 8  
    {FeedFromArray, CheckFromArray}, // 9
    {FeedFromArray, CheckFromArray}, // 10
    {FeedFromArray, CheckFromArray}, // 11
    {FeedFromArray, CheckFromArray}, // 12    
    {FeedFromArray, CheckFromArray}, // 13
    {FeedFromArray, CheckFromArray}, // 14
    {FeedFromArray, CheckFromArray}, // 15
    {FeedFromArray, CheckFromArray}, // 16    
    {FeedFromArray, CheckFromArray}, // 17
    {FeedFromArray, CheckFromArray}, // 18
    {FeedFromArray, CheckFromArray}, // 19
    {FeedFromArray, CheckFromArray}, // 20   
    {FeedFromArray, CheckFromArray}, // 21
    {FeedFromArray, CheckFromArray}, // 22
    {FeedFromArray, CheckFromArray}, // 23
    {FeedFromArray, CheckFromArray}, // 24
    {FeedFromArray, CheckFromArray}, // 25
    {FeedFromArray, CheckFromArray}, // 26
    {FeedFromArray, CheckFromArray}, // 27
    {FeedFromArray, CheckFromArray}, // 28
    {FeedFromArray, CheckFromArray}, // 29
    {FeedFromArray, CheckFromArray}, // 30
    {FeedFromArray, CheckFromArray}, // 31
    {FeederBigSegment, CheckerBigSegment}, // 32
    {FeederBigSegment, CheckerBigSegment}, // 33
    {FeederBigSegment, CheckerBigSegment}, // 34
    {FeederBigTriangle, CheckerBigTriangle}, // 35
    {FeederBigTriangle, CheckerBigTriangle}, // 36
    {FeederBigParabol, CheckerBigParabol}, // 37
    {FeederBigParabol, CheckerBigParabol}, // 38
    {FeederBigParabol, CheckerBigParabol}, // 39
    {FeederBigParabol, CheckerBigParabol}  // 40
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return sizeof(LabTests)/sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
    return "Lab 10 Convex hull";
}

static int LabTimeout = 3000;
int GetTestTimeout() {
    return LabTimeout;
}

static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;
size_t GetTestMemoryLimit() {
    return LabMemoryLimit;
}
