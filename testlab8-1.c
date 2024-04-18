#include "testlab8-base.h"
#include <stdlib.h>
#include <assert.h>

TTestcaseData Lab8SpecialTest(enum ETestcaseDataId dataId, unsigned edgeIdx) {
    const unsigned vertexCount = MAX_VERTEX_COUNT;
    const unsigned edgeCount = SumRange(1, vertexCount - 1);
    switch (dataId) {
        case VERTEX_COUNT:
        case MST_LENGTH:
            return MakeInteger(vertexCount);
        case EDGE_COUNT:
            return MakeInteger(edgeCount);
        case EDGE:
            assert(edgeIdx < edgeCount);
            unsigned row, column;
            CalcRowColumn(edgeIdx, &row, &column);
            return MakeEdge(row + 1, column + 1, 1);
        case ERROR_MESSAGE:
            return MakeString(NULL);
        default:
            abort();
    }
}
