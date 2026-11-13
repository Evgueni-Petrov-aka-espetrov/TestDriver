#include <features.h>

#if defined(__GLIBC__) && (defined(__x86_64__) || defined(__i386__))

typedef int (*start_main_t)(int, char **, char **);

extern int glibc_start_main_old(start_main_t main_fn, int argc, char **argv, void (*init)(void), void (*fini)(void),
                                void (*rtld_fini)(void), void *stack_end);
__asm__(".symver glibc_start_main_old,__libc_start_main@GLIBC_2.2.5");

int __wrap___libc_start_main(start_main_t main_fn, int argc, char **argv, void (*init)(void), void (*fini)(void),
                             void (*rtld_fini)(void), void *stack_end)
{
    return glibc_start_main_old(main_fn, argc, argv, init, fini, rtld_fini, stack_end);
}

#else

typedef int glibc_compat_unused;

#endif
