#include "StreamLogger.h"
#include "EventWrapper.h"

// the event at this point may differ from the final event written to
// event-trace
// if, e.g., timeout-checker will run after this call and tag the task as
// timeout
ezhic::Event ezhic::StreamLogger::acceptUpdate(const EventWrapper *task) const
{
    if (task) {
        // d_out << task->label() << ' ';
        d_out << task->event() << '\n';
        return task->event();
    }
    return Event();
}
