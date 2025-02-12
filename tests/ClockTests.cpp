#include "Clock.hpp"

#include <iomanip>

#include <gtest/gtest.h>

#define nullString ""

class ClockTests : public ::testing::Test
{
    protected:
        Clock clock;
};

TEST_F(ClockTests, testGetLocalTime)
{
    auto expLocalTime = clock.getLocalTime();
    auto localTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto localTimeStr = std::localtime(&localTime);
    std::ostringstream oss;
    oss << std::put_time(localTimeStr, "%c %Z");
    EXPECT_STREQ(expLocalTime.c_str(), oss.str().c_str());
}

TEST_F(ClockTests, testGetGmtTime)
{
    auto expGmtTime = clock.getGmtTime();
    auto gmtTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto gmtTimeStr = std::gmtime(&gmtTime);
    std::ostringstream oss;
    oss << std::put_time(gmtTimeStr, "%c %Z");
    EXPECT_STREQ(expGmtTime.c_str(), oss.str().c_str());
}

TEST_F(ClockTests, testGetLocalTimeStr)
{
    auto formattedLocalTime = [](const std::string_view format, const time_t* time)
    {
        std::array<char, 80> buffer;
        std::strftime(buffer.data(), sizeof(buffer), format.data(), std::localtime(time));
        return std::string(buffer.data());
    };     
    
    {
        auto expLocalTimeStr = clock.getLocalTimeStr();
        auto localTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string format = "%d/%m/%Y %H:%M:%S";
        EXPECT_STREQ(expLocalTimeStr.c_str(), formattedLocalTime(format, &localTime).c_str());
    }
    {
        std::string format = "%Y/%m/%d %H:%M:%S";
        auto expLocalTimeStr = clock.getLocalTimeStr(format);
        auto localTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        EXPECT_STREQ(expLocalTimeStr.c_str(), formattedLocalTime(format, &localTime).c_str());
    }
    {
        std::string format = "%Y-%m-%d %H:%M:%S";
        auto expLocalTimeStr = clock.getLocalTimeStr(format);
        auto localTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        EXPECT_STREQ(expLocalTimeStr.c_str(), formattedLocalTime(format, &localTime).c_str());
    }
    {
        std::string format = "%Y%m%d_%H:%M:%S";
        auto expLocalTimeStr = clock.getLocalTimeStr(format);
        auto localTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        EXPECT_STREQ(expLocalTimeStr.c_str(), formattedLocalTime(format, &localTime).c_str());
    }
    {
        std::string format = "%Y%m%d_%H%M%S";
        auto expLocalTimeStr = clock.getLocalTimeStr(format);
        auto localTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        EXPECT_STREQ(expLocalTimeStr.c_str(), formattedLocalTime(format, &localTime).c_str());
    }
}

TEST_F(ClockTests, testGetGmtTimeStr)
{
    auto formattedGmtTime = [](const std::string_view format, const time_t* time)
    {
        std::array<char, 80> buffer;
        std::strftime(buffer.data(), sizeof(buffer), format.data(), std::gmtime(time));
        return std::string(buffer.data());
    };     
    
    {
        auto expGmtTimeStr = clock.getGmtTimeStr();
        auto gmtTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::string format = "%d/%m/%Y %H:%M:%S";
        EXPECT_STREQ(expGmtTimeStr.c_str(), formattedGmtTime(format, &gmtTime).c_str());
    }
    {
        std::string format = "%Y/%m/%d %H:%M:%S";
        auto expGmtTimeStr = clock.getGmtTimeStr(format);
        auto gmtTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        EXPECT_STREQ(expGmtTimeStr.c_str(), formattedGmtTime(format, &gmtTime).c_str());
    }
    {
        std::string format = "%Y-%m-%d %H:%M:%S";
        auto expGmtTimeStr = clock.getGmtTimeStr(format);
        auto gmtTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        EXPECT_STREQ(expGmtTimeStr.c_str(), formattedGmtTime(format, &gmtTime).c_str());
    }
    {
        std::string format = "%Y%m%d_%H:%M:%S";
        auto expGmtTimeStr = clock.getGmtTimeStr(format);
        auto gmtTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        EXPECT_STREQ(expGmtTimeStr.c_str(), formattedGmtTime(format, &gmtTime).c_str());
    }
    {
        std::string format = "%Y%m%d_%H%M%S";
        auto expGmtTimeStr = clock.getGmtTimeStr(format);
        auto gmtTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        EXPECT_STREQ(expGmtTimeStr.c_str(), formattedGmtTime(format, &gmtTime).c_str());
    }
}

TEST_F(ClockTests, testGetDayOfWeek)
{
    auto expDayOfWeek = clock.getDayOfWeek();
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%A");
    EXPECT_STREQ(expDayOfWeek.c_str(), oss.str().c_str());
}

TEST_F(ClockTests, testGetMonth)
{
    auto expMonth = clock.getMonth();
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%B");
    EXPECT_STREQ(expMonth.c_str(), oss.str().c_str());
}

TEST_F(ClockTests, testGetYear)
{
    auto expYear = clock.getYear();
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y");
    EXPECT_STREQ(expYear.c_str(), oss.str().c_str());
}

TEST_F(ClockTests, testGetTimeOfTheDay)
{
    auto [hours, minutes, seconds] = clock.getTimeOfTheDay();
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto localTime = std::localtime(&now);
    EXPECT_EQ(hours, localTime->tm_hour);
    EXPECT_EQ(minutes, localTime->tm_min);
    EXPECT_EQ(seconds, localTime->tm_sec);
}

TEST_F(ClockTests, testGetGmtTimeOfTheDay)
{
    auto [hours, minutes, seconds] = clock.getGmtTimeOfTheDay();
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto gmtTime = std::gmtime(&now);
    EXPECT_EQ(hours, gmtTime->tm_hour);
    EXPECT_EQ(minutes, gmtTime->tm_min);
    EXPECT_EQ(seconds, gmtTime->tm_sec);
}

TEST_F(ClockTests, testStart)
{
    clock.start();
    EXPECT_TRUE(clock.isRunning());
}

TEST_F(ClockTests, testStop)
{
    clock.start();
    clock.stop();
    EXPECT_FALSE(clock.isRunning());
}

TEST_F(ClockTests, testGetElapsedTime)
{
    {
        clock.start();
        std::this_thread::sleep_for(std::chrono::seconds(1));
        clock.stop();
        auto elapsedTime = clock.getElapsedTime(TimeUnits::SECONDS);
        EXPECT_GE(elapsedTime, 1.0);
        EXPECT_LE(elapsedTime, 1.1);
    }
    {
        clock.start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        clock.stop();
        auto elapsedTime = clock.getElapsedTime(TimeUnits::MILLISECONDS);
        EXPECT_GE(elapsedTime, 100.0);
        EXPECT_LE(elapsedTime, 105.0);  // 100ms + 5ms (tolerance)
    }
}
