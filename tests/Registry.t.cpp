#include "gtest/gtest.h"

#include <Registry.h>

using namespace ezhic;

TEST(TestRegistry, Default_registry_has_null_timer)
{
    ASSERT_FALSE(Registry::instance().timer());
}

TEST(TestRegistry, Named_bundle_is_persistent)
{
    const Bundle &dBundle = ezreg::getBundle("foo");
    ASSERT_EQ(&dBundle, &ezreg::getBundle("foo"));
}