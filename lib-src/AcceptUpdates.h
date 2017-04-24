#ifndef INCLUDED_ACCEPTINTERFACE
#define INCLUDED_ACCEPTINTERFACE

#include "Event.h"
#include <vector>

namespace ezhic {
class EventWrapper;  // forward

class AcceptUpdates
{
  public:
    virtual ~AcceptUpdates() {}
    virtual Event acceptUpdate(const EventWrapper *task) const;
    virtual AcceptUpdates *clone() const = 0;
};

template <typename Derived>
class AcceptUpdatesCloneable : public AcceptUpdates
{
  public:
    virtual AcceptUpdates *clone() const
    {
        return new Derived(static_cast<Derived const &>(*this));
    }
};

class TimeoutTagger : public AcceptUpdatesCloneable<TimeoutTagger>
{
  public:
    explicit TimeoutTagger(Event::duration_t threshold);
    Event acceptUpdate(const EventWrapper *task) const;
    const Event::duration_t d_threshold;
};

// will run all acceptors and return the event of the LAST
class AcceptorsArray : public AcceptUpdatesCloneable<AcceptorsArray>
{
  public:
    AcceptorsArray() {}
    ~AcceptorsArray();
    AcceptorsArray(const AcceptorsArray &);

    // Own a clone of the given object implementing AcceptUpdates
    // Can store more than one
    void cloneIn(const AcceptUpdates &acpt);
    Event acceptUpdate(const EventWrapper *task) const;

  protected:
    AcceptorsArray &operator=(const AcceptorsArray &);
    std::vector<AcceptUpdates *> d_acceptors;
};
}
#endif  // EZHIC_ACCEPTINTERFACE_H
