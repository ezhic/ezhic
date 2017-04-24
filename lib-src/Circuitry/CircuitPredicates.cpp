#include "CircuitPredicates.h"
#include <algorithm>
#include <stdexcept>

namespace ezhic {
namespace Circuitry {

// translate percents to actual number (mind the ALL_HISTORY convention...)
size_t CheckMoreThanKInM::percentageToSize(size_t totalSize,
                                           long sizeOfInterestOrAll,
                                           unsigned percents)
{
    size_t tsize = sizeOfInterestOrAll == ezhic::Circuitry::ALL_HISTORY
                       ? totalSize
                       : std::min(size_t(sizeOfInterestOrAll), totalSize);
    return tsize * percents / 100;
}

// Helper predicates ======================

struct IsSuccessOrRecourse
{
    bool operator()(const Event &event)
    {
        return event.type() == Event::SUCCESS ||
               event.type() == Event::RECOURSE;
    }
};

struct IsNotEvent
{
    IsNotEvent(Event::Type event) : d_event(event) {}
    bool operator()(const Event &event) { return d_event != event.type(); }
    const Event::Type d_event;
};

bool BreakAfterKFailsInM::operator()(const EventTrace &history) const
{
    return moreThanKInLastM(d_k, d_m, history, IsNotEvent(Event::SUCCESS));
}

bool BreakAfterKPercentsFailsInM::operator()(const EventTrace &history) const
{
    size_t k = percentageToSize(history.size(), d_m, d_k);
    return moreThanKInLastM(k, d_m, history, IsNotEvent(Event::SUCCESS));
}

BreakAfterKFailsInM::BreakAfterKFailsInM(size_t k, long m)
: CheckMoreThanKInM(k, m)
{
}

BreakAfterKPercentsFailsInM::BreakAfterKPercentsFailsInM(unsigned percents,
                                                         long m)
: CheckMoreThanKInM(percents, m)
{
}

ProbeAfterK::ProbeAfterK(size_t k) : CheckMoreThanKInM(k, ALL_HISTORY)
{
}

// check if probing test loop should continue
static bool probingChecker(const Event &event)
{
    if (Event::CIRCUIT_BROKE == event.type())
        return false;
    if (event.intention() == Event::PROBE)
        return Event::SUCCESS == event.type();

    return true;
}

// probe if there were more than K events after breaking or after last failed
// probing
bool ProbeAfterK::operator()(const EventTrace &history) const
{
    size_t count = 0;
    EventTrace::const_iterator it = history.begin();
    while (it != history.end() && probingChecker(*it) && count <= d_k) {
        ++count;
        ++it;
    }
    return count > d_k;
}

CheckMoreThanKInM::CheckMoreThanKInM(size_t k, long m) : d_k(k), d_m(m)
{
}
}
}