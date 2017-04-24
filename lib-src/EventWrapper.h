#ifndef INCLUDED_EVENT_WRAPPER
#define INCLUDED_EVENT_WRAPPER

#include "Event.h"

namespace ezhic {

typedef std::string Label;
class Timer;

class EventWrapper
{
  public:
    EventWrapper();
    explicit EventWrapper(const std::string &label);
    virtual ~EventWrapper();

    void operator()();  // non virtual, calling virtual protected methods

    virtual void start();
    virtual void done();

    void timer(const Timer *timer);

    const Label &label() const { return d_label; }
    void label(const Label &label) { d_label = label; }
    const Event &event() const { return d_event; }
    Event &event() { return d_event; }
    void event(const Event &event) { d_event = event; }
    void putTimestamp();

  protected:
    virtual void run() {}
    Label d_label;
    Event d_event;
    const Timer *d_timer;

  private:
    EventWrapper(const EventWrapper &);
    EventWrapper &operator=(const EventWrapper &);
};

}  // end namespace

#endif  // EZHIC_TASK_H
