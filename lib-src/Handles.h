#ifndef INCLUDED_HANDLES_H
#define INCLUDED_HANDLES_H

namespace ezhic {

struct TypeErasedBase
{
    virtual ~TypeErasedBase() {}
    virtual TypeErasedBase *clone() const = 0;
};

template <typename T>
struct StackValue : public TypeErasedBase
{
    StackValue() {}
    explicit StackValue(const T &v) : d_value(v) {}
    virtual TypeErasedBase *clone() const { return new StackValue(d_value); }
    T d_value;
};

template <typename T>
struct TNullable : public StackValue<T>
{
    using StackValue<T>::d_value;

    TNullable() : d_isNull(true) {}
    explicit TNullable(const T &v) : StackValue<T>(v), d_isNull(false) {}
    template <typename W>  // if W convertible to T
    TNullable(const TNullable<W> &v)
    : StackValue<T>(v.d_value), d_isNull(v.d_isNull)
    {
    }

    template <typename W>  // if W convertible to T
    TNullable &operator=(const TNullable<W> &rhs)
    {
        d_value = rhs.d_value;
        d_isNull = rhs.d_isNull;
        return *this;
    }

    template <typename W>
    TNullable &operator=(const W &v)
    {
        d_value = v;
        d_isNull = false;
        return *this;
    }

    bool operator==(TNullable &rhs) const { return d_value == rhs.d_value; }
    bool isNull() const { return d_isNull; }
    void reset() { d_isNull = true; }
    const T &value() const { return d_value; }
    T &value() { return d_value; }
    virtual TypeErasedBase *clone() const { return new TNullable(d_value); }
  protected:
    bool d_isNull;
};

template <>
struct TNullable<void> : public TypeErasedBase
{
    TNullable() {}
    template <typename W>
    TNullable &operator=(W v)
    {
        return *this;
    }

    bool operator==(TNullable &rhs) const { return true; }
    bool isNull() const { return true; }
    void reset() {}
    virtual TypeErasedBase *clone() const { return new TNullable<void>(); }
};
}
#endif  // EZHIC_HANDLES_H
