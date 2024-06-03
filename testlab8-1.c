#include "testlab8-base.h"
#include "testLab.h"
#include <stdlib.h>


enum {
    VERTEX_FOR_PRIM = 7000
};

char* GetTesterName8_x(void)
{
    return "Lab 8-1 Prim Shortest Spanning Tree";
}

int SpecialFeed()
{
    FILE* const in = fopen("in.txt", "w+");
    if (in == NULL) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }

    const unsigned vertexCount = VERTEX_FOR_PRIM;
    const unsigned edgeCount = SumRange(1, VERTEX_FOR_PRIM - 1);
    fprintf(in, "%u\n%u\n", vertexCount, edgeCount);

    printf("Creating large text... ");
    unsigned start = GetTickCount();

    for (unsigned begin = 1, lenght = 1; begin < vertexCount; begin++)
    {
        for (unsigned end = begin + 1; end < vertexCount + 1;end++, lenght++)
        {
            fprintf(in, "%u %u %u\n", begin, end, lenght);
        }
    }

    fclose(in);
    start = RoundUptoThousand(GetTickCount() - start);
    printf("done in T=%u seconds. Starting exe with timeout T+3 seconds... ", start / 1000);
    LabTimeout = (int)start + 3000;
    LabMemoryLimit = vertexCount * vertexCount * 4 + MIN_PROCESS_RSS_BYTES;

    return 0;
}

TTestcaseData Lab8SpecialTest(enum ETestcaseDataId dataId, unsigned edgeIdx) {
    unsigned partOneEdge = VERTEX_FOR_PRIM - 1;
    unsigned partTwoEdge = SumRange(1, VERTEX_FOR_PRIM - 2);

    switch (dataId) {
    case VERTEX_COUNT:
        return MakeInteger(VERTEX_FOR_PRIM);
    case EDGE_COUNT:
        return MakeInteger(partOneEdge + partTwoEdge);
    case EDGE:
        if (edgeIdx < partOneEdge)
        {
            return MakeEdge(1, edgeIdx + 2, edgeIdx + 1);
        }
        else {
            unsigned row, col;
            CalcRowColumn(edgeIdx - partOneEdge, &row, &col);
            row = VERTEX_FOR_PRIM - row + col;
            col += 2;
            return MakeEdge(row, col, edgeIdx + 1);
        }
    case ERROR_MESSAGE:
        return MakeString(NULL);
    case MST_LENGTH:
        return MakeInteger(SumRange(1, VERTEX_FOR_PRIM - 1));
    default:
        abort();
    }

}
