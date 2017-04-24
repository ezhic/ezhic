#include "gtest/gtest.h"
#include <BRunner.h>
#include <Circuitry.h>
#include <Event.h>
#include <memory>

#include <Validator.h>

using namespace ezhic;

namespace {

static char funcptr()
{
    return 17;
}

class AlwaysBreak : public Circuitry::TracePredicateCloneable<AlwaysBreak>
{
    virtual bool operator()(const EventTrace &history) const { return true; }
};

class NeverBreak : public Circuitry::TracePredicateCloneable<NeverBreak>
{
    virtual bool operator()(const EventTrace &history) const { return false; }
};

class BreakIfLenGT1 : public Circuitry::TracePredicateCloneable<BreakIfLenGT1>
{
    virtual bool operator()(const EventTrace &history) const
    {
        return history.size() > 1;
    }
};
}

TEST(IntentionTagger, AlwaysBreakNeverRuns)
{
    IntentionTagger intentionTagger;
    intentionTagger.cloneIn(IntentionTagger::SHOULD_BREAK, AlwaysBreak());

    Label lbl(test_info_->name());
    Bundle bundle;
    bundle.replaceWithClone(intentionTagger);

    EXPECT_EQ(Event::NO_RUN, bundle.nextRunIntention(lbl));
}

TEST(IntentionTagger, NeverBreakAlwaysRuns)
{
    Label lbl(test_info_->name());
    Bundle bundle;
    IntentionTagger intentionTagger;
    intentionTagger.cloneIn(IntentionTagger::SHOULD_BREAK, NeverBreak());
    bundle.replaceWithClone(intentionTagger);

    EXPECT_EQ(Event::ROUTINE, bundle.nextRunIntention(lbl));
}

TEST(IntentionTagger, label_event_trace_passed_to_intention_tagger)
{
    Label lbl(test_info_->name());
    Circuit &lblCircuit = ezreg::circuit(lbl);
    lblCircuit.acceptUpdate(EventWrapper());

    Bundle bundle;
    IntentionTagger intentionTagger;
    intentionTagger.cloneIn(IntentionTagger::SHOULD_BREAK, BreakIfLenGT1());
    bundle.replaceWithClone(intentionTagger);

    EXPECT_EQ(Event::ROUTINE, bundle.nextRunIntention(lbl));
    lblCircuit.acceptUpdate(EventWrapper());
    EXPECT_EQ(Event::NO_RUN, bundle.nextRunIntention(lbl));
    Label newLabel(std::string(test_info_->name()) + "otherLabel");
    EXPECT_EQ(Event::ROUTINE, bundle.nextRunIntention(newLabel));
}

TEST(TestMBundle, MetaBundlePopulates)
{
    Label lbl(test_info_->name());
    std::stringstream sout;
    Bundle bundle = make_mbundle(
        sout, 100u, std::make_pair(2u, 4u), std::make_pair(2u, 4u), 2u, 2u);
    ezreg::writeBundle(lbl, bundle);
    TNullable<char> result = BRunner(lbl).run(&funcptr);
    EXPECT_EQ(funcptr(), result.value());
}
