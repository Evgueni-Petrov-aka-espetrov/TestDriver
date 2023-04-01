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

static bool equal(char *const string, const char *const pattern) {
    int len_string = (int) strlen(string);
    int len_pattern = (int) strlen(pattern);
    if (string[len_string - 1] == ' ') {
        string[len_string - 1] = '\0';
        len_string--;
    }
    if (len_string != len_pattern) {
        return false;
    }
    if (strchr(pattern, ' ') != NULL) {
        char *begin = (char *) pattern;
        char *space_pointer = strchr(begin, ' ');
        while (space_pointer != NULL) {
            char word[BUFF_SIZE] = "";
            strncpy(word, begin, space_pointer - begin);
            if (strstr(string, word) == NULL) {
                return false;
            }
            begin = space_pointer + 1;
            space_pointer = strchr(begin, ' ');
        }
        if (strstr(string, begin) == NULL) {
            return false;
        }
    } else {
        for (int i = 0; i < len_string; i++) {
            if (string[i] != pattern[i]) {
                return false;
            }
        }
    }
    return true;
}

static bool check(const char *const ans_first, const int ans_second, FILE *stream) {
    bool passed = true;
    char buff[BUFF_SIZE] = "";
    int n;
    if (ScanChars(stream, BUFF_SIZE, buff) != Pass) {
        passed = false;
    } else {
        if (!equal(buff, ans_first)) {
            passed = false;
            printf("wrong output 1 -- ");
        } else {
            if (ScanInt(stream, &n) != Pass) {
                passed = false;
            } else {
                if (ans_second != n) {
                    passed = false;
                    printf("wrong output 2 -- ");
                }
            }
        }
    }
    return passed;
}

static bool generate(FILE *out, int ind, int order, char *output) {
    bool res = true;
    if (order > 0) {
        for (int i = 0; i < ALPHABET_SIZE; i++) {
            output[ind] = (char) ('a' + i);
            res = generate(out, ind + 1, order - 1, output);
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
    bool passed = check(testInOut[testN].out1, testInOut[testN].out2, out);
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

static int feederBig1(void) {
    FILE *const in = fopen("in.txt", "w+");
    printf("Creating Large test... ");
    clock_t start = clock();
    int n = 17576;
    int final_amount_verts = 18278;
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        return -1;
    }
    fprintf(in, "%d\n", n);
    char output[500] = "";
    memset(output, 'a', sizeof(char) * 500);
    generate(in, 0, 3, output);
    fprintf(in, "zzzz\n");
    clock_t end = clock();
    fclose(in);
    testTimeOut = end - start;
    printf("done in T=%d seconds. Starting exe with timeout T+3 seconds... ", (int) (RoundUptoThousand(testTimeOut) / CLOCKS_PER_SEC));
    testTimeOut = testTimeOut + CLOCKS_PER_SEC * 3;
    LabMemoryLimit = (n * 500 * sizeof(char)) + final_amount_verts * (sizeof(char *) + sizeof(int) * 2 + 2 * GetLabPointerSize()) + MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int checkerBig1(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    bool passed = check("None", 18279, out);
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

static int feederBig2(void) {
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
            output[j] = (char) ('a' + i);
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
    printf("done in T=%d seconds. Starting exe with timeout T+3 seconds... ", (int) (RoundUptoThousand(testTimeOut) / CLOCKS_PER_SEC));
    testTimeOut = testTimeOut + CLOCKS_PER_SEC * 3;
    LabMemoryLimit = (n * sizeof(char)) + (n + 1) * (sizeof(char *) + sizeof(int) * 2 + 2 * GetLabPointerSize()) + MIN_PROCESS_RSS_BYTES * 2;
    return 0;
}

static int checkerBig2(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    bool passed = check("abacaba", 52000 + 2, out);
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

static int feederBig3(void) {
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
    printf("done in T=%d seconds. Starting exe with timeout T+3 seconds... ", (int) (RoundUptoThousand(testTimeOut) / CLOCKS_PER_SEC));
    testTimeOut = testTimeOut + CLOCKS_PER_SEC * 3;
    LabMemoryLimit = (n * sizeof(char)) + n * ALPHABET_SIZE * (sizeof(char *) + sizeof(int) * 2 + 2 * GetLabPointerSize()) + MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int checkerBig3(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    bool passed = check("None", MAX_WORD_LENGTH, out);
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

static int feederBig4(void) {
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
    printf("done in T=%d seconds. Starting exe with timeout T+3 seconds... ", (int) (RoundUptoThousand(testTimeOut) / CLOCKS_PER_SEC));
    testTimeOut = testTimeOut + CLOCKS_PER_SEC * 3;
    LabMemoryLimit = (n * sizeof(char)) + n * (sizeof(char *) + sizeof(int) * 2 + 2 * GetLabPointerSize()) + MIN_PROCESS_RSS_BYTES;
    return 0;
}

static int checkerBig4(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    bool passed = check("None", MAX_WORD_LENGTH, out);
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
    {feederBig1, checkerBig1},
    {feederBig2, checkerBig2},
    {feederBig3, checkerBig3},
    {feederBig4, checkerBig4},
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
