//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter="LoggerTest.*"
//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter=LoggerTest.testLogTypeStringToEnum

#include "Logger.hpp"

#include <gtest/gtest.h>

using namespace logger;

class LoggerTest : public ::testing::Test
{
    public:
        std::vector<std::string_view> logTypeStringVec = 
            { "ERR", "INF", "WARN", "DBG", "FATAL", "DEFAULT" };

        std::vector<logger::LOG_TYPE> logTypeVec = 
        {   LOG_TYPE::LOG_ERR,
            LOG_TYPE::LOG_INFO,
            LOG_TYPE::LOG_WARN,
            LOG_TYPE::LOG_DBG,
            LOG_TYPE::LOG_FATAL,
            LOG_TYPE::LOG_DEFAULT
        };
};

class LoggerTestObj final : public Logger
{
    LoggerTestObj() = delete;
    LoggerTestObj(const std::string_view timeFormat)
        : Logger(timeFormat)
    {}

    ~LoggerTestObj() = default;
    LoggerTestObj(const LoggerTestObj& rhs) = delete;
    LoggerTestObj(LoggerTestObj&& rhs) = delete;
    LoggerTestObj& operator=(const LoggerTestObj& rhs) = delete;
    LoggerTestObj& operator=(LoggerTestObj&& rhs) = delete;
};

TEST_F(LoggerTest, testLogTypeStringToEnum)
{
    for (size_t idx = 0; idx < logTypeVec.size(); ++idx)
        EXPECT_EQ(logTypeVec[idx], Logger::convertStringToLogTypeEnum(logTypeStringVec[idx]));
}

TEST_F(LoggerTest, testLogTypeEnumToString)
{
    for (size_t idx = 0; idx < logTypeVec.size(); ++idx)
        EXPECT_EQ(Logger::covertLogTypeEnumToString(logTypeVec[idx]), logTypeStringVec[idx]);
}
