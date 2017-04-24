#include "EventWrapper.h"
#include "Timer.h"

using namespace ezhic;

EventWrapper::EventWrapper(const std::string &label)
: d_label(label), d_timer(0)
{
}

EventWrapper::EventWrapper() : d_timer(0)
{
}

EventWrapper::~EventWrapper()
{
}

void EventWrapper::timer(const Timer *timer)
{
    d_timer = timer;
}

void EventWrapper::start()
{
    if (d_timer)
        d_event.recordedAtTime(d_timer->now());
}

void EventWrapper::putTimestamp()
{
    if (d_timer)
        d_event.recordedAtTime(d_timer->now());
}

void EventWrapper::done()
{
    d_event.type(Event::SUCCESS);
    d_event.duration(d_timer ? long(d_timer->now() - d_event.recordedAtTime())
                             : -1);
}

void EventWrapper::operator()()
{  // instrument virtual run
    try {
        start();
        run();
        done();
    } catch (const std::exception &e) {
        d_event.type(Event::EXCEPTION);
    } catch (...) {
        d_event.type(Event::EXCEPTION);
    }
}
