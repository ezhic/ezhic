#ifndef INCLUDED_BUNDLE
#define INCLUDED_BUNDLE

#include <Circuitry/IntentionTagger.h>
#include <string>
#include <vector>

namespace ezhic {
class AcceptUpdates;    // forward
class IntentionTagger;  // forward
class Registry;         // forward
class EventWrapper;     // forward

typedef std::string Label;

class Bundle : public AcceptUpdatesCloneable<Bundle>
{
  public:
    Bundle();
    ~Bundle();
    Bundle(const Bundle &);
    Bundle &operator=(const Bundle &);

    // Own a COPY of the given IntentionTagger object.
    Bundle &replaceWithClone(const IntentionTagger &nextRT);

    // Own a clone of the given object implementing AcceptUpdates.
    Bundle &replaceWithClone(const AcceptUpdates &acceptor);
    Bundle &isRethrow(bool flag);
    bool isRethrow() const;

    Event acceptUpdate(const EventWrapper *task) const;
    Event::Intention nextRunIntention(const Label &label) const;
    Event::Intention nextRunIntention(Circuit &circuit) const;

  protected:
    IntentionTagger d_intentionTagger;
    AcceptUpdates *d_acceptor;
    bool d_isRethrow;
};

Bundle make_mbundle(std::ostream &loggingStream,
                    Event::duration_t timeoutThreshold,
                    std::pair<unsigned, unsigned> timeoutsBeforeBreak,
                    std::pair<unsigned, unsigned> exceptionsBeforeBreak,
                    unsigned probeAfterKEvents,
                    unsigned successBeforeMend);
}

#endif  // EZHIC_BUNDLE_H
