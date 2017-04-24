#include "gtest/gtest.h"
#include <BRunner.h>
#include <StreamLogger.h>

using namespace ezhic;

namespace {
static void dummy()
{
}
}

struct AcceptArrayTest : public ::testing::Test
{
    AcceptArrayTest()
    : aca(), out(), streamLogger(out), task(), taskDuration(1)
    {
        task.event().duration(taskDuration);
    }
    AcceptorsArray aca;
    std::stringstream out;
    StreamLogger streamLogger;
    EventWrapper task;
    Event::duration_t taskDuration;
};

TEST_F(AcceptArrayTest, AcceptorsArray_with_logger_and_timeout_logs_and_tags)
{
    // The stream logger should stream out the event from the task.
    std::stringstream expectedStream;
    expectedStream << task.event() << '\n';
    // First, Add a logger acceptor
    aca.cloneIn(streamLogger);
    // Then add a timeout tagger with shorter lower bound than the task
    // duration
    aca.cloneIn(TimeoutTagger(taskDuration - 1));

    Event outcome = aca.acceptUpdate(&task);

    EXPECT_EQ(out.str(), expectedStream.str());
    EXPECT_EQ(outcome.type(), Event::TIMEOUT);
}

TEST_F(AcceptArrayTest,
       Reversed_AcceptorsArray_with_logger_and_timeout_logs_but_doesnt_tag)
{
    // First, add a timeout tagger with shorter lower bound than the task
    // duration
    aca.cloneIn(TimeoutTagger(taskDuration - 1));
    // Second, add a logger acceptor (any tagging from the timeout will be
    // overwritten by this acceptor)
    std::stringstream expectedStream;
    expectedStream << task.event() << '\n';
    aca.cloneIn(streamLogger);

    Event outcome = aca.acceptUpdate(&task);

    EXPECT_EQ(out.str(), expectedStream.str());
    EXPECT_NE(outcome.type(), Event::TIMEOUT);
}

TEST(TestAcceptorsInBundle, Logger_acceptor_logs_events_through_bundle)
{
    const Label lbl(test_info_->name());
    Bundle bundle = ezreg::getBundle(lbl);
    std::stringstream out;
    StreamLogger streamLogger(out);
    bundle.replaceWithClone(streamLogger);
    ezreg::writeBundle(lbl, bundle);
    BRunner runner(lbl);
    runner.run(dummy);
    EXPECT_NE(out.str(), "");
}