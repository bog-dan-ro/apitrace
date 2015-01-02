#ifdef __ANDROID__
#include "os_memory.hpp"
#include <cassert>
#include <cstdio>

#include <fcntl.h>
#include <unistd.h>

char statmBuff[256];
static __uint64_t pageSize = sysconf(_SC_PAGESIZE);

static long size, resident;

static inline void parseStatm()
{
    int fd = open("/proc/self/statm", O_RDONLY, 0);
    int sz = read(fd, statmBuff, 255);
    close(fd);
    statmBuff[sz] = 0;
    sz = sscanf(statmBuff, "%ld %ld",
               &size, &resident);
    assert(sz == 2);
}

long long os::getVsize()
{
    parseStatm();
    return pageSize * size;
}

long long os::getRss()
{
    parseStatm();
    return pageSize * resident;
}
#endif
