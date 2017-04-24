#include "Event.h"

namespace ezhic {
// static
const std::string Event::s_typeLabels[Event::NIL + 1];
// {
//    std::string("SUCCESS"),
//    std::string("EXCEPTION"),
//    std::string("RECOURSE"),
//    std::string("TIMEOUT"),
//    std::string("INVALID"),
//    std::string("CIRCUIT_BROKE"),
//    std::string("CIRCUIT_PROBED"),
//    std::string("CIRCUIT_FIXED"),
//    std::string("USER"),
//    std::string("NIL")};

Event::Type Event::type() const
{
    return d_type;
}

void Event::type(Event::Type t)
{
    d_type = t;
}

Event::Event(Event::Type type, long recordedAt)
: d_type(type)
, d_intention(Event::ROUTINE)
, d_recordedAtTime(recordedAt)
, d_duration(-1)
{
}

bool operator==(const Event &lhs, const Event &rhs)
{
    return lhs.type() == rhs.type() && lhs.intention() == rhs.intention() &&
           lhs.recordedAtTime() == rhs.recordedAtTime() &&
           lhs.duration() == rhs.duration() &&
           lhs.userData() == rhs.userData();
}

std::ostream &operator<<(std::ostream &out, const Event &event)
{
    out << '{' << "\"type\" : " << event.type()
        << ", \"intention\" : " << event.intention()
        << ", \"recordedAtTime\" : " << event.recordedAtTime()
        << ", \"duration\" : " << event.duration();
    if (!event.userData().empty())
        out << ", \"user\" : \"" << event.userData() << '"';
    return out << "}";
}

Event::Intention Event::intention() const
{
    return d_intention;
}
void Event::intention(Intention r)
{
    d_intention = r;
}

long Event::duration() const
{
    return d_duration;
}

void Event::recordedAtTime(long sTime)
{
    d_recordedAtTime = sTime;
}

// depending on timer accuracy, default duration
// could be zero and > could be used instead of >=
bool Event::hasKnownDuration() const
{
    return d_duration >= 0;
}

long Event::recordedAtTime() const
{
    return d_recordedAtTime;
}

void Event::duration(long duration)
{
    d_duration = duration;
}

const std::string &Event::userData() const
{
    return d_userData;
}

void Event::userData(const std::string &uData)
{
    d_userData = uData;
}

// static
const std::string &Event::typeToString(Type type)
{
    return (type < Event::NIL) ? s_typeLabels[type] : s_typeLabels[Event::NIL];
}
}