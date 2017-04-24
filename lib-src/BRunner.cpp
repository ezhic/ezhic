#include "BRunner.h"
#include "Registry.h"

namespace ezhic {

BRunner::BRunner(const Label &label)
: d_label(label)
, d_circuit(ezreg::circuit(label))
, d_bundle(ezreg::getBundle(label))
, d_timer(ezreg::timer())
{
}

// will operate on user-owned circuit, bundle, timer
BRunner::BRunner(Circuit &c, const Bundle &b, Timer *t)
: d_label(""), d_circuit(c), d_bundle(b), d_timer(t)
{
}
}