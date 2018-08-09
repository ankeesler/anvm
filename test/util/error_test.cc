#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "src/util/error.h"

using namespace testing;

TEST(ErrorTest, None) {
    EXPECT_THAT(Error::NONE, Eq(Error::NONE));

    Error empty;
    EXPECT_THAT(Error::NONE, Eq(empty));

    Error notEmpty;
    notEmpty << "";
    EXPECT_THAT(Error::NONE, Ne(notEmpty));

    Error defNotEmpty;
    defNotEmpty << "def not empty";
    EXPECT_THAT(Error::NONE, Ne(defNotEmpty));
}

TEST(ErrorTest, Basic) {
    Error e;
    EXPECT_FALSE(e);
    EXPECT_EQ(e.S(), "");

    e << "";
    EXPECT_TRUE(e);
    EXPECT_EQ(e.S(), "");

    e << "foo, bar, bat";
    EXPECT_TRUE(e);
    EXPECT_EQ(e.S(), "foo, bar, bat");

    e << "; tuna, fish, marlin";
    EXPECT_TRUE(e);
    EXPECT_EQ(e.S(), "foo, bar, bat; tuna, fish, marlin");

    Error copy = e;
    EXPECT_TRUE(copy);
    EXPECT_EQ(copy.S(), "foo, bar, bat; tuna, fish, marlin");
    EXPECT_TRUE(e);
    EXPECT_EQ(e.S(), "foo, bar, bat; tuna, fish, marlin");

    copy << "; jello, pudding, pahpblahb";
    EXPECT_TRUE(copy);
    EXPECT_EQ(copy.S(), "foo, bar, bat; tuna, fish, marlin; jello, pudding, pahpblahb");

    Error nums;
    nums << 5 << " is less than " << (long)35;
    EXPECT_TRUE(nums);
    EXPECT_EQ(nums.S(), "5 is less than 35");

    Error equals("5 is less than 35");
    EXPECT_TRUE(equals);
    EXPECT_EQ(nums, equals);

    const std::string same = "5 is less than 35";
    EXPECT_EQ(nums, Error(same));
}

TEST(ErrorTest, NotEquals) {
    Error error("here is an error");
    Error differentError("here is a different error");
    // FIXME: EXPECT_THAT(error, Ne(differentError));
}
