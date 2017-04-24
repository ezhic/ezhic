#ifndef INCLUDED_TRAITS
#define INCLUDED_TRAITS

#include "Handles.h"

namespace ezhic {

template <typename T>
struct QualifiedType
{
    typedef T type;
};

template <>
template <typename T>
struct QualifiedType<const T>
{
    typedef const T type;
};

template <>
template <typename T>
struct QualifiedType<T &>
{
    typedef T &type;
};

template <>
template <typename T>
struct QualifiedType<const T &>
{
    typedef const T &type;
};

template <>
template <typename T>
struct QualifiedType<T *>
{
    typedef T *type;
};

template <>
template <typename T>
struct QualifiedType<const T *>
{
    typedef const T *type;
};

// We are stricter with return types: we enforce return of Nullable values

// Nullary functor
template <typename Callable>
struct CallableTraits0
{
    typedef TNullable<typename Callable::result_type> result_type;
};

// Nullary free function
template <>
template <typename R>
struct CallableTraits0<R (*)()>
{
    typedef TNullable<R> result_type;
};

// Nullary member function
template <>
template <typename Class, typename R>
struct CallableTraits0<R (Class::*)()>
{
    typedef TNullable<R> result_type;
    typedef Class &obj_ref;
};

// Nullary const member function
template <>
template <typename Class, typename R>
struct CallableTraits0<R (Class::*)() const>
{
    typedef TNullable<R> result_type;
    typedef const Class obj_ref;
};

// Unary functor
template <typename Callable>
struct CallableTraits1
{
    typedef TNullable<typename Callable::result_type> result_type;
    typedef typename Callable::argument_type argument_type;
};

// Unary free function
template <>
template <typename R, typename Arg>
struct CallableTraits1<R (*)(Arg)>
{
    typedef TNullable<R> result_type;
    typedef typename QualifiedType<Arg>::type argument_type;
};

// Unary member function
template <>
template <typename Class, typename R, typename Arg>
struct CallableTraits1<R (Class::*)(Arg)>
{
    typedef TNullable<R> result_type;
    typedef Class &obj_ref;
    typedef typename QualifiedType<Arg>::type argument_type;
};

// Unary const member function
template <>
template <typename Class, typename R, typename Arg>
struct CallableTraits1<R (Class::*)(Arg) const>
{
    typedef TNullable<R> result_type;
    typedef const Class obj_ref;
    typedef typename QualifiedType<Arg>::type argument_type;
};

// Binary functor
template <typename Callable>
struct CallableTraits2
{
    typedef TNullable<typename Callable::result_type> result_type;
    typedef typename Callable::first_argument_type first_argument_type;
    typedef typename Callable::second_argument_type second_argument_type;
};

// Binary free function
template <>
template <typename R, typename Arg1, typename Arg2>
struct CallableTraits2<R (*)(Arg1, Arg2)>
{
    typedef TNullable<R> result_type;
    typedef typename QualifiedType<Arg1>::type first_argument_type;
    typedef typename QualifiedType<Arg2>::type second_argument_type;
};

// Binary member function
template <>
template <typename Class, typename R, typename Arg1, typename Arg2>
struct CallableTraits2<R (Class::*)(Arg1, Arg2)>
{
    typedef TNullable<R> result_type;
    typedef Class &obj_ref;
    typedef typename QualifiedType<Arg1>::type first_argument_type;
    typedef typename QualifiedType<Arg2>::type second_argument_type;
};

// Binary const member function
template <>
template <typename Class, typename R, typename Arg1, typename Arg2>
struct CallableTraits2<R (Class::*)(Arg1, Arg2) const>
{
    typedef TNullable<R> result_type;
    typedef const Class obj_ref;
    typedef typename QualifiedType<Arg1>::type first_argument_type;
    typedef typename QualifiedType<Arg2>::type second_argument_type;
};
}

#endif  // EZHIC_TRAITS_H
