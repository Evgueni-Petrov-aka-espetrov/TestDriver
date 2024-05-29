#include "testlab8-base.h"
#include "testLab.h"

static int SpecialFeed(void);
static int SpecialCheck(void);

static int Test34Timeout = 1000;
int GetSpecialTimeout(void) {
    return Test34Timeout;
}

static size_t Test34MemoryLimit = MIN_PROCESS_RSS_BYTES;
size_t GetSpecialMemoryLimit(void) {
    return Test34MemoryLimit;
}

TLabTest GetSpecialLabTest(void)
{
    TLabTest labTest = { SpecialFeed,SpecialCheck };
    return labTest;
}

int SpecialFeed(void) {
    static unsigned vertexcount = MAX_VERTEX_COUNT1;
    static unsigned edgecount = MAX_VERTEX_COUNT1 - 1;
    FILE* const in = fopen("in.txt", "w+");
    if (in == NULL) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large text... ");
    fflush(stdout);
    unsigned start = GetTickCount();
    unsigned length = 1;
    fprintf(in, "%u\n%u\n", vertexcount, edgecount);
    for (unsigned begin = 1; begin < MAX_VERTEX_COUNT1; begin++)
    {
        fprintf(in, "%u %u %u\n", begin, begin + 1, begin);
    }
    fclose(in);
    start = RoundUptoThousand(GetTickCount() - start);


    printf("done in T=%u seconds. Starting exe with timeout T+1 seconds... ", start / 1000);     //ТУТ ВСЕ ЧТО КАСАЕТСЯ ВРЕМЕНИ И ПАМЯТИ
    fflush(stdout);

    Test34Timeout = (int)start + 3000;
    Test34MemoryLimit = vertexcount * vertexcount * 4 + MIN_PROCESS_RSS_BYTES;

    return 0;
}

static unsigned GoodEdge(unsigned a, unsigned b)
{
    if ((b != a + 1 && a != b + 1) ||               // changes in condition
        !(0 < a && a < MAX_VERTEX_COUNT1 + 1) ||
        !(0 < b && b < MAX_VERTEX_COUNT1 + 1)) {
        return IGNORED_EDGE_IDX;
    }
    else {
        return a > b ? b : a;
    }
}

int SpecialCheck(void)
{
    FILE* const out = fopen("out.txt", "r");
    if (out == NULL) {
        printf("can't open out.txt\n");
        IncreaseTestcaseIdx(); // replace with a function
        return -1;
    }
    const char* status = Pass;
    const unsigned vertexCount = MAX_VERTEX_COUNT1;
    unsigned vertexParent[MAX_VERTEX_COUNT1];
    unsigned long long length = 0;
    unsigned long long mst_length = SumRange(1, MAX_VERTEX_COUNT1 - 1);
    InitParent(vertexCount, vertexParent);
    for (unsigned idx = 0; idx + 1 < vertexCount; ++idx) {
        unsigned a, b;
        if (ScanUintUint(out, &a, &b) != Pass) {
            status = Fail;
            break;
        }
        const unsigned edgeIdx = GoodEdge(a, b);
        if (edgeIdx == IGNORED_EDGE_IDX) {
            printf("wrong output -- ");
            status = Fail;
            break;
        }
        const unsigned rootA = FindRoot(a - 1, vertexParent);
        const unsigned rootB = FindRoot(b - 1, vertexParent);
        if (rootA == rootB) {
            printf("wrong output -- ");
            status = Fail;
            break;
        }
        vertexParent[rootA] = rootB;
        length += edgeIdx;
    }
    if (status == Pass) {
        if (CountRoots(vertexCount, vertexParent) != 1 || length > mst_length) {
        
                printf("wrong output -- ");
           
            status = Fail;
        }
    }
    if (status == Pass && HaveGarbageAtTheEnd(out)) {
        status = Fail;
    }

    fclose(out);
    printf("%s\n", status);
    IncreaseTestcaseIdx(); // replace with a function
    return status == Fail;
}
