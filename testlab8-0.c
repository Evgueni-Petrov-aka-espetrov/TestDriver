#include "testlab8-base.h"
#include "testLab.h"
#include <stdlib.h>
enum {
    VERTEX_FOR_KRUSKAL = 20000
};
char* GetTesterName8_x(void)
{
    return "Lab 8-0 Kruskal Shortest Spanning Tree";
}
int SpecialFeed()
{
    FILE* const in = fopen("in.txt", "w+");
    if (in == NULL) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    const unsigned vertexCount = VERTEX_FOR_KRUSKAL;
    const unsigned edgeCount = VERTEX_FOR_KRUSKAL - 1;
    fprintf(in, "%u\n%u\n", vertexCount, edgeCount);

    unsigned start = GetTickCount();
    for (unsigned begin = 1; begin < vertexCount; begin++)
    {
        fprintf(in, "%u %u %u\n", begin, begin + 1, begin);
    }

    fclose(in);
    start = RoundUptoThousand(GetTickCount() - start);

    LabTimeout = (int)start + 3000;
    LabMemoryLimit = 24 * (vertexCount + 1) * (vertexCount + 1) + 24 * vertexCount + MIN_PROCESS_RSS_BYTES;

    return 0;
}

TTestcaseData Lab8SpecialTest(enum ETestcaseDataId dataId, unsigned edgeIdx) {
    switch (dataId) {
    case VERTEX_COUNT:
        return MakeInteger(VERTEX_FOR_KRUSKAL);
    case EDGE_COUNT:
        return MakeInteger(VERTEX_FOR_KRUSKAL - 1);
    case EDGE:
        return MakeEdge(edgeIdx + 1, edgeIdx + 2, edgeIdx + 1);
    case ERROR_MESSAGE:
        return MakeString(NULL);
    case MST_LENGTH:
        return MakeInteger(SumRange(1, VERTEX_FOR_KRUSKAL - 1));
    default:
        abort();
    }
}
