#ifndef INCLUDED_CIRCUIT
#define INCLUDED_CIRCUIT

#include "AcceptUpdates.h"
#include "Event.h"

#include <string>

namespace ezhic {

typedef std::string Label;

// Stateful: holds the event trace and health status for a label
class Circuit
{
  public:
    enum Health
    {
        BAD,
        GOOD
    };

    Circuit();
    void health(Health healthUpdate);
    const Health health() const;
    const EventTrace &eventTrace() const;
    void clear(Health newHealth =
                   GOOD);  // zap the circuit: clear trace, reset health GOOD

    virtual void acceptUpdate(const EventWrapper *task = 0);
    virtual void acceptUpdate(const EventWrapper &task);
    virtual void acceptUpdate(const Event &event);
    // the next two will lack proper timestamps:
    virtual void acceptUpdate(const Event::Type &event);
    virtual void acceptUpdate(const std::string &userData);

  private:
    EventTrace d_events;
    Health d_status;
};
}

#endif  // INCLUDED_CIRCUITHISTORY
