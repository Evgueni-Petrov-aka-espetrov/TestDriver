#include "testLab.h"
#include <stdio.h>

#define TESTS_COUNT 20
#define TIME_LIMIT 1000
#define MEMORY_LIMIT (1 << 10)

typedef struct IOStream IOStream;

static const struct {const char *const in; const char *const out;} testInOut[] = {
        {"483921657\n967345821\n251876493\n548132976\n729564138\n136798245\n372689514\n814253769\n69541738.", "483921657\n967345821\n251876493\n548132976\n729564138\n136798245\n372689514\n814253769\n695417382"},
        {"261849735\n479635128\n538712496\n156287349\n784391652\n923564817\n315978264\n697423581\n842156...", "261849735\n479635128\n538712496\n156287349\n784391652\n923564817\n315978264\n697423581\n842156973"},
        {"483921657\n967345821\n251876493\n.........\n729564138\n136798245\n372689514\n695417382\n814253769", "483921657\n967345821\n251876493\n548132976\n729564138\n136798245\n372689514\n695417382\n814253769"},
        {"8613.7294\n5974.2361\n4326.9785\n9162.5843\n3589.4127\n2741.8956\n7895.1632\n1438.6579\n6257.3418", "861357294\n597482361\n432619785\n916275843\n358964127\n274138956\n789541632\n143826579\n625793418"},
        {"483921657\n967345821\n251876493\n548132976\n729564138\n136798245\n372689514\n814253769\n69541738a", "Incorrect input data"}, //последний символ - а
        {"261849735\n479635128\n538712496\n156287349\n784391652\n923564817\n315978264\n697423581\n842156973", "This sudoku already solved"},
        {"261849735\n479635128\n538712496\n15628b349\n784391652\n923564817\n315978264\n697423581\n842156973", "Incorrect input data"}, // четвёртая строка - b
        {"..3.2.6..\n9..3.5..1\n..18.64..\n..81.29..\n7.......8\n..67.82..\n..26.95..\n8..2.3..9\n..5.1.3..\n........6", "Incorrect input data"}, // есть 10ая строка
        {"..3.2.6..\n9..3.5..1\n..18.64..\n..81.29..\n7.....8\n..67.82..\n..26.95..\n8..2.3..9\n..5.1.3..", "Incorrect input data"}, // недостаток символов в 5ой строке
        {"48.3.....\n..1....71\n.2.......\n7.5....6.\n...2..8..\n.........\n....76...\n3.....4..\n....5....", "Incorrect input data"}, // повторение числа в строке
       {".85.2....\n9..3..54.\n........6\n..2.3.61.\n1.......7\n...6....9\n4..5..13.\n..9..4...\n.......7.\n\n\ns", "Incorrect input data"}, // 3 \n
       {".85.2....\n9..3..54.\n........6\n..2.3.61.\n1.......7\n...6....9\n4..5..13.\n..9..4...\n.......7.\t\t\ts", "Incorrect input data"}, // 3\t
       {".85.2....\n9..3..54.\n........6\n..2.3.61.\n1.......7\n...6....9\n4..5..13.\n..9..4...\n.......7.   s", "Incorrect input data"}, // 3 пробела
       {"..6......\n87..1...6\n...54...9\n6...813.4\n\n...3...5.\n.....7...\n.........\n.9.4....8\n..5.6.1..", "Incorrect input data"}, // лишний \n
       {"..6......\n87..1...6\n...54...9\n6...813.4\n...3...5.\n.....7...\n.........\n.9.4....8\n..5.6.1..", "546879213\n879213546\n213546879\n657981324\n981324657\n324657981\n768192435\n192435768\n435768192"}, //тяжёлое судоку с 22 заполненными ячейками
       {"48.3.....\n..1....7.\n.2.......\n7.5....6.\n...2..8..\n.........\n..1.76...\n3.....4..\n....5....", "Incorrect input data"}, // повторение числа в столбце
       {"48.3....7\n..1....7.\n.2.......\n7.5....6.\n...2..8..\n.........\n....76...\n3.....4..\n....5....", "Incorrect input data"}, // повторение числа в квадранте
       {"9....673.\n...94...2\n.24......\n.65....83\n..7.8.5..\n89....41.\n......97.\n7...21...\n.867....5", "958216734\n371945862\n624837159\n465179283\n217483596\n893652417\n542368971\n739521648\n186794325"}, // судоку с 29 заполненными ячейками(тяжёлое)
       {"..2..41..\n9.7..84..\n..1...9.6\n.3..5...9\n.1..6...4\n.9..4.8.7\n..3....2.\n4..7...9.\n5..31....", "852694173\n967138452\n341572986\n734851269\n218967534\n695243817\n173489625\n486725391\n529316748"}, //лёгкое судоку с 28 заполненными клетами
       {"...5....6\n..948.7..\n..8..2...\n....24..5\n.319.562.\n4..16....\n...6..1..\n..6.472..\n5....1...", "273519486\n159486732\n648732591\n967324815\n831975624\n425168973\n792653148\n316847259\n584291367"} //сложное с 28 заполненными клетами
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
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
        {FeedFromArray,CheckFromArray},
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
