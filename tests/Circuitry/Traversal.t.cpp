#include "gtest/gtest.h"

#include "Circuitry/Travellers.h"

using namespace ezhic;

struct TestTravellers : public ::testing::Test
{
    typedef std::vector<int> V;
    typedef V::const_iterator vit;
    typedef Traversal::IteratorWrapper<vit> Traveller;
    typedef Traversal::IteratorWrapper<V::const_reverse_iterator> RIt;
    V v;
};

TEST_F(TestTravellers, Iterator_wrapper_traverses_same_values)
{
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    Traveller trv(v.begin(), v.end());
    vit it = v.begin();
    while (trv) {
        EXPECT_EQ(*trv, *it);
        ++it;
        ++trv;
    }

    std::vector<int>::const_reverse_iterator vrit = v.rbegin();
    RIt rtrv(v.rbegin(), v.rend());
    while (rtrv) {
        EXPECT_EQ(*rtrv, *vrit);
        ++vrit;
        ++rtrv;
    }
}

TEST_F(TestTravellers, AtMostNTraveller_is_false_when_range_empty)
{
    Traversal::AtMostNTraveller<Traveller> trv(Traveller(v.begin(), v.end()),
                                               1);
    EXPECT_FALSE(trv);
}

TEST_F(TestTravellers, AtMostNTraveller_is_false_when_n_is_nil)
{
    v.push_back(1);
    v.push_back(2);

    Traversal::AtMostNTraveller<Traveller> trv(Traveller(v.begin(), v.end()),
                                               0);
    EXPECT_FALSE(trv);
}

TEST_F(TestTravellers, AtMostNTraveller_travels_exactly_n_for_large_range)
{
    v.push_back(1);
    v.push_back(2);

    Traversal::AtMostNTraveller<Traveller> trv(Traveller(v.begin(), v.end()),
                                               1);
    EXPECT_TRUE(trv);
    EXPECT_EQ(*trv, 1);
    ++trv;
    EXPECT_FALSE(trv);
}

TEST_F(TestTravellers,
       AtMostNTraveller_travels_all_range_but_no_more_for_big_n)
{
    v.push_back(1);
    v.push_back(2);

    Traversal::AtMostNTraveller<Traveller> trv(Traveller(v.begin(), v.end()),
                                               4);
    EXPECT_EQ(2u, Traversal::range_size(trv));
    EXPECT_TRUE(trv);
    EXPECT_EQ(*trv, 1);
    ++trv;
    EXPECT_TRUE(trv);
    EXPECT_EQ(*trv, 2);
    ++trv;
    EXPECT_FALSE(trv);
}
namespace {
struct predicate
{
    bool operator()(int v) { return static_cast<bool>(v & 1); }
};
}

TEST_F(TestTravellers, Filter_traverses_only_matches)
{
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    Traveller tr(v.begin(), v.end());
    Traversal::Filter<Traveller, predicate> trv =
        Traversal::make_filter_traveller(tr, predicate());

    EXPECT_EQ(1, *trv);
    ++trv;
    EXPECT_EQ(3, *trv);
    ++trv;
    EXPECT_FALSE(trv);
}

TEST_F(TestTravellers, Filter_with_AtMostN_traverses_only_N)
{
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    v.push_back(5);

    Traveller tr(v.begin(), v.end());
    typedef Traversal::Filter<Traveller, predicate> TFilter;
    TFilter trv = Traversal::make_filter_traveller(tr, predicate());
    Traversal::AtMostNTraveller<TFilter> atMost(trv, 2);

    EXPECT_EQ(2u, Traversal::range_size(atMost));
}

namespace {
struct counter
{
    counter() : cnt(0) {}
    void operator()(int) { ++cnt; }
    size_t cnt;
};
}

TEST_F(TestTravellers, for_each_applied_to_all)
{
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);

    Traveller trv(v.begin(), v.end());
    counter c = Traversal::apply_to_all(trv, counter());

    EXPECT_EQ(c.cnt, v.size());
    EXPECT_EQ(c.cnt, Traversal::range_size(trv));
}