#include "gtest/gtest.h"
#include <Circuitry/CircuitPredicates.h>

using namespace ezhic;

namespace {

struct AlwaysTruePredicate
{
    bool operator()(const Event &) { return true; }
};
}

using namespace Circuitry;

TEST(TestPercentsToSize, ZeroPercentsReturnZero)
{
    const size_t zero(0);
    EXPECT_EQ(zero, CheckMoreThanKInM::percentageToSize(10, -1, 0));
    EXPECT_EQ(zero, CheckMoreThanKInM::percentageToSize(10, 5, 0));
    EXPECT_EQ(zero, CheckMoreThanKInM::percentageToSize(10, 20, 0));
}

TEST(TestPercentsToSize, FiftyReturnsHalfOfMIfPositive)
{
    EXPECT_EQ(3u, CheckMoreThanKInM::percentageToSize(10, 7, 50));
    EXPECT_EQ(4u, CheckMoreThanKInM::percentageToSize(10, 8, 50));
    EXPECT_EQ(0u, CheckMoreThanKInM::percentageToSize(10, 0, 50));
}

TEST(TestPercentsToSize, TwentyFiveReturnsQuarterOfSizeIfMIsAll)
{
    EXPECT_EQ(2u, CheckMoreThanKInM::percentageToSize(10, ALL_HISTORY, 25));
    EXPECT_EQ(3u, CheckMoreThanKInM::percentageToSize(13, ALL_HISTORY, 25));
    EXPECT_EQ(0u, CheckMoreThanKInM::percentageToSize(1, ALL_HISTORY, 25));
}

TEST(TestPercentsToSize, TwentyReturnsFifthOfSizeIfMIsLarger)
{
    EXPECT_EQ(5u, CheckMoreThanKInM::percentageToSize(25, 99, 20));
    EXPECT_EQ(2u, CheckMoreThanKInM::percentageToSize(13, 99, 20));
    EXPECT_EQ(0u, CheckMoreThanKInM::percentageToSize(1, 99, 20));
}

TEST(TestMoreThanKInLastM, returnsFalseForEmptyTrace)
{
    EventTrace empty;
    //(size_t k, long m, const EventTrace &history, Predicate predicate)
    EXPECT_FALSE(moreThanKInLastM(0, 10, empty, AlwaysTruePredicate()));
    EXPECT_FALSE(moreThanKInLastM(0, -1, empty, AlwaysTruePredicate()));
    EXPECT_FALSE(moreThanKInLastM(1, 10, empty, AlwaysTruePredicate()));
    EXPECT_FALSE(moreThanKInLastM(1, -1, empty, AlwaysTruePredicate()));
}

TEST(TestMoreThanKInLastM, returnsFalseOnNotEnoughTrueOnEnough)
{
    EventTrace trace;
    trace.push_front(Event::SUCCESS);
    EXPECT_TRUE(moreThanKInLastM(0, 10, trace, AlwaysTruePredicate()));
    EXPECT_FALSE(moreThanKInLastM(1, 10, trace, AlwaysTruePredicate()));
}

TEST(TestRepairAfterKSuccessInM, returnsFalseForEmptyTrace)
{
    EventTrace empty;
    EXPECT_FALSE(RepairAfterKSuccessInM(0, 10)(empty));
    EXPECT_FALSE(RepairAfterKSuccessInM(0, -1)(empty));
    EXPECT_FALSE(RepairAfterKSuccessInM(1, 10)(empty));
    EXPECT_FALSE(RepairAfterKSuccessInM(1, -1)(empty));
}

TEST(TestRepairAfterKSuccessInM, returnsFalseOnNotEnoughTrueOnEnough)
{
    EventTrace trace;
    trace.push_front(Event::SUCCESS);
    EXPECT_TRUE(RepairAfterKSuccessInM(0, 10)(trace));
    EXPECT_FALSE(RepairAfterKSuccessInM(1, 10)(trace));
}

TEST(TestBreakAfterKFailsInM, returnsFalseOnNotEnoughTrueOnEnough)
{
    EventTrace trace;
    trace.push_front(Event::SUCCESS);
    trace.push_front(Event::TIMEOUT);

    EXPECT_FALSE(BreakAfterKFailsInM(1, 2)(trace));
    EXPECT_FALSE(BreakAfterKFailsInM(1, -1)(trace));

    trace.push_front(Event::EXCEPTION);
    EXPECT_TRUE(BreakAfterKFailsInM(1, 2)(trace));
    EXPECT_TRUE(BreakAfterKFailsInM(1, -1)(trace));

    trace.push_front(Event::SUCCESS);
    EXPECT_FALSE(BreakAfterKFailsInM(1, 2)(trace));
    EXPECT_TRUE(BreakAfterKFailsInM(1, 3)(trace));
}
