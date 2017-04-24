#ifndef INCLUDED_HELPERS
#define INCLUDED_HELPERS

#include "Timer.h"
class MonotonicDummy : public ezhic::Timer
{
  public:
    MonotonicDummy() : d_time(0) {}
    size_t now() const { return ++d_time; }
    ezhic::Timer *clone() const { return new MonotonicDummy(*this); }
  private:
    mutable long d_time;
};

class IncreasingDummy : public ezhic::Timer
{
  public:
    IncreasingDummy() : d_time(0) {}
    size_t now() const { return d_time; }
    void timePassed(unsigned long period) { d_time += period; }
    ezhic::Timer *clone() const { return new IncreasingDummy(*this); }
  private:
    long d_time;
};

#endif  // EZHIC_HELPERS_H
