#include "AcceptUpdates.h"
#include "EventWrapper.h"

namespace ezhic {

Event AcceptUpdates::acceptUpdate(const EventWrapper *task) const
{
    return task ? task->event() : Event();
}

TimeoutTagger::TimeoutTagger(Event::duration_t threshold)
: d_threshold(threshold)
{
}

Event TimeoutTagger::acceptUpdate(const EventWrapper *task) const
{
    Event event = AcceptUpdates::acceptUpdate(task);
    if (event.hasKnownDuration() && event.duration() > d_threshold)
        event.type(Event::TIMEOUT);
    return event;
}

AcceptorsArray::AcceptorsArray(const AcceptorsArray &rhs)
{
    for (std::vector<AcceptUpdates *>::const_iterator it =
             rhs.d_acceptors.begin();
         it != rhs.d_acceptors.end();
         ++it) {
        d_acceptors.push_back((*it)->clone());
    }
}

void AcceptorsArray::cloneIn(const AcceptUpdates &acpt)
{
    d_acceptors.push_back(acpt.clone());
}

Event AcceptorsArray::acceptUpdate(const EventWrapper *task) const
{
    Event event = AcceptUpdates::acceptUpdate(task);
    for (std::vector<AcceptUpdates *>::const_iterator it = d_acceptors.begin();
         it != d_acceptors.end();
         ++it) {
        event = (*it)->acceptUpdate(task);
    }
    return event;
}

AcceptorsArray::~AcceptorsArray()
{
    for (std::vector<AcceptUpdates *>::iterator it = d_acceptors.begin();
         it != d_acceptors.end();
         ++it) {
        delete *it;
    }
}
}