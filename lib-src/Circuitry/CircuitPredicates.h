#ifndef INCLUDED_CIRCUITPREDICATES
#define INCLUDED_CIRCUITPREDICATES

#include "AcceptUpdates.h"
#include "Circuit.h"
#include "Circuitry.h"
#include "Travellers.h"

namespace ezhic {
class EventWrapper;  // forward
class Registry;

namespace Circuitry {

using namespace Traversal;

typedef IteratorWrapper<EventTrace::const_iterator> TraceIt;

inline TraceIt itForTrace(const EventTrace &trace)
{
    return TraceIt(trace.begin(), trace.end());
}

template <typename Iterator, typename Predicate>
bool moreThanKMatch(Iterator it, Predicate predicate, size_t K)
{
    size_t acc;
    for (acc = 0; it && acc <= K; ++it) {
        acc += predicate(*it);
    }
    return acc > K;
};

const long ALL_HISTORY(-1);

// Considers S events:
// S == |trace| if M>=|trace| or M == ALL_HISTORY (else S == M)
// Returns: Whether more than K of last S events match the predicate
template <typename Predicate>
bool moreThanKInLastM(size_t k,
                      long m,
                      const EventTrace &history,
                      Predicate predicate)
{
    if (m == -1)
        m = history.size();
    AtMostNTraveller<TraceIt> it(itForTrace(history), m);
    return moreThanKMatch(it, predicate, k);
}

class CheckMoreThanKInM
{
  public:
    CheckMoreThanKInM(size_t k, long m = ALL_HISTORY);

    static size_t percentageToSize(size_t size, long m, unsigned percents);

  protected:
    const size_t d_k;
    const long d_m;
};

struct IsEvent
{
    IsEvent(Event::Type event) : d_event(event) {}
    bool operator()(const Event &event) { return d_event == event.type(); }
    const Event::Type d_event;
};

template <typename Event::Type event>
class CheckMoreThanKEventsInM
    : public CheckMoreThanKInM,
      public TracePredicateCloneable<CheckMoreThanKEventsInM<event> >
{
  public:
    CheckMoreThanKEventsInM(size_t k, long m = ALL_HISTORY)
    : CheckMoreThanKInM(k, m)
    {
    }
    bool operator()(const EventTrace &history) const
    {
        return moreThanKInLastM(d_k, d_m, history, IsEvent(event));
    }
};

template <typename Event::Type event>
class CheckMoreThanKPercentEventsInM
    : public CheckMoreThanKInM,
      public TracePredicateCloneable<CheckMoreThanKPercentEventsInM<event> >
{
  public:
    CheckMoreThanKPercentEventsInM(size_t percents, long m = ALL_HISTORY)
    : CheckMoreThanKInM(percents, m)
    {
    }
    bool operator()(const EventTrace &history)
    {
        size_t k = percentageToSize(history.size(), d_m, d_k);
        return moreThanKInLastM(k, d_m, history, IsEvent(event));
    }
};

typedef CheckMoreThanKEventsInM<Event::SUCCESS> RepairAfterKSuccessInM;
typedef CheckMoreThanKEventsInM<Event::TIMEOUT> BreakAfterKTimeoutsInM;
typedef CheckMoreThanKEventsInM<Event::EXCEPTION> BreakAfterKExceptionsInM;
typedef CheckMoreThanKEventsInM<Event::SUCCESS> RepairAfterKSuccessInM;
typedef CheckMoreThanKPercentEventsInM<Event::SUCCESS>
    RepairAfterKPercentsSuccessInM;
typedef CheckMoreThanKPercentEventsInM<Event::TIMEOUT>
    BreakAfterKPercentsTimeoutsInM;
typedef CheckMoreThanKPercentEventsInM<Event::EXCEPTION>
    BreakAfterKPercentsExceptionsInM;

// any failure (e.g., exception, timeout, invalid result
class BreakAfterKFailsInM : public CheckMoreThanKInM
{
  public:
    BreakAfterKFailsInM(size_t k, long m = ALL_HISTORY);

    bool operator()(const EventTrace &history) const;
};

// any failure (e.g., exception, timeout, invalid result
class BreakAfterKPercentsFailsInM : public CheckMoreThanKInM
{
  public:
    BreakAfterKPercentsFailsInM(unsigned percents, long m = ALL_HISTORY);

    bool operator()(const EventTrace &history) const;
};

// probe if there were more than K events after breaking or probing
class ProbeAfterK : public CheckMoreThanKInM,
                    public TracePredicateCloneable<ProbeAfterK>
{
  public:
    ProbeAfterK(size_t k);
    bool operator()(const EventTrace &history) const;
};
}
}
#endif
