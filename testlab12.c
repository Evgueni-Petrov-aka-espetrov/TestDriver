#include "testLab.h"
#include <stdio.h>

#define TESTS_COUNT 3
#define TIME_LIMIT 1000
#define MEMORY_LIMIT (1 << 10)

typedef struct IOStream IOStream;

static const struct {const char *const in; const char *const out;} testInOut[] = {
        {"..3.2.6..\n9..3.5..1\n..18.64..\n..81.29..\n7.......8\n..67.82..\n..26.95..\n8..2.3..9\n..5.1.3..", "483921657\n967345821\n251876493\n548132976\n729564138\n136798245\n372689514\n814253769\n695417382"},
        {"..18....5\n4...35.2.\n.3.7.....\n..6....4.\n.8..91..2\n...5.....\n....7....\n.9..23..1\n8.....9..", "261849735\n479635128\n538712496\n156287349\n784391652\n923564817\n315978264\n697423581\n842156973"},
        {"61..1.8.3\n.47.8.5..\n...9....6\n...3...14\n324.16.58\n56.498.72\n.365.9...\n....67..5\n7.....4..",  "Incorrect input data"}
};

struct IOStream {
    FILE* In;
    FILE* Out;
};

static size_t currentTest = 0;

IOStream stream = {NULL, NULL};

static int FeedFromArray(void){
    stream.In = fopen("in.txt", "w+");
    if(!stream.In){
        printf("Can't create the file. No space on disk ?");
        return -1;
    }
    if(fprintf(stream.In, "%s\n", testInOut[currentTest].in) < 0){
        printf("Can't create in.txt. No space on disk?\n");
        fclose(stream.In);
        return -1;
    }
    fclose(stream.In);
    return 0;
}

static int CheckFromArray(void){
    stream.Out = fopen("out.txt","r");
    if(!stream.Out){
        printf("Can't open out.txt");
        currentTest++;
        return -1;
    }
    char buf1[128] = {0};
    int ind = 0,passed = 1;
    while(fscanf(stream.Out,"%c",&buf1[ind]) != EOF){ind ++;}
    for(int i = 0; i < ind; i++){
        if(testInOut[currentTest].out[i] != buf1[i]){
            passed = 0;
            currentTest++;
            fclose(stream.Out);
            break;
        }
    }
    if(passed){
        passed = !HaveGarbageAtTheEnd(stream.Out);
    }
    fclose(stream.Out);
    if(passed){
        printf("PASSED\n");
        currentTest++;
        return 0;
    }
    else{
        printf("FAILED\n");
        currentTest++;
        return 1;
    }
}

const TLabTest labTests[] = {
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray}
};

TLabTest GetLabTest (int testInd){
    return labTests[testInd];
}

int GetTestCount(void){
    return TESTS_COUNT;
}

const char* GetTesterName(void){
    return "Lab 12 Sudoku solver";
}

int GetTestTimeout(void){
    return TIME_LIMIT;
}

size_t GetTestMemoryLimit(void){
    return MIN_PROCESS_RSS_BYTES + MEMORY_LIMIT;
}
