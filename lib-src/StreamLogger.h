#ifndef INCLUDED_LOGGER
#define INCLUDED_LOGGER

#include "AcceptUpdates.h"
#include <ostream>

namespace ezhic {
class StreamLogger : public AcceptUpdatesCloneable<StreamLogger>
{
  public:
    explicit StreamLogger(std::ostream &out) : d_out(out) {}
    Event acceptUpdate(const EventWrapper *task) const;

  protected:
    std::ostream &d_out;
};
}

#endif  // EZHIC_LOGGER_H
