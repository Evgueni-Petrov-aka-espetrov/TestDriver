#include "testlab8-base.h"
#include <stdlib.h>
#include <assert.h>
#include <math.h>

TTestcaseData Lab8SpecialTest(enum ETestcaseDataId dataId, unsigned edgeIdx) {
    const unsigned gridSize = (unsigned)sqrt(MAX_VERTEX_COUNT);
    const unsigned vertexCount = gridSize * gridSize;
    const unsigned edgeCountHalf = vertexCount - gridSize;
    const unsigned edgeCount = 2 * edgeCountHalf;
    switch (dataId) {
        case VERTEX_COUNT:
        case MST_LENGTH:
            return MakeInteger(vertexCount);
        case EDGE_COUNT:
            return MakeInteger(edgeCount);
        case EDGE:
            assert(edgeIdx < edgeCount);
            unsigned begin, end;
            unsigned edgeIndex = edgeIdx % edgeCountHalf;
            if (edgeIdx < edgeCountHalf) {
                begin = edgeIndex / (gridSize - 1) * gridSize + edgeIndex % (gridSize - 1);
                end = begin + 1;
            } else {
                begin = edgeIndex;
                end = begin + gridSize;
            }
            return MakeEdge(begin + 1, end + 1, 1);
        case ERROR_MESSAGE:
            return MakeString(NULL);
        default:
            abort();
    }
}
