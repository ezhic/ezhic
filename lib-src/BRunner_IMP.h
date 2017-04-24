#ifndef INCLUDED_BUNDLE_IMP_H
#define INCLUDED_BUNDLE_IMP_H

#include "EventWrapper.h"
#include "Registry.h"
#include <algorithm>

#define __EZ_START()                                                          \
    Event::Intention intention =                                              \
        _local_bundle_for_scope.nextRunIntention(_local_circuit_for_scope);   \
    _dummy_task_for_scope.event().intention(intention);                       \
    bool shouldRun = intention != Event::NO_RUN;                              \
    if (shouldRun) {                                                          \
        try {                                                                 \
            _dummy_task_for_scope.start();

#define EZ_START(__execlabel)                                                 \
    {                                                                         \
        Registry &_local_registry_ref = Registry::instance();                 \
        const Bundle &_local_bundle_for_scope =                               \
            _local_registry_ref.bundle(__execlabel);                          \
        EventWrapper _dummy_task_for_scope(__execlabel);                      \
        _dummy_task_for_scope.timer(_local_registry_ref.timer());             \
        Circuit &_local_circuit_for_scope =                                   \
            _local_registry_ref.circuit(__execlabel);                         \
    __EZ_START()

#define EZ_END()                                                              \
    _dummy_task_for_scope.done();                                             \
    }                                                                         \
    catch (...)                                                               \
    {                                                                         \
        _dummy_task_for_scope.event().type(Event::EXCEPTION);                 \
        if (_local_bundle_for_scope.isRethrow()) {                            \
            _dummy_task_for_scope.event(_local_bundle_for_scope.acceptUpdate( \
                &_dummy_task_for_scope));                                     \
            _local_circuit_for_scope.acceptUpdate(&_dummy_task_for_scope);    \
            throw;                                                            \
        }                                                                     \
    } /* end try-catch */                                                     \
    } /* end should run */                                                    \
    else { _dummy_task_for_scope.putTimestamp(); }                            \
    _dummy_task_for_scope.event(                                              \
        _local_bundle_for_scope.acceptUpdate(&_dummy_task_for_scope));        \
    _local_circuit_for_scope.acceptUpdate(&_dummy_task_for_scope);            \
    }

namespace ezhic {

namespace b_assign_void_trick {
template <typename T>
inline T &passthrough(T &v)
{
    return v;
}

template <typename T>
inline void operator,(TNullable<T> &dest, const T &v)
{
    dest = v;
}
}

#define VariadicCallInstrumentator(FUNC, ...)                                 \
    R result;                                                                 \
    {                                                                         \
        EventWrapper _dummy_task_for_scope(d_label);                          \
        _dummy_task_for_scope.timer(d_timer);                                 \
        Circuit &_local_circuit_for_scope = d_circuit;                        \
        const Bundle &_local_bundle_for_scope = d_bundle;                     \
        __EZ_START()                                                          \
        using namespace b_assign_void_trick;                                  \
        passthrough(result), FUNC(__VA_ARGS__);                               \
        EZ_END();                                                             \
        postRun(result);                                                      \
        return result;

template <typename Callable>
typename CallableTraits0<Callable>::result_type BRunner::run(Callable callable)
{
    typedef typename CallableTraits0<Callable>::result_type R;
    VariadicCallInstrumentator(callable);
}

template <typename Callable>
typename CallableTraits0<Callable>::result_type
BRunner::run_m(typename CallableTraits0<Callable>::obj_ref obj,
               Callable callable)
{
    typedef typename CallableTraits0<Callable>::result_type R;
    VariadicCallInstrumentator(((obj).*(callable)));
}

template <typename Callable>
typename CallableTraits1<Callable>::result_type
BRunner::run(Callable callable,
             typename CallableTraits1<Callable>::argument_type arg)
{
    typedef typename CallableTraits1<Callable>::result_type R;
    VariadicCallInstrumentator(callable, arg);
}

template <typename Callable>
typename CallableTraits1<Callable>::result_type
BRunner::run_m(typename CallableTraits1<Callable>::obj_ref obj,
               Callable callable,
               typename CallableTraits1<Callable>::argument_type arg)
{
    typedef typename CallableTraits1<Callable>::result_type R;
    VariadicCallInstrumentator(((obj).*(callable)), arg);
}

template <typename Callable>
typename CallableTraits2<Callable>::result_type
BRunner::run(Callable callable,
             typename CallableTraits2<Callable>::first_argument_type arg1,
             typename CallableTraits2<Callable>::second_argument_type arg2)
{
    typedef typename CallableTraits2<Callable>::result_type R;
    VariadicCallInstrumentator(callable, arg1, arg2);
}

template <typename Callable>
typename CallableTraits2<Callable>::result_type
BRunner::run_m(typename CallableTraits2<Callable>::obj_ref obj,
               Callable callable,
               typename CallableTraits2<Callable>::first_argument_type arg1,
               typename CallableTraits2<Callable>::second_argument_type arg2)
{
    typedef typename CallableTraits2<Callable>::result_type R;
    VariadicCallInstrumentator(((obj).*(callable)), arg1, arg2);
}
}

#endif  // EZHIC_BUNDLE_IMP_H
