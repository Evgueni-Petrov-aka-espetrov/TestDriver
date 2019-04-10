#include "testLab.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#if defined __linux__
#include <sys/resource.h>
#endif

static int labDo(char *labExe);
static int labUserOutOfMemory = -1;

int main(int argc, char* argv[])
{
    int i;
    printf("\nKOI FIT NSU Lab Tester (c) 2009-2014 by Evgueni Petrov\n");

    if (argc < 2) {
        printf("\nUser error: to test mylab.exe do %s mylab.exe\n", argv[0]);
        return 1;
    } if (argc == 4 && strcmp(argv[1],"-m") == 0 && atoi(argv[2]) != 0) {
        labUserOutOfMemory = atoi(argv[2])*1024;
        argv += 2;
    }

    printf("\nTesting %s...\n", labName);

    for (i = 0; i < labNTests; i++) {
        printf("TEST %d/%d: ", i+1, labNTests);
        if (labTests[i].feeder() != 0) {
            break;
        }
        if (labDo(argv[1]) != 0) {
            break;
        }
        if (labTests[i].checker() != 0) {
            break;
        }
    }

    if (i < labNTests) {
        printf("\n:-(\n\n"
        "Exe %s failed for input file in.txt in the current directory.\n"
        "Please fix and try again.\n", argv[1]);
        return 1;
    } else {
        printf("\n:-)\n\n"
        "Exe %s passed all tests.\n"
        "Please review the source code with your teaching assistant.\n", argv[1]);
        return 0;
    }
}

#if defined _WIN32
#include <windows.h>
int labDo(char *labExe)
{
    SECURITY_ATTRIBUTES labInhertitIO = {sizeof(SECURITY_ATTRIBUTES), NULL, TRUE};
    const HANDLE labIn = CreateFile("in.txt",
        GENERIC_READ,          // open for reading
        FILE_SHARE_READ,       // share for reading
        &labInhertitIO,        // default security
        OPEN_EXISTING,         // existing file only
        FILE_ATTRIBUTE_NORMAL, // normal file
        NULL);                 // no attr. template

    const HANDLE labOut = CreateFile("out.txt",
        GENERIC_WRITE,         // open for writing
        FILE_SHARE_WRITE,      // share for writing (0 = do not share)
        &labInhertitIO,        // default security
        CREATE_ALWAYS,         // overwrite existing
        FILE_ATTRIBUTE_NORMAL, // normal file
        NULL);                 // no attr. template
    const HANDLE labErr = GetStdHandle(STD_ERROR_HANDLE);
    STARTUPINFO labStartup = {
        sizeof(STARTUPINFO), // DWORD  cb;
        NULL, // LPTSTR lpReserved; must be NULL, see MSDN
        NULL, // LPTSTR lpDesktop;
        NULL, // LPTSTR lpTitle;
        0, // DWORD  dwX; ignored, see dwFlags
        0, // DWORD  dwY; ignored, see dwFlags
        0, // DWORD  dwXSize; ignored, see dwFlags
        0, // DWORD  dwYSize; ignored, see dwFlags
        0, // DWORD  dwXCountChars; ignored, see dwFlags
        0, // DWORD  dwYCountChars; ignored, see dwFlags
        0, // DWORD  dwFillAttribute; ignored, see dwFlags
        STARTF_USESTDHANDLES, // DWORD  dwFlags;
        0, // WORD   wShowWindow; ignored, see dwFlags
        0, // WORD   cbReserved2; must be 0, see MSDN
        NULL, // LPBYTE lpReserved2; must be NULL, see MSDN
	NULL,
	NULL,
	NULL,
    };
    labStartup.hStdInput = labIn; // lab stdin is in.txt
    labStartup.hStdOutput = labOut; // lab stdout is out.txt
    labStartup.hStdError = labErr; // lab and tester share stderr
    PROCESS_INFORMATION labInfo = {0};
    int exitCode = 1;
    JOBOBJECT_EXTENDED_LIMIT_INFORMATION labJobLimits = {
        { // JOBOBJECT_BASIC_LIMIT_INFORMATION
		{.QuadPart = 0}, // ignored -- LARGE_INTEGER PerProcessUserTimeLimit;
		{.QuadPart = 0}, // ignored -- LARGE_INTEGER PerJobUserTimeLimit;
                JOB_OBJECT_LIMIT_PROCESS_MEMORY
                +JOB_OBJECT_LIMIT_JOB_MEMORY
                +JOB_OBJECT_LIMIT_ACTIVE_PROCESS, // DWORD         LimitFlags;
                0, // ignored -- SIZE_T        MinimumWorkingSetSize;
                0, // ignored -- SIZE_T        MaximumWorkingSetSize;
                1, // DWORD         ActiveProcessLimit;
                0, // ignored -- ULONG_PTR     Affinity;
                0, // ignored -- DWORD         PriorityClass;
                0, // ignored -- DWORD         SchedulingClass;
        }, // JOBOBJECT_BASIC_LIMIT_INFORMATION BasicLimitInformation;
	{0}, // reserved --- IO_COUNTERS                       IoInfo;
        0, // SIZE_T                            ProcessMemoryLimit;
        0, // SIZE_T                            JobMemoryLimit;
        0, // SIZE_T                            PeakProcessMemoryUsed;
        0, // SIZE_T                            PeakJobMemoryUsed;
    };
    labJobLimits.ProcessMemoryLimit = labOutOfMemory;
    labJobLimits.JobMemoryLimit = labOutOfMemory;
    labJobLimits.PeakProcessMemoryUsed = labOutOfMemory;
    labJobLimits.PeakJobMemoryUsed = labOutOfMemory;
    const HANDLE labJob = CreateJobObject(&labInhertitIO, "labJob");
    size_t labMem0 = SIZE_MAX;

    if (labJob == 0) {
        printf("\nSystem error: %u in CreateJobObject\n", (unsigned)GetLastError());
        CloseHandle(labIn);
        CloseHandle(labOut);
        return 1;
    }

    if (!SetInformationJobObject(labJob, JobObjectExtendedLimitInformation, &labJobLimits, sizeof(labJobLimits))) {
        printf("\nSystem error: %u in SetInformationJobObject\n", (unsigned)GetLastError());
        CloseHandle(labJob);
        CloseHandle(labIn);
        CloseHandle(labOut);
        return 1;
    }

    if (!CreateProcess(NULL, // LPCTSTR lpApplicationName,
        labExe, // __inout_opt  LPTSTR lpCommandLine,
        &labInhertitIO, // __in_opt     LPSECURITY_ATTRIBUTES lpProcessAttributes,
        &labInhertitIO, // __in_opt     LPSECURITY_ATTRIBUTES lpThreadAttributes,
        TRUE, // __in         BOOL bInheritHandles,
        // CREATE_BREAKAWAY_FROM_JOB -- otherwise this process is assigned to the job which runs labtest*.exe and we can't assign it to labJob
        CREATE_SUSPENDED+CREATE_BREAKAWAY_FROM_JOB, // __in         DWORD dwCreationFlags
        NULL, // __in_opt     LPVOID lpEnvironment,
        NULL, // __in_opt     LPCTSTR lpCurrentDirectory,
        &labStartup, // __in         LPSTARTUPINFO lpStartupInfo,
        &labInfo //__out        LPPROCESS_INFORMATION lpProcessInformation
        ))
    {
        printf("\nSystem error: %u in CreateProcess\n", (unsigned)GetLastError());
        CloseHandle(labJob);
        CloseHandle(labIn);
        CloseHandle(labOut);
        return 1;
    }

    if (!AssignProcessToJobObject(labJob, labInfo.hProcess)) {
        printf("\nSystem error: %u in AssignProcessToJobObject\n", (unsigned)GetLastError());
        TerminateProcess(labInfo.hProcess, EXIT_FAILURE);
        CloseHandle(labInfo.hThread);
        CloseHandle(labInfo.hProcess);
        CloseHandle(labJob);
        CloseHandle(labIn);
        CloseHandle(labOut);
        return 1;
    }

    {
        BOOL in;
        if (!IsProcessInJob(labInfo.hProcess, labJob, &in)) {
            printf("\nSystem error: %u in IsProcessInJob\n", (unsigned)GetLastError());
            TerminateProcess(labInfo.hProcess, EXIT_FAILURE);
            CloseHandle(labInfo.hThread);
            CloseHandle(labInfo.hProcess);
            CloseHandle(labJob);
            CloseHandle(labIn);
            CloseHandle(labOut);
            return 1;
        }
    }
    {
        QueryInformationJobObject(
            labJob,
            JobObjectExtendedLimitInformation,
            &labJobLimits,
            sizeof(labJobLimits),
            NULL);


        //fprintf(stderr, "PeakProcessMemoryUsed %d\n", labJobLimits.PeakProcessMemoryUsed);
        //fprintf(stderr, "PeakJobMemoryUsed %d\n", labJobLimits.PeakJobMemoryUsed);
        labMem0 = labJobLimits.PeakProcessMemoryUsed;
    }


    if (ResumeThread(labInfo.hThread) == (DWORD)-1) {
        printf("\nSystem error: %u in ResumeThread\n", (unsigned)GetLastError());
        TerminateProcess(labInfo.hProcess, EXIT_FAILURE);
        CloseHandle(labInfo.hThread);
        CloseHandle(labInfo.hProcess);
        CloseHandle(labJob);
        CloseHandle(labIn);
        CloseHandle(labOut);
        return 1;
    }

    switch (WaitForSingleObject(labInfo.hProcess, labTimeout)) {
    case WAIT_OBJECT_0:
        {
            DWORD labExit = EXIT_FAILURE;
            if (!GetExitCodeProcess(labInfo.hProcess, &labExit)) {
                printf("\nSystem error: %u in GetExitCodeProcess\n", (unsigned)GetLastError());
            } else if (labExit >= 0x8000000) {
                printf("\nExe \"%s\" terminated with exception 0x%08x\n", labExe, (unsigned)labExit);
            } else if (labExit > 0) {
                printf("\nExe \"%s\" terminated with exit code 0x%08x != 0\n", labExe, (unsigned)labExit);
            } else {
                exitCode = 0; // + check memory footprint after this switch (...) {...}
            }
            break;
        }
    case WAIT_TIMEOUT:
        printf("\nExe \"%s\" didn't terminate in %d seconds\n", labExe, 1+(labTimeout-1)/1000);
        TerminateProcess(labInfo.hProcess, EXIT_FAILURE);
        break;
    case WAIT_FAILED:
        printf("\nSystem error: %u in WaitForSingleObject\n", (unsigned)GetLastError());
        TerminateProcess(labInfo.hProcess, EXIT_FAILURE);
        break;
    default:
        printf("\nInternal error: WaitForSingleObject returned WAIT_ABANDONED\n");
        TerminateProcess(labInfo.hProcess, EXIT_FAILURE);
    }
    {
        QueryInformationJobObject(
            labJob,
            JobObjectExtendedLimitInformation,
            &labJobLimits,
            sizeof(labJobLimits),
            NULL);
        //fprintf(stderr, "PeakProcessMemoryUsed %d\n", labJobLimits.PeakProcessMemoryUsed);
        //fprintf(stderr, "PeakJobMemoryUsed %d\n", labJobLimits.PeakJobMemoryUsed);
        labMem0 = labJobLimits.PeakProcessMemoryUsed-labMem0;
    }
    if (exitCode == 0 && labUserOutOfMemory == -1 && (long long)labMem0 > labOutOfMemory) {
        exitCode = 1, printf("\nExe \"%s\" used %dK > %dK\n", labExe, 1+(labMem0-1)/1024, 1+(labOutOfMemory-1)/1024);
    } else if (exitCode == 0 && labUserOutOfMemory != -1 && (long long)labMem0 > labUserOutOfMemory) {
        exitCode = 1, printf("\nExe \"%s\" used %dK > %dK\n", labExe, 1+(labMem0-1)/1024, 1+(labUserOutOfMemory-1)/1024);
    }

    CloseHandle(labInfo.hThread);
    CloseHandle(labInfo.hProcess);
    CloseHandle(labJob);
    CloseHandle(labIn);
    CloseHandle(labOut);
    return exitCode;
}
#else
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>             //for open with constants
#include <unistd.h>            //for close
#include <sys/types.h>         //for pid_t type
#include <errno.h>             //for errno
#include <sys/wait.h>          //for wait4
#include <time.h>              //for nanosleep
#include <signal.h>            //for sigaction
/* Задаём некие параметры безопасности.
 * Перехватываем поток ошибок?
 * Задаём начальные параметры для будущего процесса.
 * Что-то с проверкой семафоров на ошибки?
 * Задаём ограничения для будущего процесса.
 * Создаём некий рабочий объект labJob. Если не удалось, то ругаемся.
 * Привязываем к объекту некую информацию. Если не удалось, то ругаемся.
 * Проверяем процесс на какие-то ошибки.
 * Запрашиваем какую-то информацию об объекте.
 * Проверяем поток на какие-то ошибки.
 * Проверяем на завершение и выдаём букет ошибок.
 * Запрашиваем информацию об объекте.
 * Проверяем кол-во использованной памяти.
 * Закрываемся.
 */
static int check_memory(struct rusage rusage, size_t * labMem0) {
    *labMem0 = (size_t) (
	       rusage.ru_isrss + //stack
             + rusage.ru_idrss + //data??
             + rusage.ru_ixrss);  //general??
                                 //???
    if (labOutOfMemory < *labMem0) {
        return(1);
    }
    return(0);
}

int labDo(char *labExe)
{
    int exitCode = 1;
    pid_t pid;

    fflush(stdout);
    if (-1 == access(".", R_OK | W_OK | X_OK | F_OK)) {
        printf("\nSystem error: \"%s\" in access\n", strerror(errno));
        return exitCode;
    }
    pid = fork();
    if (-1 == pid) {
        printf("\nSystem error: \"%s\" in fork\n", strerror(errno));
        return exitCode;
    } else if (!pid) {
        int ret = 0;

        freopen("in.txt", "r", stdin);
        freopen("out.txt", "w", stdout);
        ret = execl(labExe, labExe, NULL);
        if (-1 == ret) {
            fprintf(stderr, "\nSystem error: \"%s\" in execl\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }
    {
        int status = 0;
        struct rusage rusage;
        struct timespec rem;

        rem.tv_sec = 1 + (labTimeout - 1) / 1000;
        rem.tv_nsec = 0;
        if (nanosleep(&rem, &rem)) {
            while (EINTR != errno && rem.tv_sec > 0 && rem.tv_nsec > 0 && nanosleep(&rem, &rem)) ;
        }
        status = wait4(pid, &status, WNOHANG, &rusage); //what is WNOHANG???
        if (-1 == status) {
            printf("\nSystem error: \"%s\" in wait4\n", strerror(errno));
            return exitCode;
        } else if (0 == status) {
            if (kill(pid, SIGKILL)) {
                printf("\nSystem error: \"%s\" in kill\n", strerror(errno));
                return exitCode;
            }
            printf("\nExe \"%s\" didn't terminate in %d seconds\n", labExe, 1+(labTimeout-1)/1000);
            return exitCode;
        } else {
            size_t labMem0 = 0;

            if (check_memory(rusage, &labMem0)) {
                printf("\nExe \"%s\" used %luK > %luK\n", labExe, 1+(labMem0-1)/1024, 1+(labOutOfMemory-1)/1024); //?!
                return exitCode;
            } else {
                exitCode = 0;
            }
        }
    }

    return exitCode;
}
#endif

