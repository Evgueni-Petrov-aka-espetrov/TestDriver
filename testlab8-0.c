#include "testlab8-base.h"
#include "testLab.h"

enum {
    VERTEX_FOR_KRUSKAL = 20000
};
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
    static unsigned vertexcount = VERTEX_FOR_KRUSKAL;
    static unsigned edgecount = VERTEX_FOR_KRUSKAL - 1;
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
    for (unsigned begin = 1; begin < VERTEX_FOR_KRUSKAL; begin++)
    {
        fprintf(in, "%u %u %u\n", begin, begin + 1, begin);
    }
    fclose(in);
    start = RoundUptoThousand(GetTickCount() - start);


    printf("done in T=%u seconds. Starting exe with timeout T+1 seconds... ", start / 1000);     //ÒÓÒ ÂÑÅ ×ÒÎ ÊÀÑÀÅÒÑß ÂÐÅÌÅÍÈ È ÏÀÌßÒÈ
    fflush(stdout);

    Test34Timeout = 1000;
    Test34MemoryLimit = 1700000000 + MIN_PROCESS_RSS_BYTES;  // ÷òîáû çàïóñòèëñÿ ïðèì, êðàñêë ïðîñèò ìåíüøå

    return 0;
}

static unsigned GoodEdge(unsigned a, unsigned b)
{
    if ((b != a + 1 && a != b + 1) ||               // changes in condition
        !(0 < a && a < VERTEX_FOR_KRUSKAL + 1) ||
        !(0 < b && b < VERTEX_FOR_KRUSKAL + 1)) {
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
    const unsigned vertexCount = VERTEX_FOR_KRUSKAL;
    unsigned vertexParent[VERTEX_FOR_KRUSKAL];
    unsigned long long length = 0;
    unsigned long long mst_length = SumRange(1, VERTEX_FOR_KRUSKAL - 1);
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
