#ifndef DEFINES_H
# define DEFINES_H

# if defined(__amd64__) || defined(__ia64__) || defined(__x86_64__)
#  define ARCHI  64
# else
#  define ARCHI  32
# endif

# if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
#  define SYSTEM 0 // LINUX
# elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__DragonFly__)
#  define SYSTEM 1 // BSD
# else
#  define SYSTEM 2 // OTHERS
# endif

#endif // !DEFINES_H
