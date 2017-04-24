#include <Validator.h>
#include <gtest/gtest.h>

using namespace ezhic;

namespace {
static int get17()
{
    return 17;
}
}

TEST(TestValidator, Success_in_trace_after_valid_result)
{
    Label label(test_info_->name());
    FixedValueValidator<int> validator(label, 17);
    validator.run(&get17);

    const EventTrace &eventTrace = ezreg::circuit(label).eventTrace();

    EXPECT_TRUE(Event::SUCCESS == eventTrace.front().type());
}

TEST(TestValidator, Invalid_in_trace_after_invalid_result)
{
    Label label(test_info_->name());
    FixedValueValidator<int> validator(label, 18);
    validator.run(&get17);

    const EventTrace &eventTrace = ezreg::circuit(label).eventTrace();

    EXPECT_TRUE(Event::SUCCESS == eventTrace.back().type());
    EXPECT_TRUE(Event::INVALID == eventTrace.front().type());
}
