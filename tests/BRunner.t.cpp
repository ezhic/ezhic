#include "Helpers.h"
#include "gtest/gtest.h"

#include <BRunner.h>
#include <memory>

#include <Bundle.h>
#include <Circuitry/Circuitry.h>
#include <Circuitry/IntentionTagger.h>

using namespace ezhic;

namespace {

char NullaryFChar()
{
    return 17;
}
void NullaryFVoid()
{
    return;
}

char UnaryFChar(int)
{
    return 17;
}
void UnaryFVoid(int)
{
    return;
}

char BinaryFChar(int, float)
{
    return 17;
}

int Throw()
{
    throw std::invalid_argument("...");
    return 0;
}

struct BNullaryTestChar
{
  public:
    typedef char result_type;

    result_type operator()() const { return 17; }
    result_type constmember() const { return 17; }
    result_type member() { return 17; }
};

struct BNullaryTestVoid
{
  public:
    typedef void result_type;
    void operator()() const { return; }
    result_type constmember() const { return; }
    result_type member() { return; }
};

struct BUnaryTestChar
{
    typedef char result_type;
    typedef int argument_type;

    result_type operator()(int) const { return 17; }
    result_type constmember(int) const { return 17; }
    result_type constrefmember(int &v) const { return v = 17; }
    result_type member(int) { return 17; }
};

struct BBinaryTestChar
{
    typedef char result_type;
    typedef int first_argument_type;
    typedef float second_argument_type;

    result_type operator()(int, float) const { return 17; }
    result_type constmember(int, float) const { return 17; }
    result_type constrefmember(int &v, float &f) const { return v = f = 17; }
    result_type member(int, float) { return 17; }
};

struct BUnaryTestVoid
{
    typedef void result_type;
    typedef int argument_type;
    result_type operator()(int) const { return; }
    result_type constmember(int) const { return; }
    result_type member(int) { return; }
};
}

class EzhicBundleTest : public ::testing::Test
{
  protected:
    EzhicBundleTest() : timer(), reg(&Registry::instance()), label(), brp()
    {
        reg->cloneInTimer(timer);
        const ::testing::TestInfo *const test_info =
            ::testing::UnitTest::GetInstance()->current_test_info();
        label = std::string(test_info->test_case_name()) +
                std::string(test_info->name());
        brp = new BRunner(label);
    }
    virtual void TearDown()
    {
        EXPECT_EQ(1u, reg->circuit(label).eventTrace().size());
    }
    ~EzhicBundleTest() { delete brp; }
    MonotonicDummy timer;
    Registry *reg;
    Label label;
    BRunner *brp;
};

TEST_F(EzhicBundleTest, BundleWithNullaryFunc)
{
    EXPECT_EQ(char(17), brp->run(&NullaryFChar).value());
}

TEST_F(EzhicBundleTest, BundleWithNullaryVoidFunc)
{
    brp->run(&NullaryFVoid);
}

TEST_F(EzhicBundleTest, BundleWithNullaryConstMemFunc)
{
    BNullaryTestChar obj;
    typedef char (BNullaryTestChar::*cmptr_t)() const;
    cmptr_t member = &BNullaryTestChar::constmember;
    EXPECT_EQ(char(17), brp->run_m(obj, member).value());
}

TEST_F(EzhicBundleTest, BundleWithNullaryMemFunc)
{
    BNullaryTestChar obj;
    typedef char (BNullaryTestChar::*mptr_t)();
    mptr_t member = &BNullaryTestChar::member;
    EXPECT_EQ(char(17), brp->run_m(obj, member).value());
}

TEST_F(EzhicBundleTest, BundleWithNullaryConstMemFuncVoid)
{
    BNullaryTestVoid obj;
    typedef void (BNullaryTestVoid::*cmptr_t)() const;
    cmptr_t member = &BNullaryTestVoid::constmember;
    EXPECT_TRUE(brp->run_m(obj, member).isNull());
}

TEST_F(EzhicBundleTest, BundleWithNullaryMemFuncVoid)
{
    BNullaryTestVoid obj;
    typedef void (BNullaryTestVoid::*mptr_t)();
    mptr_t member = &BNullaryTestVoid::member;
    EXPECT_TRUE(brp->run_m(obj, member).isNull());
}

TEST_F(EzhicBundleTest, BundleWithNullaryFunctor)
{
    EXPECT_EQ(char(17), brp->run(BNullaryTestChar()).value());
}

TEST_F(EzhicBundleTest, BundleWithNullaryVoidFunctor)
{
    brp->run(BNullaryTestVoid());
}

TEST_F(EzhicBundleTest, BundleWithUnaryFunc)
{
    EXPECT_EQ(char(17), brp->run(UnaryFChar, 11).value());
}

TEST_F(EzhicBundleTest, BundleWithUnaryVoidFunc)
{
    EXPECT_TRUE(brp->run(UnaryFVoid, 11).isNull());
}

TEST_F(EzhicBundleTest, BundleWithUnaryFunctor)
{
    EXPECT_EQ(char(17), brp->run(BUnaryTestChar(), 11).value());
}

TEST_F(EzhicBundleTest, BundleWithUnaryVoidFunctor)
{
    EXPECT_TRUE(brp->run(BUnaryTestVoid(), 11).isNull());
}

TEST_F(EzhicBundleTest, BundleWithUnaryConstMemFunc)
{
    BUnaryTestChar obj;
    typedef char (BUnaryTestChar::*cmptr_t)(int) const;
    cmptr_t member = &BUnaryTestChar::constmember;
    EXPECT_EQ(char(17), brp->run_m(obj, member, 1).value());
}

TEST_F(EzhicBundleTest, BundleWithUnaryConstMemRefFunc)
{
    BUnaryTestChar obj;
    typedef char (BUnaryTestChar::*cmptr_t)(int &) const;
    cmptr_t member = &BUnaryTestChar::constrefmember;
    int lvalue = -1;
    EXPECT_EQ(char(17), brp->run_m(obj, member, lvalue).value());
    EXPECT_EQ(17, lvalue);
}

TEST_F(EzhicBundleTest, BundleWithUnaryMemFunc)
{
    BUnaryTestChar obj;
    typedef char (BUnaryTestChar::*cmptr_t)(int);
    cmptr_t member = &BUnaryTestChar::member;
    EXPECT_EQ(char(17), brp->run_m(obj, member, 1).value());
}

TEST_F(EzhicBundleTest, BundleWithBinaryMemFunc)
{
    BBinaryTestChar obj;
    typedef char (BBinaryTestChar::*cmptr_t)(int, float);
    cmptr_t member = &BBinaryTestChar::member;
    EXPECT_EQ(char(17), brp->run_m(obj, member, 1, 1.0).value());
}

TEST_F(EzhicBundleTest, BundleWithBinaryFunc)
{
    EXPECT_EQ(char(17), brp->run(BinaryFChar, 11, 1.0).value());
}

TEST_F(EzhicBundleTest, BundleWithBinaryFunctor)
{
    EXPECT_EQ(char(17), brp->run(BUnaryTestChar(), 11).value());
}

TEST_F(EzhicBundleTest, FailingTaskCaught)
{
    EXPECT_TRUE(brp->run(&Throw).isNull());
    EXPECT_EQ(Event::EXCEPTION,
              reg->circuit(label).eventTrace().back().type());
}

TEST_F(EzhicBundleTest, ScopeWithResult)
{
    TNullable<int> result;
    EZ_START(label)
    result = 10 + 7;
    EZ_END()
    EXPECT_EQ(char(17), result.value());
}

TEST_F(EzhicBundleTest, ScopeWithVoid)
{
    EZ_START(label);  // stuff
    EZ_END()
}

TEST(EzhicBundleTestS, NestedMacroSameLabel)
{
    Label label("NestedMacroSameLabel");
    EZ_START(label);
    EZ_START(label);
    BRunner br(label);
    br.run(BUnaryTestChar(), 11);
    EZ_END();
    EZ_END();
    EXPECT_EQ(3u, ezreg::circuit("NestedMacroSameLabel").eventTrace().size());
}

TEST_F(EzhicBundleTest, NestedMacroDifferentLabel)
{
    EZ_START(label);
    EZ_START(label + "nested");
    EZ_END();
    EZ_END();
    EXPECT_EQ(1u, ezreg::circuit(label + "nested").eventTrace().size());
}

void runWithThrow(BRunner &br, const Label &label)
{
    EZ_START(label);
    Throw();
    EZ_END();
}

TEST_F(EzhicBundleTest, NoRethrowWillNotThrow)
{
    Bundle b = ezreg::getBundle(label);
    b.isRethrow(false);
    ezreg::writeBundle(label, b);
    runWithThrow(*brp, label);
}

TEST_F(EzhicBundleTest, RethrowWillRethrow)
{
    Bundle b = ezreg::getBundle(label);
    b.isRethrow(true);
    ezreg::writeBundle(label, b);
    EXPECT_THROW(runWithThrow(*brp, label), std::invalid_argument);
}

TEST(EzhicBundleTestS, AlternativeCtor)
{
    Circuit c;
    Bundle b;
    MonotonicDummy timer;
    BRunner br(c, b, &timer);
    br.run(BUnaryTestChar(), 11);
    EXPECT_EQ(1u, c.eventTrace().size());
}
