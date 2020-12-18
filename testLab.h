#pragma once

#include <stddef.h>
#include <stdio.h>

typedef struct {
    int (*Feeder)(void);
    int (*Checker)(void);
} TLabTest;

TLabTest GetLabTest(int testIdx);
int GetTestCount(void);
const char* GetTesterName(void);
int GetTestTimeout(void);
size_t GetTestMemoryLimit(void);

int HaveGarbageAtTheEnd(FILE* out);

extern const char Pass[];
extern const char Fail[];
const char* ScanUintUint(FILE* out, unsigned* a, unsigned* b);
const char* ScanIntInt(FILE* out, int* a, int* b);
const char* ScanInt(FILE* out, int* a);
const char* ScanChars(FILE* out, size_t bufferSize, char* buffer);

size_t GetLabPointerSize(void);

unsigned RoundUptoThousand(unsigned int n);

#define MIN_PROCESS_RSS_BYTES (1024*1024*sizeof(void*)/2)

#ifdef _WIN32
// Helpers for Windows
#include <windows.h>
#else
// Helpers and compatibility for Linux
#include <strings.h>
#define _strnicmp(a, b, c) strncasecmp((a), (b), (c))
#include <stdint.h>
typedef uint32_t DWORD;
DWORD GetTickCount(void);
#endif
