/**
 * @file ClockTests.cpp
 * @brief Unit tests for the Clock class using Google Test framework.
 *
 * This file contains a series of test cases to verify the functionality of the Clock class.
 * The tests cover various aspects of the Clock class, including time retrieval, formatting,
 * and timer functionality.
 *
 * The following test cases are included:
 * - testGetLocalTime: Tests the retrieval of local time.
 * - testGetGmtTime: Tests the retrieval of GMT time.
 * - testGetLocalTimeStr: Tests the retrieval of formatted local time strings.
 * - testGetGmtTimeStr: Tests the retrieval of formatted GMT time strings.
 * - testGetDayOfWeek: Tests the retrieval of the current day of the week.
 * - testGetMonth: Tests the retrieval of the current month.
 * - testGetYear: Tests the retrieval of the current year.
 * - testGetTimeOfTheDay: Tests the retrieval of the current time of the day (hours, minutes, seconds).
 * - testGetGmtTimeOfTheDay: Tests the retrieval of the current GMT time of the day (hours, minutes, seconds).
 * - testStart: Tests the start functionality of the timer.
 * - testStop: Tests the stop functionality of the timer.
 * - testGetElapsedTime: Tests the retrieval of elapsed time in different units (seconds, milliseconds).
 * - testTimerThreadSafety: Tests the thread safety of the timer functionality.
 *
 * The Clock class is expected to provide accurate time information and handle timer operations
 * correctly, even in multi-threaded environments.
 *
 * @note The tests use the Google Test framework for assertions and test case management.
 */
#include "Clock.hpp"

#include <iomanip>

#include <gtest/gtest.h>

using namespace logger;

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
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        clock.stop();
        auto elapsedTime = clock.getElapsedTime(TimeUnits::MILLISECONDS);
        EXPECT_GE(elapsedTime, 10.0);
        EXPECT_LE(elapsedTime, 15.0);  // 100ms + 5ms (tolerance)
    }
}

TEST_F(ClockTests, testTimerThreadSafety)
{
    std::mutex timerMutex;
    std::condition_variable timerCond;
    std::atomic_bool isRunning = false;

    {
        auto threadFunc1 = [&]()
        {
            std::lock_guard<std::mutex> lock(timerMutex);
            clock.start();
            ASSERT_TRUE(clock.isRunning());
            isRunning = false;
            std::this_thread::sleep_for(std::chrono::milliseconds(15));
            clock.stop();
            isRunning = false;
            timerCond.notify_one();
        };
        auto threadFunc2 = [&]()
        {
            ASSERT_TRUE(clock.isRunning());
            std::unique_lock<std::mutex> lock(timerMutex);
            timerCond.wait(lock, [&] { return !isRunning.load(); });
            ASSERT_FALSE(clock.isRunning());
        };

        std::thread t1(threadFunc1);
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        std::thread t2(threadFunc2);
        t1.join();
        t2.join();
    }
    {
        auto threadFunc1 = [&]()
        {
            std::lock_guard<std::mutex> lock(timerMutex);
            clock.start();
            ASSERT_TRUE(clock.isRunning());
            std::this_thread::sleep_for(std::chrono::milliseconds(5));
        };
        auto threadFunc2 = [&]()
        {
            EXPECT_TRUE(clock.isRunning());
            auto timeElapsed = clock.getElapsedTime(TimeUnits::MILLISECONDS);
            EXPECT_EQ(timeElapsed, -1.0);
        };
        std::thread t1(threadFunc1);
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        std::thread t2(threadFunc2);
        t1.join();
        t2.join();
    }
}

