#include "testLab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int testN = 0;
static const struct {
    const char *const tree_in;
    const char *const in;
    const char *const out;
    const char *const tree_out;
} testInOut[] = {
        {"Это животное?\nСобака\n#\n#\nКнига\n#\n#\n", "да\nда\n", "Это животное? (да/нет): \nЭто Собака? (да/нет): The character is guessed\n", NULL},
        {"Это животное?\nСобака\n#\n#\nКнига\n#\n#\n", "да\nнет\nКот\nЭто кот?\nда\n", "Это животное? (да/нет): \nЭто Собака? (да/нет): Add new character\nКто это был? \nКак его можно отличить от Собака? \nОтвет на вопрос для Кот - да или нет? Character added successfully\n", "Это животное?\nЭто кот?\nКот\n#\n#\nСобака\n#\n#\nКнига\n#\n#\n"},
        {"Это животное?\nСобака\n#\n#\nКнига\n#\n#\n", "да\nнет\nЛев\nЭто хищник?\nда\n", "Это животное? (да/нет): \nЭто Собака? (да/нет): Add new character\nКто это был? \nКак его можно отличить от Собака? \nОтвет на вопрос для Лев - да или нет? Character added successfully\n", "Это животное?\nЭто хищник?\nЛев\n#\n#\nСобака\n#\n#\nКнига\n#\n#\n"},
        {"Это животное?\nСобака\n#\n#\nКнига\n#\n#\n", "maybe\n", "Это животное? (да/нет): \nОшибка: ответ должен быть 'да' или 'нет'!\n", NULL},
        {"Это реальный человек?\nЭто политик?\nПутин\n#\n#\nИлон Маск\n#\n#\nЭто вымышленный персонаж?\nГарри Поттер\n#\n#\nМистер Бист\n#\n#\n", "да\nда\nнет\nБайден\nОн президент?\nда\n", "Это реальный человек? (да/нет): \nЭто политик? (да/нет): \nЭто Путин? (да/нет): Add new character\nКто это был? \nКак его можно отличить от Путин? \nОтвет на вопрос для Байден - да или нет? Character added successfully\n", "Это реальный человек?\nЭто политик?\nОн президент?\nБайден\n#\n#\nПутин\n#\n#\nИлон Маск\n#\n#\nЭто вымышленный персонаж?\nГарри Поттер\n#\n#\nМистер Бист\n#\n#\n"},
        {"", "", "Ошибка: пустой файл или ошибка чтения!\n", NULL},
        {"Это животное?\nСобака\n#\n#\nКнига\n#\n#\n", "да\nнет\nКот\nЭто кот?\nmaybe\n", "Это животное? (да/нет): \nЭто Собака? (да/нет): Add new character\nКто это был? \nКак его можно отличить от Собака? \nОтвет на вопрос для Кот - да или нет? Ошибка: ответ должен быть 'да' или 'нет'!\n", NULL},
        {"Это животное?\nСобака\n#\n#\nКнига\n#\n#\n", "\n", "Это животное? (да/нет): \nОшибка: ответ должен быть 'да' или 'нет'!\n", NULL},
        {"", "", "Ошибка: пустой файл или ошибка чтения!\n", NULL},
        {"Кот\n#\n#\n", "да\n", "Это Кот? (да/нет): The character is guessed\n", NULL},
        {"Это животное?\nСобака\n#\n#\nКнига\n#\n#\n", "нет\nда\n", "Это животное? (да/нет): \nЭто Книга? (да/нет): The character is guessed\n", NULL},
        {"Это живое?\nЭто животное?\nСобака\n#\n#\nЧеловек\n#\n#\nЭто неживое?\nКнига\n#\n#\nКамень\n#\n#\n", "да\nда\nда\n", "Это живое? (да/нет): \nЭто животное? (да/нет): \nЭто Собака? (да/нет): The character is guessed\n", NULL},
        {"Это животное?\nСобака\n#\n#\nКнига\n#\n#\n", "да\nнет\nКот\nЭто кот?\nда\nнет\nнет\nРобот\nЭто робот?\nда\n", "Это животное? (да/нет): \nЭто Собака? (да/нет): Add new character\nКто это был? \nКак его можно отличить от Собака? \nОтвет на вопрос для Кот - да или нет? Character added successfully\nЭто животное? (да/нет): \nЭто Книга? (да/нет): Add new character\nКто это был? \nКак его можно отличить от Книга? \nОтвет на вопрос для Робот - да или нет? Character added successfully\n", "Это животное?\nЭто кот?\nКот\n#\n#\nСобака\n#\n#\nЭто робот?\nРобот\n#\n#\nКнига\n#\n#\n"},
        {"Это живое?\nЭто животное?\nСобака\n#\n#\nЧеловек\n#\n#\nЭто неживое?\nКнига\n#\n#\nКамень\n#\n#\n", "да\nнет\nда\n", "Это живое? (да/нет): \nЭто животное? (да/нет): \nЭто Человек? (да/нет): The character is guessed\n", NULL},
        {"Это животное?\nСобака\n#\n#\nКнига\n#\n#\n", "да\nнет\n\nЭто кот?\nда\n", "Это животное? (да/нет): \nЭто Собака? (да/нет): Add new character\nКто это был? \nОшибка: имя персонажа не может быть пустым!\n", NULL},
        {"Это живое?\nЭто животное?\nСобака\n#\n#\nЧеловек\n#\n#\nКнига\n#\n#\n", "да\ninvalid\n", "Это живое? (да/нет): \nЭто животное? (да/нет): \nОшибка: ответ должен быть 'да' или 'нет'!\n", NULL},
        {"Это животное?\nСобака\n#\n#\nКнига\n#\n#\n", "выход\n", "Это животное? (да/нет): \n", NULL},
        {"Это живое?\nЭто животное?\nМлекопитающее?\nСобака\n#\n#\nКот\n#\n#\nПтица?\nОрёл\n#\n#\nВоробей\n#\n#\nРастение?\nДерево\n#\n#\nКуст\n#\n#\nНеживое?\nКамень\n#\n#\nМеталл\n#\n#\n", "да\nда\nда\nда\n", "Это живое? (да/нет): \nЭто животное? (да/нет): \nМлекопитающее? (да/нет): \nЭто Собака? (да/нет): The character is guessed\n", NULL},
        {"Это очень длинный вопрос, который занимает ровно сто символов и проверяет пределы строки в программе?\nСобака\n#\n#\nКнига\n#\n#\n", "да\nда\n", "Это очень длинный вопрос, который занимает ровно сто символов и проверяет пределы строки в программе? (да/нет): \nЭто Собака? (да/нет): The character is guessed\n", NULL},
        {"Это животное?\nСобака\n#\n#\nКнига\n#\n#\n", "да\nнет\nКот\nЭто кот?\nда\nнет\nнет\nРучка\nЭто канцелярия?\nда\nвыход\n", "Это животное? (да/нет): \nЭто Собака? (да/нет): Add new character\nКто это был? \nКак его можно отличить от Собака? \nОтвет на вопрос для Кот - да или нет? Character added successfully\nЭто животное? (да/нет): \nЭто Книга? (да/нет): Add new character\nКто это был? \nКак его можно отличить от Книга? \nОтвет на вопрос для Ручка - да или нет? Character added successfully\n", "Это животное?\nЭто кот?\nКот\n#\n#\nСобака\n#\n#\nЭто канцелярия?\nРучка\n#\n#\nКнига\n#\n#\n"},
        {"Это животное с длинным названием, которое занимает много места и проверяет обработку длинных строк в программе?\nСобака с очень длинным именем, которое должно быть корректно обработано программой\n#\n#\nКнига с длинным названием, которое также тестирует обработку строк\n#\n#\n", "да\nнет\nНовый персонаж с очень длинным именем для проверки буфера\nЭто очень длинный вопрос, который должен быть корректно обработан программой, чтобы проверить, как она справляется с длинными строками ввода?\nда\n", "Это животное с длинным названием, которое занимает много места и проверяет обработку длинных строк в программе? (да/нет): \nЭто Собака с очень длинным именем, которое должно быть корректно обработано программой? (да/нет): Add new character\nКто это был? \nКак его можно отличить от Собака с очень длинным именем, которое должно быть корректно обработано программой? \nОтвет на вопрос для Новый персонаж с очень длинным именем для проверки буфера - да или нет? Character added successfully\n", "Это животное с длинным названием, которое занимает много места и проверяет обработку длинных строк в программе?\nЭто очень длинный вопрос, который должен быть корректно обработан программой, чтобы проверить, как она справляется с длинными строками ввода?\nНовый персонаж с очень длинным именем для проверки буфера\n#\n#\nСобака с очень длинным именем, которое должно быть корректно обработано программой\n#\n#\nКнига с длинным названием, которое также тестирует обработку строк\n#\n#\n"},
        {NULL, NULL, NULL, NULL},
};

static int FeedFromArray(void) {
    if (testInOut[testN].tree_in != NULL) {
        FILE *const tree = fopen("tree.txt", "w");
        if (!tree) return -1;
        if (fprintf(tree, "%s", testInOut[testN].tree_in) < 0) {
            fclose(tree);
            return -1;
        }
        fclose(tree);
    }

    FILE *const in = fopen("in.txt", "w");
    if (!in) return -1;
    if (fprintf(in, "%s", testInOut[testN].in) < 0) {
        fclose(in);
        return -1;
    }
    fclose(in);
    return 0;
}

static int CheckFromArray(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        testN++;
        return -1;
    }

    char buf[1024] = {0};
    size_t bytesRead = 0;
    int c;
    while (bytesRead < sizeof(buf) - 1 && !feof(out)) {
        c = fgetc(out);
        if (c != EOF) buf[bytesRead++] = (char)c;
    }
    buf[bytesRead] = '\0';
    fclose(out);

    const char* status = Pass;
    if (_strnicmp(testInOut[testN].out, buf, strlen(testInOut[testN].out)) != 0) status = Fail;

    if (status == Pass && testInOut[testN].tree_out) {
        FILE *const tree = fopen("tree.txt", "r");
        if (!tree) {
            testN++;
            return -1;
        }
        char treeBuf[1024] = {0};
        bytesRead = 0;
        while (bytesRead < sizeof(treeBuf) - 1 && !feof(tree)) {
            c = fgetc(tree);
            if (c != EOF) treeBuf[bytesRead++] = (char)c;
        }
        treeBuf[bytesRead] = '\0';
        fclose(tree);

        if (_strnicmp(testInOut[testN].tree_out, treeBuf, strlen(testInOut[testN].tree_out)) != 0) status = Fail;
    }

    if (status == Pass && HaveGarbageAtTheEnd(out)) status = Fail;

    printf("%s\n", status == Pass ? "PASSED" : "FAILED");
    testN++;
    return status == Fail;
}

static int FeederBig(void) {
    FILE *const tree = fopen("tree.txt", "w");
    if (!tree) {
        printf("can't create tree.txt. No space on disk?\n");
        return -1;
    }

    int max_depth = 10;
    for (int level = 0; level < max_depth - 1; level++) {
        char question[100];
        int level_value = (1 << level) - 1;
        snprintf(question, sizeof(question), "Вопрос на уровне %d?\n", level_value);
        fprintf(tree, "%s", question);
        fprintf(tree, "#\n");
    }
    char character[100];
    int last_level_value = (1 << (max_depth - 1)) - 1;
    snprintf(character, sizeof(character), "Персонаж %d\n", last_level_value);
    fprintf(tree, "%s", character);
    fprintf(tree, "#\n#\n");

    fclose(tree);

    FILE *const in = fopen("in.txt", "w");
    if (!in) {
        printf("can't create in.txt. No space on disk?\n");
        fclose(in);
        return -1;
    }
    for (int i = 0; i < max_depth - 1; i++) {
        fprintf(in, "нет\n");
    }
    fprintf(in, "да\n");
    fclose(in);
    return 0;
}

static int CheckerBig(void) {
    FILE *const out = fopen("out.txt", "r");
    if (!out) {
        printf("can't open out.txt\n");
        testN++;
        return -1;
    }

    char buf[4096] = {0};
    size_t bytesRead = 0;
    int c;
    while (bytesRead < sizeof(buf) - 1 && !feof(out)) {
        c = fgetc(out);
        if (c != EOF) {
            buf[bytesRead] = (char)c;
            bytesRead++;
        }
    }
    buf[bytesRead] = '\0';
    fclose(out);

    int max_depth = 10;
    char expected[4096] = {0};
    for (int level = 0; level < max_depth - 1; level++) {
        char question[100];
        snprintf(question, sizeof(question), "Вопрос на уровне %d? (да/нет): \n", (1 << level) - 1);
        strcat(expected, question);
    }

    char last_question[100];
    snprintf(last_question, sizeof(last_question), "Это Персонаж %d? (да/нет): The character is guessed\n", (1 << (max_depth - 1)) - 1);
    strcat(expected, last_question);

    const char* status = Pass;
    if (_strnicmp(expected, buf, strlen(expected)) != 0) {
        printf("Test %d failed: Expected: %s\nGot: %s\n", testN + 1, expected, buf);
        status = Fail;
    }

    if (status == Pass && HaveGarbageAtTheEnd(out)) {
        printf("Test %d failed: Garbage at the end of output\n", testN + 1);
        status = Fail;
    }

    printf("%s\n", status);
    testN++;
    return status == Fail;
}

static size_t LabMemoryLimit = MIN_PROCESS_RSS_BYTES;

static const TLabTest LabTests[] = {
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
        {FeederBig, CheckerBig},
};

TLabTest GetLabTest(int testIdx) {
    return LabTests[testIdx];
}

int GetTestCount(void) {
    return sizeof(LabTests) / sizeof(LabTests[0]);
}

const char* GetTesterName(void) {
    return "Akinator Lab Tester";
}

int GetTestTimeout(void) {
    return 6000;
}

size_t GetTestMemoryLimit(void) {
    return LabMemoryLimit;
}
