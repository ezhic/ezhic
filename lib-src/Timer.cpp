#include "Timer.h"

#define USE_LINUX_SYS_TIME 1
#ifdef USE_LINUX_SYS_TIME
#include <sys/time.h>
#endif

namespace ezhic {

static size_t _now()
{
#ifdef USE_LINUX_SYS_TIME
    timeval t;
    gettimeofday(&t, 0);
    return size_t(t.tv_sec) * 1000 + size_t(t.tv_usec) / 1000;
#else
    return 0;
#endif
}

size_t LinuxSysTimer::now() const
{
    return _now();
}
Timer *LinuxSysTimer::clone() const
{
    return new LinuxSysTimer(*this);
}
}
