#include "Circuit.h"
#include "EventWrapper.h"

namespace ezhic {
Circuit::Circuit() : d_events(), d_status(GOOD)
{
}

void Circuit::health(Health healthUpdate)
{
    if (d_status == healthUpdate)
        return;
    acceptUpdate(d_status == BAD ? Event::CIRCUIT_FIXED
                                 : Event::CIRCUIT_BROKE);
    d_status = healthUpdate;
}

void Circuit::clear(Health newHealth)
{
    d_events.clear();
    health(newHealth);
}

const Circuit::Health ezhic::Circuit::health() const
{
    return d_status;
}

const EventTrace &ezhic::Circuit::eventTrace() const
{
    return d_events;
}

void Circuit::acceptUpdate(const EventWrapper &task)
{
    d_events.push_front(task.event());
}

void Circuit::acceptUpdate(const ezhic::EventWrapper *task)
{
    if (task)
        acceptUpdate(*task);
}

void Circuit::acceptUpdate(const Event &event)
{
    d_events.push_front(event);
}

void Circuit::acceptUpdate(const Event::Type &event)
{
    acceptUpdate(Event(event));
}

void Circuit::acceptUpdate(const std::string &userData)
{
    Event e(Event::USER);
    e.userData(userData);
    d_events.push_front(e);
}
}