#include "testLab.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <stdarg.h>
#include <time.h>
#if defined __linux__
#include <sys/resource.h>
#endif

static int LaunchLabExecutable(char* labExe);
static int UserTestAll = 0;
static int UserMemoryLimit = -1;
static int UserTimeout = -1;

static size_t GetMemoryLimit(void) {
    return UserMemoryLimit < 0 ? GetTestMemoryLimit() : (size_t) UserMemoryLimit;
}

static int GetTimeout(void) {
    return UserTimeout < 0 ? GetTestTimeout() : UserTimeout;
}

static void PrintWithoutBuffering(const char format[], ...) {
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    fflush(stdout);
}

static const char* GetRunnerCommand(const char* runnerExe, char* labExe) {
    static char runnerCommand[4096] = {0};
    int testRunnerSize = snprintf(
        runnerCommand, sizeof(runnerCommand), "%s -m %zu -t %d -e %s",
        runnerExe, (GetMemoryLimit() + 1023) / 1024, GetTimeout(), labExe);
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (testRunnerSize < 0) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("\nInternal error: snprintf returned negative value\n");
        return NULL;
    }
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (testRunnerSize == sizeof(runnerCommand) - 1) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("\nInternal error: snprintf stopped at the end of buffer\n");
        return NULL;
    }
    return runnerCommand;
}

int main(int argc, char* argv[]) {
    int i, count, fail;
    const char* runnerExe = argv[0];

    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (argc >= 3 && strcmp(argv[1], "-a") == 0) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        UserTestAll = 1;
        argv++;
        argc--;
    }
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (argc >= 4 && strcmp(argv[1], "-m") == 0 && atoi(argv[2]) != 0) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        UserMemoryLimit = atoi(argv[2])*1024;
        argv += 2;
        argc -= 2;
    }
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (argc >= 4 && strcmp(argv[1], "-t") == 0 && atoi(argv[2]) != 0) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        UserTimeout = atoi(argv[2]);
        argv += 2;
        argc -= 2;
    }
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (argc >= 3 && strcmp(argv[1], "-e") == 0) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        return LaunchLabExecutable(argv[2]);
    }

    PrintWithoutBuffering("\nKOI FIT NSU Lab Tester (c) 2009-2020 by Evgueni Petrov\n");

    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (argc < 2) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("\nTo test mylab.exe, do %s mylab.exe\n", runnerExe);
        return 1;
    }

    PrintWithoutBuffering("\nTesting %s...\n", GetTesterName());

    for (i = 0, count = 0, fail = 0; i < GetTestCount(); i++, fail = 0) {
        PrintWithoutBuffering("TEST %d/%d: ", i+1, GetTestCount());
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        if (GetLabTest(i).Feeder() != 0) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            if (!UserTestAll) {
                fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
                break;
            }
            fail = 1;
        }
        const char* runnerCommand = GetRunnerCommand(runnerExe, argv[1]);
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        if (!runnerCommand) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            break;
        }
        double ms0 = GetTickCount();
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        if (system(runnerCommand) != 0) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            break;
        }
        double ms1 = GetTickCount();
        int millisecondsPerDay = 1000 * 60 * 60 * 24;
        double msElapsed = ((int)(ms1 - ms0) + millisecondsPerDay) % millisecondsPerDay;
        PrintWithoutBuffering("%.0f ms, ", msElapsed);
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        if (GetLabTest(i).Checker() != 0) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            if (!UserTestAll) {
                fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
                break;
            }
            fail = 1;
        }
        count += 1 - fail;
    }

    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (count < GetTestCount()) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("\n:-(\n\n"
        "Executable file %s failed for input file in.txt in the current directory.\n"
        "Please fix and try again.\n", argv[1]);
        return 1;
    } else {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("\n:-)\n\n"
        "Executable file %s passed all tests.\n"
        "Please review the source code with your teaching assistant.\n", argv[1]);
        return 0;
    }
}

int HaveGarbageAtTheEnd(FILE* out) {
    while (1) {
        char c;
        int status = fscanf(out, "%c", &c);
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        if (status < 0) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            return 0;
        }
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        if (!strchr(" \t\r\n", c)) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            PrintWithoutBuffering("garbage at the end -- ");
            return 1;
        }
    }
}

const char Pass[] = "PASSED";
const char Fail[] = "FAILED";

const char* ScanUintUint(FILE* out, unsigned* a, unsigned* b) {
    int status = fscanf(out, "%u%u", a, b);
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (status < 0) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("output too short -- ");
        return Fail;
    } else if (status < 2) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("bad output format -- ");
        return Fail;
    }
    return Pass;
}

const char* ScanIntInt(FILE* out, int* a, int* b) {
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (ScanInt(out, a) == Pass && ScanInt(out, b) == Pass) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        return Pass;
    }
    return Fail;
}

const char* ScanU32(FILE* out, uint32_t* a) {
    int status = fscanf(out, "%u", a);
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (status < 0) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("output too short -- ");
        return Fail;
    } else if (status < 1) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("bad output format -- ");
        return Fail;
    }
    return Pass;
}

const char* ScanInt(FILE* out, int* a) {
    int status = fscanf(out, "%d", a);
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (status < 0) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("output too short -- ");
        return Fail;
    } else if (status < 1) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("bad output format -- ");
        return Fail;
    }
    return Pass;
}

const char* ScanChars(FILE* out, size_t bufferSize, char* buffer) {
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (fgets(buffer, bufferSize, out) == NULL) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("no output -- ");
        return Fail;
    }
    char* newlinePtr = strchr(buffer, '\n');
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (newlinePtr != NULL) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        *newlinePtr = '\0';
    }
    return Pass;
}

size_t GetLabPointerSize(void) {
    return 8; // TODO: determine from bitness of the lab executable
}

unsigned RoundUptoThousand(unsigned int n) {
    return (n + 999) / 1000 * 1000;
}

static void ReportTimeout(const char labExe[]) {
    PrintWithoutBuffering("\nExecutable file \"%s\" didn't terminate in %d seconds\n", labExe, RoundUptoThousand(GetTimeout()) / 1000);
}

static void ReportOutOfMemory(const char labExe[], unsigned labMem) {
    PrintWithoutBuffering("\nExecutable file \"%s\" used %dKB > %dKB\n", labExe, RoundUptoThousand(labMem) / 1000, RoundUptoThousand(GetMemoryLimit()) / 1000);
}

static void ReportException(const char labExe[]) {
    PrintWithoutBuffering("\nExecutable file \"%s\" terminated with exception\n", labExe);
}

static void ReportNonZeroExitCode(const char labExe[]) {
    PrintWithoutBuffering("\nExecutable file \"%s\" terminated with return code != 0\n", labExe);
}

#if defined _WIN32
#include <windows.h>
int LaunchLabExecutable(char* labExe)
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
    labJobLimits.ProcessMemoryLimit = GetMemoryLimit();
    labJobLimits.JobMemoryLimit = GetMemoryLimit();
    labJobLimits.PeakProcessMemoryUsed = GetMemoryLimit();
    labJobLimits.PeakJobMemoryUsed = GetMemoryLimit();
    const HANDLE labJob = CreateJobObject(&labInhertitIO, "labJob");
    size_t labMem0 = SIZE_MAX;

    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (labJob == 0) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("\nSystem error: %u in CreateJobObject\n", (unsigned)GetLastError());
        CloseHandle(labIn);
        CloseHandle(labOut);
        return 1;
    }

    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (!SetInformationJobObject(labJob, JobObjectExtendedLimitInformation, &labJobLimits, sizeof(labJobLimits))) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("\nSystem error: %u in SetInformationJobObject\n", (unsigned)GetLastError());
        CloseHandle(labJob);
        CloseHandle(labIn);
        CloseHandle(labOut);
        return 1;
    }

    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
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
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("\nSystem error: %u in CreateProcess\n", (unsigned)GetLastError());
        CloseHandle(labJob);
        CloseHandle(labIn);
        CloseHandle(labOut);
        return 1;
    }

    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (!AssignProcessToJobObject(labJob, labInfo.hProcess)) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("\nSystem error: %u in AssignProcessToJobObject\n", (unsigned)GetLastError());
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
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        if (!IsProcessInJob(labInfo.hProcess, labJob, &in)) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            PrintWithoutBuffering("\nSystem error: %u in IsProcessInJob\n", (unsigned)GetLastError());
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


    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (ResumeThread(labInfo.hThread) == (DWORD)-1) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("\nSystem error: %u in ResumeThread\n", (unsigned)GetLastError());
        TerminateProcess(labInfo.hProcess, EXIT_FAILURE);
        CloseHandle(labInfo.hThread);
        CloseHandle(labInfo.hProcess);
        CloseHandle(labJob);
        CloseHandle(labIn);
        CloseHandle(labOut);
        return 1;
    }

    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    switch (WaitForSingleObject(labInfo.hProcess, GetTimeout())) {
    case WAIT_OBJECT_0:
        {
            DWORD labExit = EXIT_FAILURE;
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            if (!GetExitCodeProcess(labInfo.hProcess, &labExit)) {
                fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
                PrintWithoutBuffering("\nSystem error: %u in GetExitCodeProcess\n", (unsigned)GetLastError());
            } else if (labExit >= 0x8000000) {
                fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
                ReportException(labExe);
            } else if (labExit > 0) {
                fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
                ReportNonZeroExitCode(labExe);
            } else {
                fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
                exitCode = 0; // + check memory footprint after this switch (...) {...}
            }
            break;
        }
    case WAIT_TIMEOUT:
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        ReportTimeout(labExe);
        TerminateProcess(labInfo.hProcess, EXIT_FAILURE);
        break;
    case WAIT_FAILED:
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("\nSystem error: %u in WaitForSingleObject\n", (unsigned)GetLastError());
        TerminateProcess(labInfo.hProcess, EXIT_FAILURE);
        break;
    default:
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        PrintWithoutBuffering("\nInternal error: WaitForSingleObject returned WAIT_ABANDONED\n");
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
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if ((long long)labMem0 > GetMemoryLimit()) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        exitCode = 1;
        ReportOutOfMemory(labExe, labMem0);
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
#include <sys/time.h>

#ifdef __APPLE__
// https://unix.stackexchange.com/questions/30940/getrusage-system-call-what-is-maximum-resident-set-size#comment552809_30940
#define RU_MAXRSS_UNITS 1u
#else
#define RU_MAXRSS_UNITS 1024u
#endif

DWORD GetTickCount(void) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (DWORD)(tv.tv_sec*1000+tv.tv_usec/1000);
}

static int CheckMemory(struct rusage rusage, size_t * labMem0) {
    *labMem0 = (size_t) rusage.ru_maxrss * RU_MAXRSS_UNITS;
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (GetMemoryLimit() < *labMem0) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        return 1;
    }
    return 0;
}

static void ReportSystemError(const char api[]) {
    PrintWithoutBuffering("\nSystem error: \"%s\" in %s\n", strerror(errno), api);
}

static void SigchldTrap(int signo) {
    // Заглушка, чтобы спровоцировать EINTR при SIGCHLD
    // По умолчанию SIGCHLD работает как SIG_IGN c SA_RESTART и без SA_NOCLDWAIT
    // Выставить вручную SIG_IGN нельзя, поскольку будет трактоваться как SA_NOCLDWAIT
    (void) signo;
}

static int _LaunchLabExecutable(char* labExe);

static int LaunchLabExecutable(char* labExe) {
    struct sigaction new = {0};
    new.sa_handler = SigchldTrap;
    (void)sigemptyset(&new.sa_mask); // не умеет завершаться неуспешно
    new.sa_flags = SA_NOCLDSTOP; // Не используем SA_RESETHAND, чтобы не зависеть от использования fork вне LaunchLabExecutable
    struct sigaction old;
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (sigaction(SIGCHLD, &new, &old)) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        ReportSystemError("sigaction");
        return -1;
    }

    int retCode = _LaunchLabExecutable(labExe);
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (sigaction(SIGCHLD, &old, NULL)) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        ReportSystemError("sigaction");
        return -1;
    }

    return retCode;
}

typedef enum {Timeout, Exception, NonZeroStatus, OK, WaitError} EWaitStatus;

static int WaitForProcess(pid_t pid, struct timespec* ts, struct rusage* rusage) {
    while (ts->tv_sec > 0 || ts->tv_nsec > 0) {
        int status = 0;
        pid_t child = wait4(pid, &status, WNOHANG, rusage);
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        if (child == 0) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            int nanosleepStatus = nanosleep(ts, ts);
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            if (nanosleepStatus == 0) {
                fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
                return Timeout;
            }
            assert(errno == EINTR);
        } else if (child == -1) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            return WaitError;
        } else if (WIFEXITED(status)) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            if (WEXITSTATUS(status) == 0) {
                fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
                return OK;
            } else {
                fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
                return NonZeroStatus;
            }
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            return Exception;
        } else {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            return WaitError;
        }
    }
    return Timeout;
}

static int _LaunchLabExecutable(char* labExe)
{
    int exitCode = 1;

    fflush(stdout);
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (-1 == access(".", R_OK | W_OK | X_OK | F_OK)) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        ReportSystemError("access");
        return exitCode;
    }
    pid_t pid = fork();
    fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
    if (-1 == pid) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        ReportSystemError("fork");
        return exitCode;
    } else if (0 == pid) {
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        // in forked process
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        if (!freopen("in.txt", "r", stdin)) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            ReportSystemError("freopen");
            exit(EXIT_FAILURE);
        }
        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        if (!freopen("out.txt", "w", stdout)) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            ReportSystemError("freopen");
            exit(EXIT_FAILURE);
        }
        int ret = execl(labExe, labExe, NULL);
        assert(ret == -1);
        (void)ret; // -Wunused-variable for NDEBUG
        ReportSystemError("execl");
        exit(EXIT_FAILURE);
    }
    {
        // in main process
        struct rusage rusage = {0};
        struct timespec rem = {0};
        rem.tv_sec = GetTimeout() / 1000;
        rem.tv_nsec = (GetTimeout() % 1000) * 1000000;
        EWaitStatus status = WaitForProcess(pid, &rem, &rusage);

        fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
        if (WaitError == status) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            ReportSystemError("wait4");
        } else if (Timeout == status) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            ReportTimeout(labExe);
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            if (kill(pid, SIGKILL)) {
                fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
                ReportSystemError("kill");
            }
        } else if (Exception == status) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            ReportException(labExe);
        } else if (NonZeroStatus == status) {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            ReportNonZeroExitCode(labExe);
        } else {
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            size_t labMem0 = 0;
            fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
            if (CheckMemory(rusage, &labMem0)) {
                fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
                ReportOutOfMemory(labExe, labMem0);
            } else {
                fprintf(stderr, "function: %s, line: %d\n", __FUNCTION__, __LINE__);
                exitCode = 0;
            }
        }
    }

    return exitCode;
}
#endif
