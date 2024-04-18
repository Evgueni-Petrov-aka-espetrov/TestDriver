#pragma once

enum { MAX_VERTEX_COUNT = 6500 };

struct TEdge {
    unsigned Begin;
    unsigned End;
    unsigned long long Length;
};

typedef union {
    struct TEdge Edge;
    unsigned long long Integer;
    const char* String;
} TTestcaseData;

enum ETestcaseDataId {
    VERTEX_COUNT,
    EDGE_COUNT,
    EDGE,
    ERROR_MESSAGE,
    MST_LENGTH
};

TTestcaseData MakeInteger(unsigned long long integer);
TTestcaseData MakeString(const char* string);
TTestcaseData MakeEdge(unsigned begin, unsigned end, unsigned long long length);
unsigned SumRange(unsigned begin, unsigned end);
void CalcRowColumn(unsigned linearIdx, unsigned* rowIdx, unsigned* columnIdx);

TTestcaseData Lab8SpecialTest(enum ETestcaseDataId dataId, unsigned edgeIdx);
