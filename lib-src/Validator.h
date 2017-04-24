#ifndef INCLUDED_VALIDATOR
#define INCLUDED_VALIDATOR

#include "BRunner.h"
#include "Handles.h"
#include "Registry.h"
#include "Traits.h"

namespace ezhic {

// Two examples of overriding postRun to intercept the result.
// Both update the event trace according to their logic.
// We can use this signal to determine if the circuit should be broken.

// The first validates the result.
// The seconds sets a sane value if none was returned by the call.

template <typename T>
class FixedValueValidator : public BRunner
{
  public:
    explicit FixedValueValidator(const Label &label, const T &value)
    : BRunner(label), d_value(value)
    {
    }

    virtual void postRun(TypeErasedBase &result)
    {
        TNullable<T> &res = dynamic_cast<TNullable<T> &>(result);
        // arguably, an empty result is not what we wanted
        if (res.isNull() || d_value != res.value())
            ezreg::circuit(d_label).acceptUpdate(Event::INVALID);
    }

    const T d_value;
};

template <typename T>
class FixedValueRecourse : public BRunner
{
  public:
    explicit FixedValueRecourse(const Label &label, const T &value)
    : BRunner(label), d_value(value)
    {
    }

    virtual void postRun(TypeErasedBase &result)
    {
        TNullable<T> &res = dynamic_cast<TNullable<T> &>(result);
        if (res.isNull()) {
            res = d_value;
            ezreg::circuit(d_label).acceptUpdate(Event::RECOURSE);
        }
    }

    const T d_value;
};
}

#endif  // EZHIC_VALIDATOR_H
