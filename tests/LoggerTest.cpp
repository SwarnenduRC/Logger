//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter="LoggerTest.*"
//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter=LoggerTest.testLogEntryMacro
// valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind.log ./bin/TestLogger_d

#include "Logger.hpp"

#include "CommonFunc.hpp"

#include "LOGGER_MACROS.hpp"

#include "FileOps.hpp"
#include "ConsoleOps.hpp"

using namespace logger;

class LoggerTest : public CommonTestDataGenerator
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

TEST_F(LoggerTest, testLogEntryMacro)
{
    std::string logMsg = "Testing LogEntryMacro";
    LOG_ENTRY();
    //EXPECT_TRUE(loggerObj.getLogStream().str().empty()) << loggerObj.getLogStream().str();
    LOG_WARN("About to exit testLogEntryMacro in thread");
    LOG_WARN("Testing warning")
    auto cnt = 2;
    LOG_ASSERT(3 > 2);
    LOG_ASSERT_MSG(1 < 2, "Assertion failure {}nd time", cnt)
    LOG_EXIT()
}

TEST_F(LoggerTest, testPolymorphomicIntegrity)
{
    std::string logMsg = "Testing PolymorphomicIntegrity";
    auto cnt = 10;
    LOG_ENTRY("{} {:#x}", logMsg, cnt);
    {
        LoggingOps* pLogger = new ConsoleOps();
        delete pLogger;
    }
    {
        std::unique_ptr<LoggingOps> pLogger(new ConsoleOps());
    }
    {
        std::uintmax_t maxFileSize = 1024;
        std::string fileName = "TestFile.txt";
        auto expFilePath = std::filesystem::current_path().string() + getPathSeperator();
        std::unique_ptr<LoggingOps> pLogger;
        {
            pLogger.reset(new FileOps(maxFileSize, fileName));
        }
        pLogger.reset(new ConsoleOps());
        std::uintmax_t maxTextSize = 10;
        auto text = generateRandomText(maxTextSize);
        pLogger->write(text);
    }
}
