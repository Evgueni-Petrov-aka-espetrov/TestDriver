#include "testLab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

static int testN = 0;

static unsigned char zipped[64*1024*1024];
static size_t lenZipped = 0;

static const struct {const char *const in; size_t nIn; double nBits; int header;} testIn[] = {
    {"0123456789ABCD\r\n"
    "0123456789ABCD\r\n"
    "0123456789ABCD\r\n"
    "0123456789ABCD\r\n"
    "0123456789ABCD\r\n"
    "0123456789ABCD\r\n"
    "0123456789ABCD\r\n"
    "0123456789ABCD\r\n"
    "0123456789ABCD\r\n", 16*9, 4, 4+160/8+100}, // 1 2
    {"", 0, 0, 0+100}, // 3 4
    {"z", 1, 1, 4+2+100}, // 5 6
    {"zz", 2, 1, 4+2+100}, // 7 8
    {"zzzz", 4, 1, 4+2+100}, // 9 10
    {"\xFF\xFF\xFF\xFF", 4, 1, 4+2+100}, // 11 12
    {"zzzz\xFF\xFF\xFF\xFF", 8, 1, 4+3+100}, // 13 14
    {"\n\r\n\r", 4, 1, 4+3+100}, // 15 16
    {"\r\n\r\n", 4, 1, 4+3+100}, // 17 18
    {"\n\n\n\n", 4, 1, 4+3+100}, // 19 20
    {"\r\r\r\r", 4, 1, 4+3+100}, // 21 22
    {"\0\0\0\0", 4, 1, 4+3+100}, // 23 24
    {"\0\1\0\1", 4, 1, 4+3+100}
};

static int FeedFromArray(void)
{
    FILE *const in = fopen("in.txt", "wb");
    int error = 0;
    if (!in) {
        error = 1;
    } else if (testN%2 == 0) { // ask to compress data
        const char *c, *const lastIn = testIn[testN/2].in+testIn[testN/2].nIn;
        error = EOF == fprintf(in, "c\r\n");
        for (c = testIn[testN/2].in; !error && c != lastIn; c++) {
            error |= fwrite(c, 1, 1, in) < 1;
        }
    } else { // ask to decompress compressed data
        const unsigned char *c, *const lastZipped = zipped+lenZipped;
        error = EOF == fprintf(in, "d\r\n");
        for (c = zipped; !error && c != lastZipped; c++) {
            error |= fwrite(c, 1, 1, in) < 1;
        }
    }
    fclose(in);
    if (error) {
        printf("can't create in.txt. No space on disk?\n");
    }
    return error;
}

static int CheckFromArray(void)
{
    FILE *const out = fopen("out.txt", "rb");
    int passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    if (testN%2 == 0) { // check the compression ratio is matched
        size_t maxLenZipped = (size_t)(ceil(testIn[testN/2].nIn*testIn[testN/2].nBits/8)+testIn[testN/2].header);
        lenZipped = fread(zipped, 1, maxLenZipped+1, out);
        if (lenZipped > maxLenZipped) {
            passed = 0;
            printf("output is too long -- ");
        }
    } else { // check that compress+decompress doesn't corrupt the data
        const size_t lenOut = fread(zipped, 1, testIn[testN/2].nIn+1, out);
        if (lenOut < testIn[testN/2].nIn) {
            passed = 0;
            printf("output is too short -- ");
        } else if (lenOut > testIn[testN/2].nIn) {
            passed = 0;
            printf("output is too long -- ");
        } else if (memcmp(zipped, testIn[testN/2].in, testIn[testN/2].nIn) != 0) {
            passed = 0;
            printf("output is wrong -- ");
        }
    }
    fclose(out);
    if (passed) {
        printf("PASSED\n");
        testN++;
        return 0;
    } else {
        printf("FAILED\n");
        testN++;
        return 1;
    }
}

static unsigned big1N = 16;
static unsigned fib(unsigned n)
{
    static unsigned f[128] = {1, 1};
    if (n >= sizeof(f)/sizeof(f[0])) {
        printf("Tester error: out of range in fib\n");
        exit(1);
    }
    if (f[n] == 0) {
        f[n] = fib(n-1)+fib(n-2);
    }
    return f[n];
}

static int feederBig1(void)
{
    FILE *const in = fopen("in.txt", "wb");
    int error = 0;
    if (!in) {
        error = 1;
    } else if (testN%2 == 0) { // ask to compress data
        unsigned char c;
        error = EOF == fprintf(in, "c\r\n");
        for (c = 0; !error && c < big1N; c++) {
            memset(zipped, c, fib(c));
            error |= fwrite(zipped, 1, fib(c), in) < fib(c);
        }
    } else { // ask to decompress compressed data
        const unsigned char *c, *const lastZipped = zipped+lenZipped;
        error = EOF == fprintf(in, "d\r\n");
        for (c = zipped; !error && c != lastZipped; c++) {
            error |= fwrite(c, 1, 1, in) < 1;
        }
    }
    fclose(in);
    if (error) {
        printf("can't create in.txt. No space on disk?\n");
    }
    return error;
}

static int checkerBig1(void)
{
    FILE *const out = fopen("out.txt", "rb");
    int passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    if (testN%2 == 0) { // check the compression ratio is matched
        unsigned char c;
        unsigned bits = fib(0)*(big1N-1);
        for (c = 1; c < big1N; c++) {
            bits += fib(c)*(big1N-c);
        }
        const size_t expectedSize = 4+(10*big1N+bits+7)/8;
        lenZipped = fread(zipped, 1, expectedSize+1, out);
        if (lenZipped > expectedSize) {
            passed = 0;
            printf("output is too long -- ");
        }
    } else { // check that compress+decompress doesn't corrupt the data
        const unsigned nIn = fib(big1N+1)-1;
        const size_t nOut = fread(zipped, 1, nIn+1, out);
        if (nOut < nIn) {
            passed = 0;
            printf("output is too short, must be %d -- ", nIn);
        } else if (nOut > nIn) {
            passed = 0;
            printf("output is too long -- ");
        } else {
            int z = 0;
            unsigned char c;
            for (c = 0; passed && c < big1N; c++) {
                unsigned int i;
                for (i = 0; passed && i < fib(c); i++) {
                    passed = zipped[z++] == c;
                }
            }
            if (!passed) {
                printf("output is wrong, expected 0x%02x, got 0x%02x at %d -- ", zipped[z-1], c, z);
            }
        }
        big1N = big1N == 16 ? 21
            : big1N == 21 ? 26
            : big1N == 26 ? 31
            : big1N == 31 ? 36
            : big1N == 36 ? 41
            : 41;
    }
    fclose(out);
    if (passed) {
        printf("PASSED\n");
        testN++;
        return 0;
    } else {
        printf("FAILED\n");
        testN++;
        return 1;
    }
}

static int feederBig2(void)
{
    FILE *const in = fopen("in.txt", "wb");
    int error = 0;
    if (!in) {
        error = 1;
    } else if (testN%2 == 0) { // ask to compress data
        int i;
        error = EOF == fprintf(in, "c\r\n");
        for (i = 0; !error && i < 256*256; i++) {
            const unsigned char c = i & 0xff;
            error |= fwrite(&c, 1, 1, in) < 1;
        }
    } else { // ask to decompress compressed data
        const unsigned char *c, *const lastZipped = zipped+lenZipped;
        error = EOF == fprintf(in, "d\r\n");
        for (c = zipped; !error && c != lastZipped; c++) {
            error |= fwrite(c, 1, 1, in) < 1;
        }
    }
    fclose(in);
    if (error) {
        printf("can't create in.txt. No space on disk?\n");
    }
    return error;
}

static int checkerBig2(void)
{
    FILE *const out = fopen("out.txt", "rb");
    int passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    if (testN%2 == 0) { // check the compression ratio is matched
        const size_t expectedSize = 4+(10*256+256*256*8+7)/8;
        lenZipped = fread(zipped, 1, expectedSize+1, out);
        if (lenZipped > expectedSize) {
            passed = 0;
            printf("output is too long -- ");
        }
    } else { // check that compress+decompress doesn't corrupt the data
        const size_t nOut = fread(zipped, 1, 256*256+1, out);
        if (nOut < 256*256) {
            passed = 0;
            printf("output is too short -- ");
        } else if (nOut > 256*256) {
            passed = 0;
            printf("output is too long -- ");
        } else {
            int i;
            for (i = 0; passed && i < 256*256; i++) {
                    passed = zipped[i] == (i & 0xff);
            }
            if (!passed) {
                printf("output is wrong, expected 0x%02x, got 0x%02x at %d -- ", zipped[i], i & 0xff, i);
            }
        }
    }
    fclose(out);
    if (passed) {
        printf("PASSED\n");
        testN++;
        return 0;
    } else {
        printf("FAILED\n");
        testN++;
        return 1;
    }
}

static int feederBig3(void)
{
    FILE *const in = fopen("in.txt", "wb");
    int error = 0;
    if (!in) {
        error = 1;
    } else if (testN%2 == 0) { // ask to compress data
        int i;
        error = EOF == fprintf(in, "c\r\n");
        for (i = 0; !error && i < 256*256*256; i++) {
            const char c = 'x';
            error |= fwrite(&c, 1, 1, in) < 1;
        }
    } else { // ask to decompress compressed data
        const unsigned char *c, *const lastZipped = zipped+lenZipped;
        error = EOF == fprintf(in, "d\r\n");
        for (c = zipped; !error && c != lastZipped; c++) {
            error |= fwrite(c, 1, 1, in) < 1;
        }
    }
    fclose(in);
    if (error) {
        printf("can't create in.txt. No space on disk?\n");
    }
    return error;
}

static int checkerBig3(void)
{
    FILE *const out = fopen("out.txt", "rb");
    int passed = 1;
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    if (testN%2 == 0) { // check the compression ratio is matched
        const size_t expectedSize = 4+(10*1+256*256*256+7)/8;
        lenZipped = fread(zipped, 1, expectedSize+1, out);
        if (lenZipped > expectedSize) {
            passed = 0;
            printf("output is too long -- ");
        }
    } else { // check that compress+decompress doesn't corrupt the data
        size_t lenOut = fread(zipped, 1, 256*256*256+1, out);
        if (lenOut < 256*256*256) {
            passed = 0;
            printf("output is too short -- ");
        } else if (lenOut > 256*256*256) {
            passed = 0;
            printf("output is too long -- ");
        } else {
            int i;
            for (i = 0; passed && i < 256*256*256; i++) {
                passed = zipped[i] == 'x';
            }
            if (!passed) {
                printf("output is wrong, expect 0x%02x, got 0x%02x at %d -- ", zipped[i], i & 0xff, i);
            }
        }
    }
    fclose(out);
    if (passed) {
        printf("PASSED\n");
        testN++;
        return 0;
    } else {
        printf("FAILED\n");
        testN++;
        return 1;
    }
}

const TLabTest LabTests[] = {
    {FeedFromArray, CheckFromArray}, // 1
    {FeedFromArray, CheckFromArray}, // 2
    {FeedFromArray, CheckFromArray}, // 3
    {FeedFromArray, CheckFromArray}, // 4
    {FeedFromArray, CheckFromArray}, // 5
    {FeedFromArray, CheckFromArray}, // 6
    {FeedFromArray, CheckFromArray}, // 7
    {FeedFromArray, CheckFromArray}, // 8
    {FeedFromArray, CheckFromArray}, // 9
    {FeedFromArray, CheckFromArray}, // 10
    {FeedFromArray, CheckFromArray}, // 11
    {FeedFromArray, CheckFromArray}, // 12
    {FeedFromArray, CheckFromArray}, // 13
    {FeedFromArray, CheckFromArray}, // 14
    {FeedFromArray, CheckFromArray}, // 15
    {FeedFromArray, CheckFromArray}, // 16
    {FeedFromArray, CheckFromArray}, // 17
    {FeedFromArray, CheckFromArray}, // 18
    {FeedFromArray, CheckFromArray}, // 19
    {FeedFromArray, CheckFromArray}, // 20
    {FeedFromArray, CheckFromArray}, // 21
    {FeedFromArray, CheckFromArray}, // 22
    {FeedFromArray, CheckFromArray}, // 23
    {FeedFromArray, CheckFromArray}, // 24
    {FeedFromArray, CheckFromArray}, // 25
    {FeedFromArray, CheckFromArray}, // 26
    {feederBig1, checkerBig1}, // 27 n=16
    {feederBig1, checkerBig1}, // 28 n=21
    {feederBig2, checkerBig2}, // 29
    {feederBig2, checkerBig2},
    {feederBig3, checkerBig3},
    {feederBig3, checkerBig3},
    {feederBig1, checkerBig1}, // n=26
    {feederBig1, checkerBig1}, // n=31
    {feederBig1, checkerBig1}, // n=36
    {feederBig1, checkerBig1}, // n=41
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
