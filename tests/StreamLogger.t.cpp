#include <Event.h>
#include <EventWrapper.h>
#include <StreamLogger.h>
#include <gtest/gtest.h>

using namespace ezhic;

TEST(StreamLoggerTest, null_task_returns_default_event_logs_nothing)
{
    std::stringstream out;
    StreamLogger logger(out);
    Event event = logger.acceptUpdate(0);
    EXPECT_EQ(event, Event());
    EXPECT_EQ(out.str(), "");
}

TEST(StreamLoggerTest, logs_event_from_task_and_returns_copy_of_event)
{
    EventWrapper task;
    Event event;
    event.recordedAtTime(4);
    event.duration(0);
    event.intention(Event::NOT_APPLICABLE);
    event.type(Event::CIRCUIT_BROKE);
    std::stringstream eventStream;
    eventStream << event << '\n';
    task.event(event);

    std::stringstream out;
    StreamLogger streamLogger(out);

    Event outputEvent = streamLogger.acceptUpdate(&task);
    EXPECT_EQ(outputEvent, event);
    EXPECT_EQ(eventStream.str(), out.str());
}
