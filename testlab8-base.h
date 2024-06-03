#pragma once
#include "testLab.h"


extern int LabTimeout;
extern size_t LabMemoryLimit;

enum { IGNORED_VERTEX_IDX = 0 };
static const unsigned IGNORED_EDGE_IDX = (unsigned)-1;
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

void CalcRowColumn(unsigned linearIdx, unsigned* rowIdx, unsigned* columnIdx);
char* GetTesterName8_x(void);

int SpecialFeed();
unsigned SumRange(unsigned begin, unsigned end);

TTestcaseData MakeInteger(unsigned long long integer);
TTestcaseData MakeEdge(unsigned begin, unsigned end, unsigned long long length);
TTestcaseData MakeString(const char* string);


TTestcaseData Lab8SpecialTest(enum ETestcaseDataId dataId, unsigned edgeIdx);





