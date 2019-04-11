#ifndef labreporter
#define labreporter
#include <stddef.h>

struct labFeedAndCheck {int (*feeder)(void), (*checker)(void);};

extern const struct labFeedAndCheck labTests[];
extern const int labNTests;
extern const char labName[];
extern int labTimeout;
extern size_t labOutOfMemory;

#ifdef _WIN32
// Helpers for Windows
#include <windows.h>
#else
// Helpers and compatibility for Linux
#include <strings.h>
#define _strnicmp(a, b, c) strncasecmp((a), (b), (c))
#include <stdint.h>
#include <sys/time.h>
typedef uint32_t DWORD;
inline DWORD GetTickCount(void)
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (DWORD)(tv.tv_sec*1000+tv.tv_usec/1000);
}
#endif /* _WIN32 */

inline unsigned int tickDifference(unsigned int start, unsigned int finish)
{
    return finish-start;
}
#endif /* labreporter */
