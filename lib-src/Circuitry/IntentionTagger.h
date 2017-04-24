#ifndef INCLUDED_INTENTIONTAGGER
#define INCLUDED_INTENTIONTAGGER

#include "AcceptUpdates.h"
#include "Event.h"

#include <vector>

namespace ezhic {

class Circuit;

// forward declarations of functors
namespace Circuitry {
class TracePredicate;
}

// Stateless utility: will use injected logic to figure
// whether a given circuit should run
class IntentionTagger
{
  public:
    IntentionTagger() {}
    IntentionTagger(const IntentionTagger &);
    IntentionTagger &operator=(const IntentionTagger &);
    void swap(IntentionTagger &rhs);

    virtual ~IntentionTagger();

    // GOOD: should run iff NO checker finds the circuit history intolerable
    // BAD: should run if either: a) the history is good enough to be repaired
    //                            b) we want to probe the unreliable service
    virtual Event::Intention operator()(Circuit &circuit) const;

    enum CheckerType
    {
        SHOULD_BREAK,
        SHOULD_MEND,
        SHOULD_PROBE,
        NONE
    };

    // Own a clone of the given object implementing TracePredicate
    // Can store more than one
    IntentionTagger &cloneIn(CheckerType type,
                             const Circuitry::TracePredicate &checker);

  private:
    std::vector<const Circuitry::TracePredicate *> d_checkers[NONE];
};
}
#endif  // INCLUDED_INTENTIONTAGGER
