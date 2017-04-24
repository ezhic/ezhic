#ifndef INCLUDED_TRAVELLERS
#define INCLUDED_TRAVELLERS

#include <iterator>

namespace ezhic {
namespace Traversal {

// And be one traveler, long I stood
// And looked down one as far as I could
//                      R. Frost

// uses user-defined conversion to bool to stop,
// can't be used with std::algorithms
// and we support only const versions

template <typename Traveller>
class BasicTraversal
{
  public:
    BasicTraversal(Traveller it) : d_it(it) {}
    typedef typename Traveller::value_type value_type;
    const value_type &operator*() const { return *d_it; }
    operator bool() const { return (d_it); }
  protected:
    Traveller d_it;
};

template <typename Traveller, typename Predicate>
class Filter : public BasicTraversal<Traveller>
{
  public:
    using BasicTraversal<Traveller>::d_it;
    Filter(Traveller it, Predicate predicate)
    : BasicTraversal<Traveller>(it), d_predicate(predicate)
    {
    }
    void operator++()
    {
        do {
            ++d_it;
        } while ((d_it) && !d_predicate(*d_it));
    }

  protected:
    Predicate d_predicate;
};

template <typename T, typename P>
Filter<T, P> make_filter_traveller(T t, P p)
{
    return Filter<T, P>(t, p);
};

template <typename Traveller>
class AtMostNTraveller : public BasicTraversal<Traveller>
{
    using BasicTraversal<Traveller>::d_it;

  public:
    AtMostNTraveller(Traveller it, size_t N)
    : BasicTraversal<Traveller>(it), d_count(0), d_N(N)
    {
    }
    void operator++()
    {
        ++d_it;
        ++d_count;
    }
    operator bool() const
    {
        return (d_it) && (d_count < d_N);
    }  // this is hiding Basic conversion operator
    size_t count() const { return d_count; }  // get the actual count
  protected:
    size_t d_count;
    const size_t d_N;
};

// converts from [begin,end) pair to traveller object
template <typename Iterator>
struct IteratorWrapper
{
    typedef typename std::iterator_traits<Iterator>::value_type value_type;
    IteratorWrapper(Iterator begin, Iterator end) : d_begin(begin), d_end(end)
    {
    }
    void operator++() { ++d_begin; }
    const value_type &operator*() const { return *d_begin; }
    operator bool() const { return d_begin != d_end; }
    Iterator d_begin;
    Iterator d_end;
};

template <typename Traveller>
size_t range_size(Traveller trv)
{
    size_t cnt = 0;
    while (trv) {
        ++cnt;
        ++trv;
    }
    return cnt;
}

template <typename Traveller, typename F>
F apply_to_all(Traveller trv, F functor)
{
    while (trv) {
        functor(*trv);
        ++trv;
    }
    return functor;
};
}
}
#endif  // EZHIC_TRAVELLERS_H
