
#include "gtest/gtest.h"

#include "EventWrapper.h"
#include "Helpers.h"

using namespace ezhic;

TEST(TestTask, DefaultConstructedHas_default_event_ie_Nil)
{
    EventWrapper task;
    EXPECT_EQ(Event(), task.event());
    EXPECT_EQ(Event::NIL, task.event().type());
}

TEST(TestTask, DefaultConstructed_has_no_duration)
{
    EventWrapper task;
    EXPECT_FALSE(task.event().hasKnownDuration());
}

class ThrowingTask : public EventWrapper
{
  public:
    virtual void run() { throw std::runtime_error("test"); }
};

TEST(TestTask, LongTasksHaveNonZeroRuntime)
{
    EventWrapper task;
    MonotonicDummy timer;
    task.timer(&timer);
    task();
    EXPECT_GT(task.event().duration(), 0);
}

TEST(TestTask, ThrowingTaskIsHandled)
{
    ThrowingTask task;
    task();
    EXPECT_EQ(Event::EXCEPTION, task.event().type());
}
