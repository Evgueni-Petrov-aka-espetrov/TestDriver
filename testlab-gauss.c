#include "testLab.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

static int testN = 0;
static const struct {const char* const in, *const out;} testInOut[] = {
    // bad input
    {"\n", "bad input"},
    {"0 2", "bad input"},
    {"1 2", "bad input"},
    {"1000000000 2", "bad input"}, // не уверен, что идея лабороторной как-то связанна с проверкой N
    // small mod (uint8_t)
    {"1 2\n1", "1"},
    {"2 2\n1 0\n0 1", "1"},
    {"2 2\n0 0\n0 0", "0"},
    {"2 2\n1 1\n1 1", "0"},
    {"2 2\n1 1\n0 0", "0"},
    {"2 2\n0 1\n1 0", "1"},
    {"2 3\n0 1\n1 0", "2"},
    {"3 11\n1 2 3\n4 5 6\n7 8 9", "0"},
    {"3 2\n1 0 1\n0 1 0\n1 0 1", "0"},
    {"3 229\n100 101 0\n228 1 227\n156 5 138", "203"},
    {"3 251\n111 117 70\n95 212 202\n5 116 238", "4"},
    // mid mod (uint16_t)
    {"1 48611\n10", "10"},
    {"2 7919\n1 0\n0 1", "1"},
    {"2 4973\n0 0\n0 0", "0"},
    {"2 5903\n0 1\n1 0", "5902"},
    {"2 30911\n1 1\n1 1", "0"},
    {"2 17627\n1 1\n1 0", "17626"},
    {"3 60013\n1 2 3\n4 5 6\n7 8 9", "0"},
    {"3 65521\n36077 65346 8810\n35619 7729 62614\n6647 11922 16501", "20278"},
    // big mod (uint32_t)
    {"1 79357\n10", "10"},
    {"2 2038074743\n1 0\n0 1", "1"},
    {"2 3188776067\n0 0\n0 0", "0"},
    {"2 4294967291\n0 1\n1 0", "4294967290"},
    {"2 685957033\n1 1\n1 1", "0"},
    {"2 2127107077\n1 1\n1 0", "2127107076"},
    {"3 4259878741\n1 2 3\n4 5 6\n7 8 9", "0"},
    {"3 2300618137\n1939394573 566833575 435023098\n1592112954 1638375196 2289248927\n279481876 561856566 1649552576", "889189530"}
};

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
    FILE* const out = fopen("out.txt", "r");
    if (out == NULL) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    char buf[128] = {0};
    const char* status = ScanChars(out, sizeof(buf), buf);
    if (status == Pass && _strnicmp(testInOut[testN].out, buf, strlen(testInOut[testN].out)) != 0) {
        status = Fail;
    }
    if (status == Pass && HaveGarbageAtTheEnd(out)) {
        status = Fail;
    }
    fclose(out);
    printf("%s\n", status);
    ++testN;
    return status == Fail;
}

static uint32_t __seed;
static inline uint32_t s_rand(void) {
    uint32_t x = __seed;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    __seed = x;
    return x * 0x9E3779BBu;
}

static int feederBig(uint32_t seed, int n, uint32_t mod) {
    FILE *const in = fopen("in.txt", "w+");
    if (!in) {
      printf("can't create in.txt. No space on disk?\n");
      return -1;
    }
    __seed = seed;
    fprintf(in, "%d %u\n", n, mod);
    for (int i = 0; i < n; i++) {
        for(int j = 0; j < n; j++)
            if (fprintf(in, "%u ", s_rand() % mod) < 0) {
                printf("can't create in.txt. No space on disk?\n");
                fclose(in);
                return -1;
            }
        fprintf(in, "\n");  
    }
    fclose(in);
    return 0;
}

static const char* ScanU32(FILE* out, uint32_t* a) {
    int status = fscanf(out, "%u", a);
    if (status < 0) {
        puts("output too short -- ");
        return Fail;
    } else if (status < 1) {
        puts("bad output format -- ");
        return Fail;
    }
    return Pass;
}
static int checkerBig(uint32_t det_ans) {
    FILE* const out = fopen("out.txt", "r");
    if (out == NULL) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }
    uint32_t det;
    const char* status = ScanU32(out, &det);
    if (status == Pass && det != det_ans)
        status = Fail;
    if (status == Pass && HaveGarbageAtTheEnd(out))
        status = Fail;
    fclose(out);
    printf("%s\n", status);
    ++testN;
    return status == Fail;
}

static size_t LabMemoryLimit;
static int LabTimeout;

static int feederBig1(void) {
    LabTimeout = 2000;
    LabMemoryLimit = 1000 * 1000 * sizeof(uint8_t) + MIN_PROCESS_RSS_BYTES;
    return feederBig(666u, 1000, 2u);
}
static int checkerBig1(void) {
    return checkerBig(0);
}

static int feederBig2(void) {
    LabTimeout = 3000;
    LabMemoryLimit = 2000 * 2000 * sizeof(uint8_t) + MIN_PROCESS_RSS_BYTES;
    return feederBig(123u, 2000, 229u);
}
static int checkerBig2(void) {
    return checkerBig(89u);
}

static int feederBig3(void) {
    LabTimeout = 4000;
    LabMemoryLimit = 1500 * 1500 * sizeof(uint16_t) + MIN_PROCESS_RSS_BYTES;
    return feederBig(777u, 1500, 59359u);
}
static int checkerBig3(void) {
    return checkerBig(44457u);
}

static int feederBig4(void) {
    LabTimeout = 2000;
    LabMemoryLimit = 1000 * 1000 * sizeof(uint32_t) + MIN_PROCESS_RSS_BYTES;
    return feederBig(99999999u, 1000, 4294967291u);
}
static int checkerBig4(void) {
    return checkerBig(1160432611u);
}

static int feederBig5(void) { // первый и второй столбец линейно зависимы и полностью считать опеределитель не надо, основное время - чтение матрицы
    LabTimeout = 1500;
    LabMemoryLimit = 5000 * 5000 * sizeof(uint32_t) + MIN_PROCESS_RSS_BYTES;

    __seed = 784538u;
    int n = 5000;
    uint32_t mod = 1986141617u;

    FILE *const in = fopen("in.txt", "w+");
    if (!in) {
      printf("can't create in.txt. No space on disk?\n");
      return -1;
    }
    
    uint32_t first_col[n];
    uint32_t second_col[n];

    for (int i = 0; i < n; i++) {
        uint32_t num = s_rand() % mod;
        first_col[i] = num;
        second_col[i] = num << 1; // делим на 2
    }

    fprintf(in, "%d %u\n", n, mod);
    for (int i = 0; i < n; i++) {
        if (fprintf(in, "%u ", first_col[i]) < 0) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
        if (fprintf(in, "%u ", second_col[i]) < 0) {
            printf("can't create in.txt. No space on disk?\n");
            fclose(in);
            return -1;
        }
        for(int j = 2; j < n; j++)
            if (fprintf(in, "%u ", s_rand() % mod) < 0) {
                printf("can't create in.txt. No space on disk?\n");
                fclose(in);
                return -1;
            }
        fprintf(in, "\n");  
    }
    fclose(in);
    return 0;
}

static int checkerBig5(void) {
    return checkerBig(0u);
}

static int feederBigBig(void) {
    LabTimeout = 8000;
    LabMemoryLimit = 3000 * 3000 * sizeof(uint8_t) + MIN_PROCESS_RSS_BYTES;
    return feederBig(3465077311u, 3000, 251u);
}
static int checkerBigBig(void) {
    return checkerBig(43u);
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
    {FeedFromArray, CheckFromArray}, // 27
    {FeedFromArray, CheckFromArray}, // 28
    {FeedFromArray, CheckFromArray}, // 29
    {FeedFromArray, CheckFromArray}, // 30
    {FeedFromArray, CheckFromArray}, // 31
    {feederBig1, checkerBig1},       // 32
    {feederBig2, checkerBig2},       // 33
    {feederBig3, checkerBig3},       // 34
    {feederBig4, checkerBig4},       // 35
    {feederBig5, checkerBig5},       // 36
    {feederBigBig, checkerBigBig},   // 37
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return sizeof(LabTests)/sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
    /////////////////////////////
    // Неофициальное название: //
    //      lab-churkin        //
    /////////////////////////////
    return "Lab Gauss Det % mod";
}

static int LabTimeout = 1000;
int GetTestTimeout(void) {
    return LabTimeout;
}

static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;
size_t GetTestMemoryLimit(void) {
    return LabMemoryLimit;
}