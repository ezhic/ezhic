#ifndef INCLUDED_BRUNNER_H
#define INCLUDED_BRUNNER_H

#include "Bundle.h"
#include "Circuitry/Circuit.h"
#include "Handles.h"
#include "Timer.h"
#include "Traits.h"

namespace ezhic {
struct BRunner
{
    // will use global registry to initialize
    explicit BRunner(const Label &label);
    // will operate on user-owned circuit, bundle, timer
    BRunner(Circuit &c, const Bundle &b, Timer *t);
    virtual ~BRunner() {}
    const Label d_label;
    Circuit &d_circuit;
    const Bundle &d_bundle;
    const Timer *d_timer;

    // allow intercepting type-erased result for post-run in derived classes
    // we pledge to call it for each run (after updating acceptors)
    virtual void postRun(TypeErasedBase &result) {}
    //======== 0 arguments ===================================
    template <typename Callable>
    typename CallableTraits0<Callable>::result_type run(Callable callable);

    template <typename Callable>
    typename CallableTraits0<Callable>::result_type
    run_m(typename CallableTraits0<Callable>::obj_ref obj, Callable callable);

    //======== 1 argument ===================================
    template <typename Callable>
    typename CallableTraits1<Callable>::result_type
    run(Callable callable, typename CallableTraits1<Callable>::argument_type);

    template <typename Callable>
    typename CallableTraits1<Callable>::result_type
    run_m(typename CallableTraits1<Callable>::obj_ref obj,
          Callable callable,
          typename CallableTraits1<Callable>::argument_type);

    //======== 2 arguments ===================================
    template <typename Callable>
    typename CallableTraits2<Callable>::result_type
    run(Callable callable,
        typename CallableTraits2<Callable>::first_argument_type,
        typename CallableTraits2<Callable>::second_argument_type);

    template <typename Callable>
    typename CallableTraits2<Callable>::result_type
    run_m(typename CallableTraits2<Callable>::obj_ref obj,
          Callable callable,
          typename CallableTraits2<Callable>::first_argument_type,
          typename CallableTraits2<Callable>::second_argument_type);
};
}

#include "BRunner_IMP.h"

#endif  // EZHIC_BRUNNER_H
