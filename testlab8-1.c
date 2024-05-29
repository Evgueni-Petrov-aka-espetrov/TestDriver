#include "testlab8-base.h"
#include "testLab.h"
#include <stdbool.h>

enum {
    VERTEX_FOR_PRIM =7000
};
static int SpecialFeed(void);
static int SpecialCheck(void);

static int Test34Timeout = 5000;


int GetSpecialTimeout(void){
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
    const unsigned vertexcount = VERTEX_FOR_PRIM;
    const unsigned edgecount = SumRange(1, VERTEX_FOR_PRIM - 1);
    FILE* const in = fopen("in.txt", "w+");
    if (in == NULL) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    printf("Creating large text... ");
    fflush(stdout);
    unsigned start = GetTickCount();
    unsigned length = SumRange(1, VERTEX_FOR_PRIM - 1);
    fprintf(in, "%u\n%u\n", vertexcount, edgecount);
    for (unsigned begin = 1;begin < VERTEX_FOR_PRIM ;begin++)
    {
        for (unsigned end = begin + 1; end < VERTEX_FOR_PRIM; end++)
        {
            fprintf(in, "%u %u %u\n", begin, end, length);
            length--;
        }
    }
    for (unsigned end = 1;end < VERTEX_FOR_PRIM;end++)
    {
        fprintf(in, "%d %u %u\n", VERTEX_FOR_PRIM, end, length);
        length--;
    }
    
    fclose(in);
    start = RoundUptoThousand(GetTickCount() - start);


    printf("done in T=%u seconds. Starting exe with timeout T+4 seconds... ", start / 1000);
    fflush(stdout);



    Test34Timeout = (int)start + 60000;                                         //ÒÓÒ ÂÑÅ ×ÒÎ ÊÀÑÀÅÒÑß ÂÐÅÌÅÍÈ È ÏÀÌßÒÈ
    Test34MemoryLimit = vertexcount * vertexcount * 4 + MIN_PROCESS_RSS_BYTES;

    return 0;


}
static unsigned GoodEdge(unsigned a, unsigned b)
{
    if (!(0 < a && a <= VERTEX_FOR_PRIM && 
        0 < b && b <= VERTEX_FOR_PRIM &&
        a!=b))
    {
        printf("error1 %u %u\n", a,b);

        return IGNORED_EDGE_IDX;
    }
    else if (a != VERTEX_FOR_PRIM && b != VERTEX_FOR_PRIM) {
        {
            printf("error2 %u %u\n", a, b);
            return IGNORED_EDGE_IDX;
        }
    }
    else {
      
        return a == VERTEX_FOR_PRIM ? b : a;
        
    }
   
}



static bool Treeisconnected(bool* vertexConnected)
{
    for (int i = 1;i < VERTEX_FOR_PRIM ;i++)
    {
        if (!vertexConnected[i])
        {
            printf("not conncted %d\n", i);
            return false;
        }
    }
    return true;
}



int SpecialCheck(void)
{
    FILE* const out = fopen("out.txt", "r");
    if (out == NULL) {
        printf("can't open out.txt\n");
        IncreaseTestcaseIdx();
        return -1;
    }
    const char* status = Pass;
    const unsigned vertexCount = VERTEX_FOR_PRIM;
    bool vertexConnected[VERTEX_FOR_PRIM] = { false };
    unsigned long long length = 0;
    for (unsigned idx = 0; idx + 1 < vertexCount; ++idx) {
        unsigned a, b;
        if (ScanUintUint(out, &a, &b) != Pass) {
            status = Fail;
            break;
        }
        const unsigned edgeIdx = GoodEdge(a, b);
        if (edgeIdx == IGNORED_EDGE_IDX) {
            printf("wrong output1 -- ");
            status = Fail;
            break;
        }
        vertexConnected[edgeIdx] = true;
    }
    if (status == Pass) {
        if (!Treeisconnected(vertexConnected))
        {
            printf("wrong output2 -- ");
            status = Fail;
        }
    }

    if (status == Pass && HaveGarbageAtTheEnd(out)) {
        status = Fail;
    }

    fclose(out);
    printf("%s\n", status);
    IncreaseTestcaseIdx();
    return status == Fail;

}