#include "testLab.h"
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <time.h>

#define BUFF_SIZE 256
#define ALPHABET_SIZE 26
#define MAX_WORD_LENGTH 10000

static int testN = 0;
static int testTimeOut = CLOCKS_PER_SEC * 3;
static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;

static const struct {const char *const in; const char *const out1; int out2;} testInOut[] = {
    {"3\napple\nmilk\nbread\nap\n", "apple", 4},
    {"3\napple\nmilk\nbread\nre\n", "None", 4},
    {"8\nappearance\napplicable\nappreciate\napprentice\nappendices\napocalypse\napologetic\napolitical\napp\n", "appearance appendices applicable appreciate apprentice", 14},
    {"0\n", "None", 0},
    {"2\naa\naa\na\n", "aa", 1},
    {"2\naaaaaaa\na\na\n", "a aaaaaaa", 2},
    {"5\nred\nrose\ngreen\nyellow\npink\nr\n", "red rose", 7},
    {"3\naba\nabacaba\nabacabaaba\naba\n", "aba abacaba abacabaaba", 3},
    {"3\naba\nabacaba\nabacabaaba\nabac\n", "abacaba abacabaaba", 3},
    {"3\naba\nabacaba\nabacabaaba\nabacab\n", "abacaba abacabaaba", 3},
    {"8\nappearance\napplicable\nappreciate\napprentice\nappendices\napocalypse\napologetic\napolitical\nappr\n", "appreciate apprentice", 14},
    {"3\nphone\ntelephone\niphone\nphone\n", "phone", 4},
    {"10\nprecaution\npredicament\npresume\nprefix\nprejudice\nprearrange\npreconceive\npredestination\npreeminent\npreoccupied\npred\n", "predicament predestination", 13},
    {"10\nprecaution\npredicament\npresume\nprefix\nprejudice\nprearrange\npreconceive\npredestination\npreeminent\npreoccupied\nprez\n", "None", 13},
    {"10\nprecaution\npredicament\npresume\nprefix\nprejudice\nprearrange\npreconceive\npredestination\npreeminent\npreoccupied\npre\n", "precaution preconceive predicament predestination presume prefix prejudice prearrange preeminent preoccupied", 13},
    {"5\nbeautiful\ncolorful\nplayful\nthoughtful\nrespectful\nful\n", "None", 6},
    {"3\nintriguing\nnostalgic\noptimistic\nintriguingg\n", "None", 4},
    {"4\ndislike\ndisagree\ndisappear\ndisrupt\ndisa\n", "disagree disappear", 6},
    {"4\ndislike\ndisagree\ndisappear\ndisrupt\ndislike\n", "dislike", 6},
    {"4\ndislike\ndisagree\ndisappear\ndisrupt\ndi\n", "dislike disagree disappear disrupt", 6},
};

static bool Equal(char *const string, const char *const pattern) {
    size_t lenString = strlen(string);
    size_t lenPattern = strlen(pattern);
    if (lenString == 0) {
        return false;
    }
    if (string[lenString - 1] == ' ') {
        string[lenString - 1] = '\0';
        lenString--;
    }
    if (lenString != lenPattern) {
        return false;
    }
    if (strchr(pattern, ' ') != NULL) {
        const char *begin = pattern;
        char *spacePointer = strchr(begin, ' ');
        while (spacePointer != NULL) {
            char word[BUFF_SIZE] = "";
            strncpy(word, begin, spacePointer - begin);
            if (strstr(string, word) == NULL) {
                return false;
            }
            begin = spacePointer + 1;
            spacePointer = strchr(begin, ' ');
        }
        if (strstr(string, begin) == NULL) {
            return false;
        }
    } else {
        for (size_t i = 0; i < lenString; i++) {
            if (string[i] != pattern[i]) {
                return false;
            }
        }
    }
    return true;
}

static bool Check(const char *const ans_first, const int ans_second, FILE *stream) {
    bool passed = true;
    char buff[BUFF_SIZE] = "";
    int n;
    if (ScanChars(stream, BUFF_SIZE, buff) != Pass) {
        passed = false;
    } else {
        if (!Equal(buff, ans_first)) {
            passed = false;
            printf("wrong output 1 -- ");
        } else {
            if (ScanInt(stream, &n) != Pass) {
                passed = false;
            } else {
                if (ans_second != n) {
                    passed = false;
                    printf("wrong output 2 got %d, expected %d -- ", n, ans_second);
                }
            }
        }
    }
    return passed;
}

static bool GenerateSequence(FILE *out, int ind, int order, char *output) {
    bool res = true;
    if (order > 0) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            output[ind] = 'a' + i;
            res = GenerateSequence(out, ind + 1, order - 1, output);
        }
    }
    if (order == 0) {
        if (fprintf(out, "%s\n", output) < 0) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(out);
            res = false;
        }
    }
    return res;
}

static int FeedFromArray(void) {
    FILE *const in = fopen("in.txt", "w+");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    if (fprintf(in, "%s", testInOut[testN].in) < 0) {
        printf("can't create in.txt. No space on disk?\n");
        fclose(in);
        return -1;
    }
    fclose(in);
    return 0;
}

static int CheckFromArray(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    bool passed = Check(testInOut[testN].out1, testInOut[testN].out2, out);
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

static int FeederBig1(void) {
    FILE *const in = fopen("in.txt", "w+");
    printf("Creating Large test... ");
    clock_t start = clock();
    int n = 17576;
    int finalAmountVerts = 18278;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "%d\n", n);
    char output[500 + 1] = "";
    memset(output, 'a', sizeof(char) * 500);
    GenerateSequence(in, 0, 3, output);
    fprintf(in, "zzzz\n");
    clock_t end = clock();
    fclose(in);
    testTimeOut = end - start;
    printf("done in T=%ld seconds. Starting exe with timeout T+3 seconds... ", RoundUptoThousand(testTimeOut) / CLOCKS_PER_SEC);
    testTimeOut = testTimeOut + CLOCKS_PER_SEC * 3;
    size_t memoryForChars = (n * 500 * sizeof(char));
    size_t memoryForTreeNodes = finalAmountVerts * (sizeof(char *) + sizeof(int) * 2 + 2 * GetLabPointerSize());
    LabMemoryLimit = memoryForChars + memoryForTreeNodes + MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int CheckerBig1(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    bool passed = Check("None", 18279, out);
    if (passed) {
        passed = !HaveGarbageAtTheEnd(out);
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

static int FeederBig2(void) {
    FILE *const in = fopen("in.txt", "w+");
    printf("Creating Large test... ");
    clock_t start = clock();
    int n = MAX_WORD_LENGTH / 5 * ALPHABET_SIZE;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "%d\n", n + 1);
    for (int i = 0; i < ALPHABET_SIZE; i++) {
        char output[MAX_WORD_LENGTH / 5 + 1] = "";
        for (int j = 0; j < MAX_WORD_LENGTH / 5; j++) {
            output[j] = 'a' + i;
            if (fprintf(in, "%s\n", output) < 0) {
                printf("can't create in.txt. No space on disk?\n");
                return -1;
            }
        }
    }
    fprintf(in, "abacaba\naba");
    clock_t end = clock();
    fclose(in);
    testTimeOut = end - start;
    printf("done in T=%ld seconds. Starting exe with timeout T+3 seconds... ", RoundUptoThousand(testTimeOut) / CLOCKS_PER_SEC);
    testTimeOut = testTimeOut + CLOCKS_PER_SEC * 3;
    size_t memoryForRecursion = n * (sizeof(void *) * 3);
    size_t memoryForChars = n * sizeof(char);
    size_t memoryForTreeNodes = (n + 1) * (sizeof(char *) + sizeof(int) * 2 + 2 * GetLabPointerSize());
    LabMemoryLimit = memoryForChars + memoryForTreeNodes + memoryForRecursion + MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int CheckerBig2(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    bool passed = Check("abacaba", 52000 + 2, out);
    if (passed) {
        passed = !HaveGarbageAtTheEnd(out);
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

static int FeederBig3(void) {
    FILE *const in = fopen("in.txt", "w+");
    printf("Creating Large test... ");
    clock_t start = clock();
    int n = MAX_WORD_LENGTH;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "%d\n", n);
    char output[MAX_WORD_LENGTH + 1] = "";
    memset(output, 'b', sizeof(char) * n);
    fprintf(in, "%s\n", output);
    for (int i = n - 1; i > 0; i--) {
        output[i] = '\0';
        if (fprintf(in, "%s\n", output) < 0) {
            printf("can't create in.txt. No space on disk?\n");
            return -1;
        }
    }
    fprintf(in, "d\n");
    clock_t end = clock();
    fclose(in);
    testTimeOut = end - start;
    printf("done in T=%ld seconds. Starting exe with timeout T+3 seconds... ", RoundUptoThousand(testTimeOut) / CLOCKS_PER_SEC);
    testTimeOut = testTimeOut + CLOCKS_PER_SEC * 3;
    size_t memoryForChars = n * sizeof(char);
    size_t memoryForTreeNodes =  n * ALPHABET_SIZE * (sizeof(char *) + sizeof(int) * 2 + 2 * GetLabPointerSize());
    LabMemoryLimit = memoryForChars + memoryForTreeNodes + MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int CheckerBig3(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    bool passed = Check("None", MAX_WORD_LENGTH, out);
    if (passed) {
        passed = !HaveGarbageAtTheEnd(out);
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

static int FeederBig4(void) {
    FILE *const in = fopen("in.txt", "w+");
    printf("Creating Large test... ");
    clock_t start = clock();
    int n = MAX_WORD_LENGTH;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    char output[MAX_WORD_LENGTH + 1] = "";
    fprintf(in, "%d\n", n);
    for (int i = 0; i < n; i++) {
        output[i] = 'a';
        if (fprintf(in, "%s\n", output) < 0) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
    }
    fprintf(in, "b\n");
    clock_t end = clock();
    fclose(in);
    testTimeOut = end - start;
    printf("done in T=%ld seconds. Starting exe with timeout T+3 seconds... ", RoundUptoThousand(testTimeOut) / CLOCKS_PER_SEC);
    testTimeOut = testTimeOut + CLOCKS_PER_SEC * 3;
    size_t memoryForChars = n * sizeof(char);
    size_t memoryForTreeNodes =  n * (sizeof(char *) + sizeof(int) * 2 + 2 * GetLabPointerSize());
    LabMemoryLimit = memoryForChars + memoryForTreeNodes + MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int CheckerBig4(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    bool passed = Check("None", MAX_WORD_LENGTH, out);
    if (passed) {
        passed = !HaveGarbageAtTheEnd(out);
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
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeederBig1, CheckerBig1},
    {FeederBig2, CheckerBig2},
    {FeederBig3, CheckerBig3},
    {FeederBig4, CheckerBig4},
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return sizeof(LabTests)/sizeof(LabTests[0]);
}

const char *GetTesterName(void) {
    return "Lab 6-3 radix tree";
}

int GetTestTimeout(void) {
    return testTimeOut;
}

size_t GetTestMemoryLimit(void) {
    return LabMemoryLimit;
}
