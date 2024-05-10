#include "testLab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int testN = 0;
static const struct {const char *const in; int out;} testInOut[] = {
    // Possible errors
    {"0\n\n", 0},
    {"1\n25\n", 1},
    {"3\n-1 -1 2\n", 2},
    {"3\n-1 2 -1\n", 2},
        
    // Small rotates variations
    {"5\n3 5 8 9 10", 3},
    {"5\n15 9 8 5 2", 3},
    {"5\n3 5 8 1 0", 3},
    {"5\n15 9 2 32 168", 3},

    // Pairs of small + big rotates variations
    {"5\n1 2 3 5 4", 3},
    {"5\n1 2 5 3 4", 3},
    {"5\n3 2 1 5 4", 3},
    {"5\n5 2 1 3 4", 3},

    // 2 small rotates and 2 big rotates in different order (start from small)
    {"10\n12 32 168 9 2 200 170 1 3 4", 4},
    {"8\n12 32 168 9 2 22 30 23", 4},
    {"9\n12 32 168 64 200 48 11 10 22", 4},
    {"10\n12 32 168 64 200 48 11 13 22 10", 4},
    {"9\n12 32 168 22 10 15 9 8 11", 4},
    {"10\n13 32 168 22 10 15 9 11 12 8", 4},
    {"10\n12 9 2 32 168 22 10 3 1 4", 4},
    {"11\n12 9 2 32 168 1 22 10 23 15 17", 4},

    // 2 small rotates and 2 big rotates in different order (start from big)
    {"8\n25 52 39 13 9 35 64 259", 4},
    {"9\n25 52 39 -5 38 30 67 -39 -18", 4},
    {"8\n18 39 20 -25 10 -30 105 110", 4},
    {"9\n52 25 39 87 99 14 0 112 105", 4},
    {"7\n52 25 39 195 207 75 -8", 3},
    {"9\n52 25 39 5 0 159 201 64 55", 4},
    {"9\n17 10 13 4 -8 28 20 45 52", 4},
    {"9\n5 20 10 30 40 0 3 -2 -5", 4},
};

static int FeedFromArray(void)
{
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

static int CheckFromArray(void)
{
  FILE *const out = fopen("out.txt", "r");
  int passed = 1;
  if (!out) {
    printf("can't open out.txt\n");
    testN++;
    return -1;
  }
  {
    int n;
    if (ScanInt(out, &n) != Pass) {
      passed = 0;
    } else if (testInOut[testN].n != n) {
      passed = 0;
      printf("wrong output -- ");
    }
  }
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

static size_t LabMemoryLimit;

static int feederBig(void)
{
  FILE *const in = fopen("in.txt", "w+");
  int i;
  if (!in) {
    printf("can't create in.txt. No space on disk?\n");
    return -1;
  }
  fprintf(in, "2000000\n");
  for (i = 0; i < 2000000; i++) {
    if (fprintf(in, "%d ", i) < 0) {
      printf("can't create in.txt. No space on disk?\n");
      fclose(in);
      return -1;
    }
  }
  fprintf(in, "\n");
  fclose(in);
  LabMemoryLimit = 2000000*(sizeof(int)+sizeof(int)+2*GetLabPointerSize())+MIN_PROCESS_RSS_BYTES;
  return 0;
}

static int checkerBig(void)
{
  FILE *const out = fopen("out.txt", "r");
  int passed = 1;
  if (!out) {
    printf("can't open out.txt\n");
    testN++;
    return -1;
  }
  {
    int n;
    if (ScanInt(out, &n) != Pass) {
      passed = 0;
    } else if (21 != n) {
      passed = 0;
      printf("wrong output -- ");
    }
  }
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

static int feederBig1(void)
{
  FILE *const in = fopen("in.txt", "w+");
  unsigned i;
  if (!in) {
    printf("can't create in.txt. No space on disk?\n");
    return -1;
  }
  fprintf(in, "2000000\n");
  for (i = 0; i < 2000000; i++) {
    if (fprintf(in, "%d ", i^0xcafecafe) < 0) {
      printf("can't create in.txt. No space on disk?\n");
      fclose(in);
      return -1;
    }
  }
  fprintf(in, "\n");
  fclose(in);
  LabMemoryLimit = 2000000*(sizeof(int)+sizeof(int)+2*GetLabPointerSize())+MIN_PROCESS_RSS_BYTES;
  return 0;
}

static int checkerBig1(void)
{
  FILE *const out = fopen("out.txt", "r");
  int passed = 1;
  if (!out) {
    printf("can't open out.txt\n");
    testN++;
    return -1;
  }
  {
    int n;
    if (ScanInt(out, &n) != Pass) {
      passed = 0;
    } else if (23 != n) {
      passed = 0;
      printf("wrong output -- ");
    }
  }
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

static void genTree(FILE *in, int height, int min, int max)
{
  int mid = (min*21+max*34)/55;
  if (height == 0) {
    return;
  }
  if (height == 1) {
    if (fprintf(in, "%d ", min) < 0) {
      printf("can't create in.txt. No space on disk?\n");
      fclose(in);
      exit(1);
    }
    return;
  }
  if (height == 2) {
    if (min == max) {
      printf("Internal error: generation failed, h = %d, min = %d, max = %d\n", height, min, max);
      fclose(in);
      exit(1);
    }
    if (fprintf(in, "%d %d ", max, min) < 0) {
      printf("can't create in.txt. No space on disk?\n");
      fclose(in);
      exit(1);
    }
    return;
  }
  if (mid <= min || max <= mid) {
    printf("Internal error: generation failed, h = %d, min = %d, max = %d\n", height, min, max);
    fclose(in);
    exit(1);
  }
  genTree(in, height-1, min, mid-1);
  genTree(in, height-2, mid+1, max);
  if (fprintf(in, "%d ", mid) < 0) {
    printf("can't create in.txt. No space on disk?\n");
    fclose(in);
    exit(1);
  }
}

static int feederBig2(void)
{
  FILE *const in = fopen("in.txt", "w+");
  if (!in) {
    printf("can't create in.txt. No space on disk?\n");
    return -1;
  }
  fprintf(in, "1346269\n");
  genTree(in, 30, 0, 4000000);
  fprintf(in, "\n");
  fclose(in);
  LabMemoryLimit = 1346269*(sizeof(int)+sizeof(int)+2*GetLabPointerSize())+MIN_PROCESS_RSS_BYTES;
  return 0;
}

static int checkerBig2(void)
{
  FILE *const out = fopen("out.txt", "r");
  int passed = 1;
  if (!out) {
    printf("can't open out.txt\n");
    testN++;
    return -1;
  }
  {
    int n;
    if (ScanInt(out, &n) != Pass) {
      passed = 0;
    } else if (29 != n) {
      passed = 0;
      printf("wrong output -- ");
    }
  }
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
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {FeedFromArray, CheckFromArray},
    {feederBig, checkerBig},
    {feederBig1, checkerBig1},
    {feederBig2, checkerBig2},
};

TLabTest GetLabTest(int testIdx) {
  return LabTests[testIdx];
}

int GetTestCount(void) {
  return sizeof(LabTests)/sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
  return "Lab 6-0 AVL trees";
}

int GetTestTimeout(void) {
  return 6000;
}

static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;
size_t GetTestMemoryLimit(void) {
  return LabMemoryLimit;
}
