#include "Clock.hpp"

#include <gtest/gtest.h>

#define nullString ""

class ClockTests : public ::testing::Test
{
    protected:
        Clock clock;
};

TEST_F(ClockTests, testDummy)
{
    auto currLocalTime = clock.getLocalTime();
    ASSERT_NE(currLocalTime, nullString);

    auto currUtcTime = clock.getGmtTime();
    ASSERT_NE(currUtcTime, nullString);

    clock.start();
    ASSERT_TRUE(clock.isRunning());
    std::this_thread::sleep_for(std::chrono::seconds(1));
    clock.stop();
    ASSERT_FALSE(clock.isRunning());
    EXPECT_NEAR(clock.getElapsedTime(), 1, 0.1);
}