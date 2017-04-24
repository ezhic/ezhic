#include <algorithm>

#include "Bundle.h"
#include "Circuitry/CircuitPredicates.h"
#include "EventWrapper.h"
#include "Registry.h"
#include "StreamLogger.h"

using namespace ezhic;

Bundle::Bundle() : d_acceptor(0), d_isRethrow(false)
{
}

Bundle::~Bundle()
{
    if (d_acceptor)
        delete d_acceptor;
}

Bundle::Bundle(const Bundle &rhs)
{
    d_intentionTagger = rhs.d_intentionTagger;
    d_isRethrow = rhs.isRethrow();
    d_acceptor = rhs.d_acceptor ? rhs.d_acceptor->clone() : 0;
}

Bundle &Bundle::operator=(const Bundle &rhs)
{
    if (this != &rhs) {
        d_intentionTagger = rhs.d_intentionTagger;
        d_isRethrow = rhs.isRethrow();
        d_acceptor = rhs.d_acceptor ? rhs.d_acceptor->clone() : 0;
    }
    return *this;
}

Bundle &Bundle::isRethrow(bool flag)
{
    d_isRethrow = flag;
    return *this;
}

bool Bundle::isRethrow() const
{
    return d_isRethrow;
}

Event Bundle::acceptUpdate(const EventWrapper *task) const
{
    return d_acceptor ? d_acceptor->acceptUpdate(task)
                      : AcceptUpdates::acceptUpdate(task);
}

Bundle &Bundle::replaceWithClone(const AcceptUpdates &acceptor)
{
    if (d_acceptor)
        delete d_acceptor;
    d_acceptor = acceptor.clone();
    return *this;
}

Event::Intention Bundle::nextRunIntention(const Label &label) const
{
    return nextRunIntention(ezreg::circuit(label));
}

Event::Intention Bundle::nextRunIntention(Circuit &circuit) const
{
    return d_intentionTagger(circuit);
}

Bundle &Bundle::replaceWithClone(const IntentionTagger &nextRT)
{
    d_intentionTagger = nextRT;
    return *this;
}

Bundle ezhic::make_mbundle(std::ostream &loggingStream,
                           Event::duration_t timeoutThreshold,
                           std::pair<unsigned, unsigned> timeoutsBeforeBreak,
                           std::pair<unsigned, unsigned> exceptionsBeforeBreak,
                           unsigned probeAfterKEvents,
                           unsigned successBeforeMend)
{
    Bundle bundle;
    AcceptorsArray aca;
    aca.cloneIn(StreamLogger(loggingStream));
    aca.cloneIn(TimeoutTagger(timeoutThreshold));
    bundle.replaceWithClone(aca);

    IntentionTagger intentionTagger;
    intentionTagger.cloneIn(
        IntentionTagger::SHOULD_BREAK,
        Circuitry::BreakAfterKTimeoutsInM(timeoutsBeforeBreak.first,
                                          timeoutsBeforeBreak.second));

    intentionTagger.cloneIn(
        IntentionTagger::SHOULD_BREAK,
        Circuitry::BreakAfterKExceptionsInM(exceptionsBeforeBreak.first,
                                            exceptionsBeforeBreak.second));

    intentionTagger.cloneIn(IntentionTagger::SHOULD_PROBE,
                            Circuitry::ProbeAfterK(probeAfterKEvents));
    intentionTagger.cloneIn(IntentionTagger::SHOULD_MEND,
                            Circuitry::RepairAfterKSuccessInM(
                                successBeforeMend, successBeforeMend + 1));

    bundle.replaceWithClone(intentionTagger);
    return bundle;
}