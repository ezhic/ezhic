#ifndef INCLUDED_EVENT_H
#define INCLUDED_EVENT_H

#include <list>
#include <ostream>
#include <string>

namespace ezhic {
class Event
{
  public:
    typedef long duration_t;

    enum Intention
    {
        ROUTINE,         // 0
        PROBE,           // 1
        NO_RUN,          // 2
        NOT_APPLICABLE,  // 3
    };

    enum Type
    {
        SUCCESS,        // 0 got a result in time
        EXCEPTION,      // 1 exception was thrown
        TIMEOUT,        // 2 call timed out
        USER,           // 3 reserved for user data
        INVALID,        // 4 returned result was invalid
        RECOURSE,       // 5 never performed the call, used recourse
        CIRCUIT_BROKE,  // 6
        CIRCUIT_FIXED,  // 7
        NIL             // 8 sentinel, not used
    };
    static const std::string s_typeLabels[Event::NIL + 1];

    Event(Type type = NIL, long recordedAt = -1);
    Type type() const;
    void type(Type t);
    Intention intention() const;
    void intention(Intention r);
    long recordedAtTime() const;
    void recordedAtTime(long sTime);
    long duration() const;
    void duration(long duration);
    bool hasKnownDuration() const;

    const std::string &userData() const;
    void userData(const std::string &uData);

    static const std::string &typeToString(Type type);

  private:
    Type d_type;
    Event::Intention d_intention;
    // we are deliberately fuzzy about time units
    long d_recordedAtTime;
    duration_t d_duration;
    std::string d_userData;
};

bool operator==(const Event &lhs, const Event &rhs);
std::ostream &operator<<(std::ostream &, const Event &);

typedef std::list<Event> EventTrace;
}
#endif  // EZHIC_EVENT_H
