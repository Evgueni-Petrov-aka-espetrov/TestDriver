#pragma once
#include "testLab.h"
enum { MAX_VERTEX_COUNT = 5000,
   
    MAX_VERTEX_COUNT1 = 5000
};

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



unsigned SumRange(unsigned begin, unsigned end);
void InitParent(unsigned vertexCount, unsigned parent[]);
int CountRoots(unsigned vertexCount, const unsigned parent[]);
int FindRoot(unsigned vertex, const unsigned parent[]);
int GetSpecialTimeout(void);
size_t GetSpecialMemoryLimit(void);
TLabTest GetSpecialLabTest(void);
void IncreaseTestcaseIdx(void);



