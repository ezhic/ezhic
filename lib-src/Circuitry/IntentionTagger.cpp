#include "IntentionTagger.h"
#include "CircuitPredicates.h"
#include <algorithm>

namespace ezhic {

struct TraceChecker
{  // just binds a history and executes functors with it
    TraceChecker(const EventTrace &history) : d_history(history) {}
    template <typename Predicate>
    bool operator()(const Predicate *predicate) const
    {
        return predicate ? (*predicate)(d_history) : false;
    }

    const EventTrace &d_history;
};

template <typename CVector, typename Predicate>
static bool hasMatchesWithPredicate(const CVector &checkers,
                                    Predicate predicate)
{
    return checkers.end() !=
           std::find_if(checkers.begin(), checkers.end(), predicate);
};

Event::Intention IntentionTagger::operator()(Circuit &circuit) const
{
    const EventTrace &eventTrace = circuit.eventTrace();
    Circuit::Health currentCircuitHealth = circuit.health();

    if (hasMatchesWithPredicate(d_checkers[SHOULD_BREAK],
                                TraceChecker(eventTrace))) {
        circuit.health(Circuit::BAD);
        return Event::NO_RUN;
    }

    if (currentCircuitHealth == Circuit::GOOD)
        return Event::ROUTINE;

    // circuit state is bad
    if (hasMatchesWithPredicate(d_checkers[SHOULD_MEND],
                                TraceChecker(eventTrace))) {
        circuit.health(Circuit::GOOD);
        return Event::ROUTINE;
    }

    bool shouldProbe = hasMatchesWithPredicate(d_checkers[SHOULD_PROBE],
                                               TraceChecker(eventTrace));
    if (shouldProbe) {
        return Event::PROBE;
    }
    return Event::NO_RUN;
}

IntentionTagger::IntentionTagger(const IntentionTagger &rhs)
{
    for (unsigned t = 0; t != NONE; ++t)
        for (std::vector<const Circuitry::TracePredicate *>::const_iterator
                 it = rhs.d_checkers[t].begin();
             it != rhs.d_checkers[t].end();
             ++it) {
            d_checkers[t].push_back((*it)->clone());
        }
}

IntentionTagger &IntentionTagger::operator=(const IntentionTagger &rhs)
{
    if (this != &rhs) {
        for (unsigned t = 0; t != NONE; ++t)
            for (std::vector<const Circuitry::TracePredicate *>::const_iterator
                     it = rhs.d_checkers[t].begin();
                 it != rhs.d_checkers[t].end();
                 ++it) {
                d_checkers[t].push_back((*it)->clone());
            }
    }
    return *this;
}

void IntentionTagger::swap(IntentionTagger &rhs)
{
    for (unsigned t = 0; t != NONE; ++t)
        rhs.d_checkers[t].swap(d_checkers[t]);
}

IntentionTagger::~IntentionTagger()
{
    for (unsigned t = 0; t != NONE; ++t)
        for (std::vector<const Circuitry::TracePredicate *>::iterator it =
                 d_checkers[t].begin();
             it != d_checkers[t].end();
             ++it)
            delete *it;
}

IntentionTagger &
IntentionTagger::cloneIn(CheckerType type,
                         const Circuitry::TracePredicate &checker)
{
    if (type < NONE)
        d_checkers[type].push_back(checker.clone());
    return *this;
}
}
