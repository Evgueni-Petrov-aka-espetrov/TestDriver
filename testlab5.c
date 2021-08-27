#include "testLab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int TestN = 0;

typedef struct {
    const char *const Input;
    size_t InputLength;
    size_t CompressedLength;
} TTestCase;

#define CalcCompressedLength(abSize, bitsPerChar, charCount) (4+(10*(abSize)+(charCount)*(bitsPerChar)+7)/8+100)

static const TTestCase TestCases[] = {
    {"0123456789ABCD\r\n" "0123456789ABCD\r\n" "0123456789ABCD\r\n" "0123456789ABCD\r\n"
    "0123456789ABCD\r\n" "0123456789ABCD\r\n" "0123456789ABCD\r\n" "0123456789ABCD\r\n"
    "0123456789ABCD\r\n", 16*9, CalcCompressedLength(16, 4, 16*9)}, // 1 2
    {"", 0, CalcCompressedLength(0, 0, 0)}, // 3 4
    {"z", 1, CalcCompressedLength(1, 1, 1)}, // 5 6
    {"zz", 2, CalcCompressedLength(1, 1, 2)}, // 7 8
    {"zzzz", 4, CalcCompressedLength(1, 1, 4)}, // 9 10
    {"\xFF\xFF\xFF\xFF", 4, CalcCompressedLength(1, 1, 4)}, // 11 12
    {"zzzz\xFF\xFF\xFF\xFF", 8, CalcCompressedLength(2, 1, 8)}, // 13 14
    {"\n\r\n\r", 4, CalcCompressedLength(2, 1, 4)}, // 15 16
    {"\r\n\r\n", 4, CalcCompressedLength(2, 1, 4)}, // 17 18
    {"\n\n\n\n", 4, CalcCompressedLength(1, 1, 4)}, // 19 20
    {"\r\r\r\r", 4, CalcCompressedLength(1, 1, 4)}, // 21 22
    {"\0\0\0\0", 4, CalcCompressedLength(1, 1, 4)}, // 23 24
    {"\0\1\0\1", 4, CalcCompressedLength(2, 1, 4)}
};

#undef CalcCompressedLength

static unsigned char Input[32*1024*1024];
static size_t InputLength = 0;

static unsigned char LabOutput[sizeof(Input) + 1];
static size_t LabOutputLength = 0;

static int IsInputTooLarge(size_t inputLength) {
    if (inputLength > sizeof(Input)) {
        printf("Tester error: generated input is too large");
        return 1;
    }
    return 0;
}

static int FeedData(char mode, const unsigned char* data, size_t count) {
    FILE *const input = fopen("in.txt", "wb");
    int error = input == NULL || fprintf(input, "%c", mode) != 1 || fwrite(data, 1, count, input) != count;
    fclose(input);
    if (error) {
        printf("can't create in.txt. No space on disk?\n");
    }
    return error;
}

static int FeedCompress(void) {
    if (TestN%2 != 0) {
        printf("Tester error: compression test must preceed decompression test\n");
        return 1;
    }
    return FeedData('c', Input, InputLength);
}

static int CheckAtMost(int gotBytes, int expectedBytes) {
    if (gotBytes > expectedBytes) {
        printf("output is too long, got %d bytes, expect at most %d bytes -- ", gotBytes, expectedBytes);
        return 0;
    }
    return 1;
}

static int CheckCompress(size_t expectedLength) {
    FILE *const out = fopen("out.txt", "rb");
    if (!out) {
        printf("can't open out.txt\n");
        TestN++;
        return -1;
    }
    if (expectedLength+1 > sizeof(LabOutput)) {
        printf("Tester error: expected output is too large");
        return -1;
    }
    LabOutputLength = fread(LabOutput, 1, expectedLength+1, out);
    fclose(out);
    if (CheckAtMost(LabOutputLength, expectedLength)) {
        printf("PASSED\n");
        TestN++;
        return 0;
    } else {
        printf("FAILED\n");
        TestN++;
        return 1;
    }
}

static int FeedCompressFromArray(void) {
    InputLength = TestCases[TestN/2].InputLength;
    memcpy(Input, TestCases[TestN/2].Input, InputLength);
    return FeedCompress();
}

static int CheckCompressFromArray(void) {
    return CheckCompress(TestCases[TestN/2].CompressedLength);
}

static int FeedDecompress(void) {
    if (TestN%2 != 1) {
        printf("Tester error: compression test must preceed decompression test\n");
        return 1;
    }
    return FeedData('d', LabOutput, LabOutputLength);
}

static int CheckSame(int gotBytes, int expectedBytes) {
    if (gotBytes != expectedBytes) {
        printf("output is wrong, got %d bytes, expect exactly %d bytes -- ", gotBytes, expectedBytes);
        return 0;
    }
    return 1;
}

static int CheckDecompress(void) {
    FILE *const out = fopen("out.txt", "rb");
    if (!out) {
        printf("can't open out.txt\n");
        TestN++;
        return -1;
    }
    const size_t outputLength = fread(LabOutput, 1, InputLength+1, out);
    fclose(out);
    int passed = CheckSame(outputLength, InputLength);
    if (passed && memcmp(LabOutput, Input, InputLength) != 0) {
        passed = 0;
        printf("output is wrong -- ");
    }
    if (passed) {
        printf("PASSED\n");
        TestN++;
        return 0;
    } else {
        printf("FAILED\n");
        TestN++;
        return 1;
    }
}

static unsigned AlphabetSize = 16;
static unsigned CalcFib(unsigned n) {
    static unsigned f[128] = {1, 1};
    if (n >= sizeof(f)/sizeof(f[0])) {
        printf("Tester error: out of range in CalcFib\n");
        exit(1);
    }
    if (f[n] == 0) {
        f[n] = CalcFib(n-1)+CalcFib(n-2);
    }
    return f[n];
}

static int FeedCompressBig1(void) {
    InputLength = 0;
    for (unsigned char c = 0; c < AlphabetSize; ++c) {
        if (IsInputTooLarge(InputLength + CalcFib(c))) {
            return -1;
        }
        memset(Input + InputLength, c, CalcFib(c));
        InputLength += CalcFib(c);
    }
    return FeedCompress();
}

static int CheckCompressBig1(void) {
    unsigned bits = CalcFib(0)*(AlphabetSize-1);
    for (unsigned char c = 1; c < AlphabetSize; ++c) {
        bits += CalcFib(c)*(AlphabetSize-c);
    }
    int expectedLength = 4+(10*AlphabetSize+bits+7)/8;
    AlphabetSize = AlphabetSize == 16 ? 21
        : AlphabetSize == 21 ? 26
        : AlphabetSize == 26 ? 31
        : AlphabetSize == 31 ? 35
        : AlphabetSize == 35 ? 41
        : 41;
    return CheckCompress(expectedLength);
}

static int FeedCompressBig2(void) {
    InputLength = 256*256;
    if (IsInputTooLarge(InputLength)) {
        return -1;
    }
    for (size_t i = 0; i < InputLength; ++i) {
        Input[i] = i & 0xff;
    }
    return FeedCompress();
}

static int CheckCompressBig2(void) {
    return CheckCompress(4+(10*256+256*256*8+7)/8);
}

static int FeedCompressBig3(void) {
    InputLength = 256*256*256;
    if (IsInputTooLarge(InputLength)) {
        return -1;
    }
    for (size_t i = 0; i < InputLength; ++i) {
        Input[i] = 'x';
    }
    return FeedCompress();
}

static int CheckCompressBig3(void) {
    return CheckCompress(4+(9+256*256*256+7)/8);
}

const TLabTest LabTests[] = {
    {FeedCompressFromArray, CheckCompressFromArray}, // 1
    {FeedDecompress, CheckDecompress}, // 2
    {FeedCompressFromArray, CheckCompressFromArray}, // 3
    {FeedDecompress, CheckDecompress}, // 4
    {FeedCompressFromArray, CheckCompressFromArray}, // 5
    {FeedDecompress, CheckDecompress}, // 6
    {FeedCompressFromArray, CheckCompressFromArray}, // 7
    {FeedDecompress, CheckDecompress}, // 8
    {FeedCompressFromArray, CheckCompressFromArray}, // 9
    {FeedDecompress, CheckDecompress}, // 10
    {FeedCompressFromArray, CheckCompressFromArray}, // 11
    {FeedDecompress, CheckDecompress}, // 12
    {FeedCompressFromArray, CheckCompressFromArray}, // 13
    {FeedDecompress, CheckDecompress}, // 14
    {FeedCompressFromArray, CheckCompressFromArray}, // 15
    {FeedDecompress, CheckDecompress}, // 16
    {FeedCompressFromArray, CheckCompressFromArray}, // 17
    {FeedDecompress, CheckDecompress}, // 18
    {FeedCompressFromArray, CheckCompressFromArray}, // 19
    {FeedDecompress, CheckDecompress}, // 20
    {FeedCompressFromArray, CheckCompressFromArray}, // 21
    {FeedDecompress, CheckDecompress}, // 22
    {FeedCompressFromArray, CheckCompressFromArray}, // 23
    {FeedDecompress, CheckDecompress}, // 24
    {FeedCompressFromArray, CheckCompressFromArray}, // 25
    {FeedDecompress, CheckDecompress}, // 26
    {FeedCompressBig1, CheckCompressBig1}, // 27 n=16
    {FeedDecompress, CheckDecompress}, // 28
    {FeedCompressBig2, CheckCompressBig2}, // 29
    {FeedDecompress, CheckDecompress}, // 30
    {FeedCompressBig3, CheckCompressBig3}, // 31
    {FeedDecompress, CheckDecompress}, // 32
    {FeedCompressBig1, CheckCompressBig1}, // 33 n=21
    {FeedDecompress, CheckDecompress}, // 34
    {FeedCompressBig1, CheckCompressBig1}, // 35 n=26
    {FeedDecompress, CheckDecompress}, // 36
    {FeedCompressBig1, CheckCompressBig1}, // 37 n=31
    {FeedDecompress, CheckDecompress}, // 38
    {FeedCompressBig1, CheckCompressBig1}, // 39 n=35
    {FeedDecompress, CheckDecompress}, // 40
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return sizeof(LabTests)/sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
    return "Lab 5 Huffman";
}

int GetTestTimeout() {
    return 6000;
}

size_t GetTestMemoryLimit() {
    return MIN_PROCESS_RSS_BYTES;
}
