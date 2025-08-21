//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter="LoggerTest.*"
//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter=LoggerTest.testLogEntryMacro
//valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes --verbose --log-file=valgrind.log ./bin/TestLogger_d

#include "CommonFunc.hpp"
#include "Logger.hpp"
#include "LOGGER_MACROS.hpp"
#include "FileOps.hpp"
#include "ConsoleOps.hpp"

#include <type_traits>

using namespace logger;

class LoggerTest : public CommonTestDataGenerator
{
    public:
        std::vector<std::string_view> logTypeStringVec = 
            { "ERR", "INF", "WARN", "DBG", "FATAL", "ASRT", "DEFAULT" };

        std::vector<logger::LOG_TYPE> logTypeVec = 
        {   LOG_TYPE::LOG_ERR,
            LOG_TYPE::LOG_INFO,
            LOG_TYPE::LOG_WARN,
            LOG_TYPE::LOG_DBG,
            LOG_TYPE::LOG_FATAL,
            LOG_TYPE::LOG_ASSERT,
            LOG_TYPE::LOG_DEFAULT
        };
        static void assertionDeathTest()
        {
            LOG_ASSERT(2 > 3);
        }
        static void assertionMsgDeathTest()
        {
            LOG_ASSERT(0.62f < -0.51f, "Death at the end");
        }
        static void fatalDeathTest()
        {
            LOG_FATAL("FATAL error at line {}", __LINE__);
        }
        static void testLoggedData(
            const LOG_TYPE& expLogType,
            const std::string_view prettyFuncName,
            const std::string_view marker,
            const std::string_view logMsg = "") noexcept
        {
            auto& logStream = loggerObj.getLogStream();
            std::ostringstream oss;
            oss << std::this_thread::get_id();
            EXPECT_TRUE(logStream.str().find(oss.str()) != std::string::npos)
                << "oss.str() = " << oss.str() << ", " << "logStream.str() = " << logStream.str();
            std::ostringstream().swap(oss);
            oss << __FILE__;
            EXPECT_TRUE(logStream.str().find(oss.str()) != std::string::npos)
                << "oss.str() = " << oss.str() << ", " << "logStream.str() = " << logStream.str();

            EXPECT_TRUE(logStream.str().find(Logger::covertLogTypeEnumToString(expLogType)) != std::string::npos);
            ASSERT_TRUE(std::string::npos != prettyFuncName.find(":"));
            auto className = prettyFuncName.substr(0, prettyFuncName.find_first_of(":"));
            className = className.substr(className.rfind(" ") + 1);
            auto funcNameWithoutClassName = prettyFuncName.substr(prettyFuncName.find_last_of(":") + 1);
            funcNameWithoutClassName = funcNameWithoutClassName.substr(0, funcNameWithoutClassName.find_first_of("("));

            EXPECT_TRUE(logStream.str().find(className) != std::string::npos) << "className = " << className << std::endl;
            EXPECT_TRUE(logStream.str().find(funcNameWithoutClassName) != std::string::npos) << "funcNameWithoutClassName = " << funcNameWithoutClassName;
            EXPECT_TRUE(logStream.str().find(marker) != std::string::npos);

            if (!logMsg.empty())
                EXPECT_TRUE(logStream.str().find(logMsg) != std::string::npos);
        }
        static int* funcReturningPointer(const int val1, const int val2) noexcept
        {
            LOG_ENTRY();
            static std::unique_ptr<int> pRetVal = std::make_unique<int>(val1 * val2);
            LOG_EXIT();
            return pRetVal.get();
        }
        static std::unique_ptr<std::string> funcReturnUniquePtr() noexcept
        {
            LOG_ENTRY();
            LOG_EXIT();
            return std::make_unique<std::string>("LoggerTesting");
        }
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

TEST_F(LoggerTest, testLogList)
{
    std::uintmax_t maxTextSize = 10;
    std::vector<std::string> dataVec;
    std::list<std::string> dataList;
    for (auto cnt = 0; cnt < 5; ++cnt)
    {
        dataVec.push_back(generateRandomText(maxTextSize));
        dataList.push_back(generateRandomText(maxTextSize));
    }

    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
    LOG_LIST(dataVec, "Testing log msgs for list of size {:d}", dataVec.size());
    testLoggedData(LOG_TYPE::LOG_INFO, __PRETTY_FUNCTION__, FORWARD_ANGLES);
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
    LOG_LIST(dataList, "Testing log msgs for list of size {:d}", dataVec.size());
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
}

/**
 * @note The following three tests should be marked disabled
 * while running all the test cases for this test suite.
 * Reasone being, assertion failure testing in a multithreaded
 * enviornment is not guranteed to work as expected always without
 * causing a deadlock. Eveven with the death test style
 * set to threadsafe. So if you want to test the assertion failure,
 * you can enable these tests and run them separately in isolation for each test.
 * @see https://github.com/google/googletest/blob/main/docs/advanced.md#death-tests-and-threads
 * for more details.
 */
TEST_F(LoggerTest, DISABLED_testAssertion)
{
    GTEST_FLAG_SET(death_test_style, "threadsafe");
    ASSERT_DEATH(assertionDeathTest(), "");
}

TEST_F(LoggerTest, DISABLED_testAssertionWithMsg)
{
    GTEST_FLAG_SET(death_test_style, "threadsafe");
    ASSERT_DEATH(assertionMsgDeathTest(), "");
}

TEST_F(LoggerTest, DISABLED_testFatalError)
{
    GTEST_FLAG_SET(death_test_style, "threadsafe");
    ASSERT_DEATH(fatalDeathTest(), "");
}

TEST_F(LoggerTest, testLogEntryExit)
{
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
    LOG_ENTRY();
    testLoggedData(LOG_TYPE::LOG_INFO, __PRETTY_FUNCTION__, FORWARD_ANGLES);
    LOG_EXIT();
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
    testLoggedData(LOG_TYPE::LOG_INFO, __PRETTY_FUNCTION__, BACKWARD_ANGLES);
}

TEST_F(LoggerTest, testLogEntryExitDebug)
{
#if defined (DEBUG) || defined (__DEBUG__)
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
#endif
    LOG_ENTRY_DBG();
#if defined (DEBUG) || defined (__DEBUG__)
    testLoggedData(LOG_TYPE::LOG_INFO, __PRETTY_FUNCTION__, FORWARD_ANGLES);
#endif
    LOG_EXIT_DBG();
#if defined (DEBUG) || defined (__DEBUG__)
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
    testLoggedData(LOG_TYPE::LOG_INFO, __PRETTY_FUNCTION__, BACKWARD_ANGLES);
#endif
}

TEST_F(LoggerTest, testLogEntryExitWithMsg)
{
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
    auto logType = static_cast<std::underlying_type_t<LOG_TYPE>>(LOG_TYPE::LOG_INFO);
    LOG_ENTRY("Entering now {} with LOG_TYPE as {:#08x} and log marker as {}", 
        std::string("testLogEntryExitWithMsg"), logType, FORWARD_ANGLES);
    testLoggedData(LOG_TYPE::LOG_INFO, __PRETTY_FUNCTION__, FORWARD_ANGLES);

    LOG_EXIT("Exiting {} with LOG_TYPE as {:#08x} and log marker as {}", 
        std::string("testLogEntryExitWithMsg"), logType, BACKWARD_ANGLES);
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
    testLoggedData(LOG_TYPE::LOG_INFO, __PRETTY_FUNCTION__, BACKWARD_ANGLES);
}

TEST_F(LoggerTest, testLogEntryExitWithMsgDebugMode)
{
#if defined (DEBUG) || defined (__DEBUG__)
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
#endif
    auto logType = static_cast<std::underlying_type_t<LOG_TYPE>>(LOG_TYPE::LOG_INFO);
    LOG_ENTRY_DBG("Entering now {} with LOG_TYPE as {:#08x} and log marker as {}",
        std::string("testLogEntryExitWithMsg"), logType, FORWARD_ANGLES);
#if defined (DEBUG) || defined (__DEBUG__)
    testLoggedData(LOG_TYPE::LOG_INFO, __PRETTY_FUNCTION__, FORWARD_ANGLES);
#endif

    LOG_EXIT_DBG("Exiting {} with LOG_TYPE as {:#08x} and log marker as {}",
        std::string("testLogEntryExitWithMsg"), logType, BACKWARD_ANGLES);
#if defined (DEBUG) || defined (__DEBUG__)
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
    testLoggedData(LOG_TYPE::LOG_INFO, __PRETTY_FUNCTION__, BACKWARD_ANGLES);
#endif
}

TEST_F(LoggerTest, testLogWarning)
{
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
    auto logType = static_cast<std::underlying_type_t<LOG_TYPE>>(LOG_TYPE::LOG_WARN);
    LOG_WARN("Going to exit {} with LOG_TYPE as {:#08x} and log marker as {}",
        std::string("testLogWarning"), logType, FORWARD_ANGLE);
    testLoggedData(LOG_TYPE::LOG_WARN, __PRETTY_FUNCTION__, FORWARD_ANGLE);
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
}

TEST_F(LoggerTest, testLogInfo)
{
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
    auto logType = static_cast<std::underlying_type_t<LOG_TYPE>>(LOG_TYPE::LOG_INFO);
    LOG_INFO("In {} with LOG_TYPE as {:#08x} and log marker as {}",
        std::string("testLogInfo"), logType, FORWARD_ANGLE);
    testLoggedData(LOG_TYPE::LOG_INFO, __PRETTY_FUNCTION__, FORWARD_ANGLE);
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
}

TEST_F(LoggerTest, testLogError)
{
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
    auto logType = static_cast<std::underlying_type_t<LOG_TYPE>>(LOG_TYPE::LOG_ERR);
    LOG_ERR("No error to exit {} with LOG_TYPE as {:#08x} and log marker as {}",
        std::string("testLogInfo"), logType, FORWARD_ANGLE);
    testLoggedData(LOG_TYPE::LOG_ERR, __PRETTY_FUNCTION__, FORWARD_ANGLE);
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
}

TEST_F(LoggerTest, testLogImp)
{
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
    auto logType = static_cast<std::underlying_type_t<LOG_TYPE>>(LOG_TYPE::LOG_IMP);
    LOG_IMP("It should be working for {} as well with LOG_TYPE as {:#08x} and log marker as {}",
        std::string("testLogImp"), logType, FORWARD_ANGLE);
    testLoggedData(LOG_TYPE::LOG_IMP, __PRETTY_FUNCTION__, FORWARD_ANGLE);
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
}

TEST_F(LoggerTest, testLogDbg)
{
#if defined (DEBUG) || defined (__DEBUG__)
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
    auto logType = static_cast<std::underlying_type_t<LOG_TYPE>>(LOG_TYPE::LOG_DBG);
    LOG_DBG("It should be working for {} in debug mode as well with LOG_TYPE as {:#08x} and log marker as {}",
        std::string("testLogDbg"), logType, FORWARD_ANGLE);
    testLoggedData(LOG_TYPE::LOG_DBG, __PRETTY_FUNCTION__, FORWARD_ANGLE);
    std::cout << std::endl; // Put a line break so that the printed log msg can be seen clearly
#endif
}

/**
 * @brief The objective of this test feature
 * is to test and polish the class name and
 * function name in the actual log message.
 * We need to look at the console or file
 * (console option is quick and better) to
 * check how the class and function names
 * are been populated in the log message
 * and trim/polish them accordingly.
 */
TEST_F(LoggerTest, testDiffFuncSignatures)
{
    EXPECT_NE(0, *funcReturningPointer(10, 10));
    EXPECT_FALSE(funcReturnUniquePtr()->empty());
    {
        auto lamdaFunc = [](const std::string& val1, const std::string& val2)
        {
            LOG_ENTRY();
            if (val1.empty() && val2.empty())
            {
                LOG_INFO("Both the incoming string are empty");
            }
            else
            {
                LOG_INFO("Combined string results of the incoming strings {} and {} is {}", 
                    val1, val2, std::string(val1 + val2));
            }
            LOG_EXIT();
            return 0;
        };
        EXPECT_EQ(0, lamdaFunc("Testing ", "function signature"));
    }
}
