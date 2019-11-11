#ifndef labreporter
#define labreporter
#include <stddef.h>

struct labFeedAndCheck {int (*feeder)(void), (*checker)(void);};

extern const struct labFeedAndCheck labTests[];
extern const int labNTests;
extern const char labName[];
extern int labTimeout;
extern size_t labOutOfMemory;

// MIN_PROCESS_RSS_BYTES IS CHANGED
// Before: (1024*1024*sizeof(void*)/4)
// Doubling MIN_PROCESS_RSS_BYTES is humane solution
// that helps people with VS Debug builds, self-written libraries, very old (or very new) compilers.
// Also there is no way to abuse it (no proofs for this statement yet)
#define MIN_PROCESS_RSS_BYTES (1024*1024*sizeof(void*)/2)


#ifdef _WIN32
// Helpers for Windows
#include <windows.h>

inline unsigned int tickDifference(unsigned int start, unsigned int finish)
{
    return finish-start;
}

#else
// Helpers and compatibility for Linux
#include <strings.h>
#define _strnicmp(a, b, c) strncasecmp((a), (b), (c))
#include <stdint.h>
#include <sys/time.h>
typedef uint32_t DWORD;
static inline DWORD GetTickCount(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (DWORD)(tv.tv_sec*1000+tv.tv_usec/1000);
}

static inline unsigned int tickDifference(unsigned int start, unsigned int finish)
{
    return finish-start;
}
#endif
#endif /* labreporter */
