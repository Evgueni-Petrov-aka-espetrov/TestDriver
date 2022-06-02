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
    if (testRunnerSize < 0) {
        PrintWithoutBuffering("\nInternal error: snprintf returned negative value\n");
        return NULL;
    }
    if (testRunnerSize == sizeof(runnerCommand) - 1) {
        PrintWithoutBuffering("\nInternal error: snprintf stopped at the end of buffer\n");
        return NULL;
    }
    return runnerCommand;
}

int main(int argc, char* argv[])
{
    int i;
    const char* runnerExe = argv[0];

    if (argc >= 4 && strcmp(argv[1], "-m") == 0 && atoi(argv[2]) != 0) {
        UserMemoryLimit = atoi(argv[2])*1024;
        argv += 2;
        argc -= 2;
    }
    if (argc >= 4 && strcmp(argv[1], "-t") == 0 && atoi(argv[2]) != 0) {
        UserTimeout = atoi(argv[2]);
        argv += 2;
        argc -= 2;
    }
    if (argc >= 3 && strcmp(argv[1], "-e") == 0) {
        return LaunchLabExecutable(argv[2]);
    }

    PrintWithoutBuffering("\nKOI FIT NSU Lab Tester (c) 2009-2020 by Evgueni Petrov\n");

    if (argc < 2) {
        PrintWithoutBuffering("\nTo test mylab.exe, do %s mylab.exe\n", runnerExe);
        return 1;
    }

    PrintWithoutBuffering("\nTesting %s...\n", GetTesterName());

    for (i = 0; i < GetTestCount(); i++) {
        PrintWithoutBuffering("TEST %d/%d: ", i+1, GetTestCount());
        if (GetLabTest(i).Feeder() != 0) {
            break;
        }
        const char* runnerCommand = GetRunnerCommand(runnerExe, argv[1]);
        if (!runnerCommand) {
            break;
        }
        double ms0 = GetTickCount();
        if (system(runnerCommand) != 0) {
            break;
        }
        double ms1 = GetTickCount();
        int millisecondsPerDay = 1000 * 60 * 60 * 24;
        double msElapsed = ((int)(ms1 - ms0) + millisecondsPerDay) % millisecondsPerDay;
        PrintWithoutBuffering("%.0f ms, ", msElapsed);
        if (GetLabTest(i).Checker() != 0) {
            break;
        }
    }

    if (i < GetTestCount()) {
        PrintWithoutBuffering("\n:-(\n\n"
        "Executable file %s failed for input file in.txt in the current directory.\n"
        "Please fix and try again.\n", argv[1]);
        return 1;
    } else {
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
        if (status < 0) {
            return 0;
        }
        if (!strchr(" \t\r\n", c)) {
            PrintWithoutBuffering("garbage at the end -- ");
            return 1;
        }
    }
}

const char Pass[] = "PASSED";
const char Fail[] = "FAILED";

const char* ScanUintUint(FILE* out, unsigned* a, unsigned* b) {
    int status = fscanf(out, "%u%u", a, b);
    if (status < 0) {
        PrintWithoutBuffering("output too short -- ");
        return Fail;
    } else if (status < 2) {
        PrintWithoutBuffering("bad output format -- ");
        return Fail;
    }
    return Pass;
}

const char* ScanIntInt(FILE* out, int* a, int* b) {
    if (ScanInt(out, a) == Pass && ScanInt(out, b) == Pass) {
        return Pass;
    }
    return Fail;
}

const char* ScanInt(FILE* out, int* a) {
    int status = fscanf(out, "%d", a);
    if (status < 0) {
        PrintWithoutBuffering("output too short -- ");
        return Fail;
    } else if (status < 1) {
        PrintWithoutBuffering("bad output format -- ");
        return Fail;
    }
    return Pass;
}

const char* ScanChars(FILE* out, size_t bufferSize, char* buffer) {
    if (fgets(buffer, bufferSize, out) == NULL) {
        PrintWithoutBuffering("no output -- ");
        return Fail;
    }
    char* newlinePtr = strchr(buffer, '\n');
    if (newlinePtr != NULL) {
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
    PrintWithoutBuffering("\nExecutable file \"%s\" used %dKi > %dKi\n", labExe, RoundUptoThousand(labMem) / 1000, RoundUptoThousand(GetMemoryLimit()) / 1000);
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

    if (labJob == 0) {
        PrintWithoutBuffering("\nSystem error: %u in CreateJobObject\n", (unsigned)GetLastError());
        CloseHandle(labIn);
        CloseHandle(labOut);
        return 1;
    }

    if (!SetInformationJobObject(labJob, JobObjectExtendedLimitInformation, &labJobLimits, sizeof(labJobLimits))) {
        PrintWithoutBuffering("\nSystem error: %u in SetInformationJobObject\n", (unsigned)GetLastError());
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
        PrintWithoutBuffering("\nSystem error: %u in CreateProcess\n", (unsigned)GetLastError());
        CloseHandle(labJob);
        CloseHandle(labIn);
        CloseHandle(labOut);
        return 1;
    }

    if (!AssignProcessToJobObject(labJob, labInfo.hProcess)) {
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
        if (!IsProcessInJob(labInfo.hProcess, labJob, &in)) {
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


    if (ResumeThread(labInfo.hThread) == (DWORD)-1) {
        PrintWithoutBuffering("\nSystem error: %u in ResumeThread\n", (unsigned)GetLastError());
        TerminateProcess(labInfo.hProcess, EXIT_FAILURE);
        CloseHandle(labInfo.hThread);
        CloseHandle(labInfo.hProcess);
        CloseHandle(labJob);
        CloseHandle(labIn);
        CloseHandle(labOut);
        return 1;
    }

    switch (WaitForSingleObject(labInfo.hProcess, GetTimeout())) {
    case WAIT_OBJECT_0:
        {
            DWORD labExit = EXIT_FAILURE;
            if (!GetExitCodeProcess(labInfo.hProcess, &labExit)) {
                PrintWithoutBuffering("\nSystem error: %u in GetExitCodeProcess\n", (unsigned)GetLastError());
            } else if (labExit >= 0x8000000) {
                ReportException(labExe);
            } else if (labExit > 0) {
                ReportNonZeroExitCode(labExe);
            } else {
                exitCode = 0; // + check memory footprint after this switch (...) {...}
            }
            break;
        }
    case WAIT_TIMEOUT:
        ReportTimeout(labExe);
        TerminateProcess(labInfo.hProcess, EXIT_FAILURE);
        break;
    case WAIT_FAILED:
        PrintWithoutBuffering("\nSystem error: %u in WaitForSingleObject\n", (unsigned)GetLastError());
        TerminateProcess(labInfo.hProcess, EXIT_FAILURE);
        break;
    default:
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
    if ((long long)labMem0 > GetMemoryLimit()) {
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
    if (GetMemoryLimit() < *labMem0) {
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
    if (sigaction(SIGCHLD, &new, &old)) {
        ReportSystemError("sigaction");
        return -1;
    }

    int retCode = _LaunchLabExecutable(labExe);
    if (sigaction(SIGCHLD, &old, NULL)) {
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
        if (child == 0) {
            int nanosleepStatus = nanosleep(ts, ts);
            if (nanosleepStatus == 0) {
                return Timeout;
            }
            assert(errno == EINTR);
        } else if (child == -1) {
            return WaitError;
        } else if (WIFEXITED(status)) {
            if (WEXITSTATUS(status) == 0) {
                return OK;
            } else {
                return NonZeroStatus;
            }
        } else if (WIFSIGNALED(status)) {
            return Exception;
        } else {
            return WaitError;
        }
    }
    return Timeout;
}

static int _LaunchLabExecutable(char* labExe)
{
    int exitCode = 1;

    fflush(stdout);
    if (-1 == access(".", R_OK | W_OK | X_OK | F_OK)) {
        ReportSystemError("access");
        return exitCode;
    }
    pid_t pid = fork();
    if (-1 == pid) {
        ReportSystemError("fork");
        return exitCode;
    } else if (0 == pid) {
        // in forked process
        if (!freopen("in.txt", "r", stdin)) {
            ReportSystemError("freopen");
            exit(EXIT_FAILURE);
        }
        if (!freopen("out.txt", "w", stdout)) {
            ReportSystemError("freopen");
            exit(EXIT_FAILURE);
        }
        int ret = execl(labExe, labExe, NULL);
        assert(ret == -1);
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

        if (WaitError == status) {
            ReportSystemError("wait4");
        } else if (Timeout == status) {
            ReportTimeout(labExe);
            if (kill(pid, SIGKILL)) {
                ReportSystemError("kill");
            }
        } else if (Exception == status) {
            ReportException(labExe);
        } else if (NonZeroStatus == status) {
            ReportNonZeroExitCode(labExe);
        } else {
            size_t labMem0 = 0;
            if (CheckMemory(rusage, &labMem0)) {
                ReportOutOfMemory(labExe, labMem0);
            } else {
                exitCode = 0;
            }
        }
    }

    return exitCode;
}
#endif
