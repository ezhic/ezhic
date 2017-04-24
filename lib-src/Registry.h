#ifndef INCLUDED_REGISTRY
#define INCLUDED_REGISTRY

#include "AcceptUpdates.h"
#include "Bundle.h"
#include "Circuitry/Circuit.h"
#include "Event.h"
#include <map>

/*
 * labels are used to name executions.
 * labels <-> executions is many-to-many
 * execution happens in a context of a bundle
 * execution statuses are stored inside circuit objects
 *
 * a bundle may execute a functor (with arguments)
 * or an arbitrary code between
 * EZ_START(label) and EZ_END
 *
 * registry has a selection of named bundles
 *
*/

namespace ezhic {
class Timer;  // forward

class Registry
{
  public:
    Timer *timer() const { return d_timer; }
    // Own a clone of the given object implementing Timer.
    Registry &cloneInTimer(const Timer &timer);

    // return a ref to bundle associated with <label>
    // if none was given, it is first set to a copy of the default bundle
    const Bundle &bundle(const Label &label);

    // associates <label> with a COPY of the given bundle <b>
    // the history for the label is invalidated and cleared
    // the health state of its circuit is set to GOOD
    const Bundle &writeBundle(const Label &label, const Bundle &b);

    Circuit &circuit(const Label &label);

    void acceptUpdate(const EventWrapper *task = 0);

    static const Label DEFAULT_BUNDLE;
    static Registry &instance();

  private:
    Registry();
    Registry(const Registry &);
    Registry &operator=(const Registry &);

    static Registry *s_registry;
    Timer *d_timer;
    std::map<Label, Bundle> d_bundles;
    std::map<Label, Circuit> d_circuits;
};

namespace ezreg {
// expose registry methods:
Timer *timer();
void cloneInTimer(const Timer &timer);
const Bundle &getBundle(const Label &label);
const Bundle &writeBundle(const Label &label, const Bundle &b);
Circuit &circuit(const Label &label);
}
}

#endif  // EZHIC_REGISTRY_H
