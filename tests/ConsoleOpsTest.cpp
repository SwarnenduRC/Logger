//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter="ConsoleOpsTest.*"
//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter=ConsoleOpsTest.testloggingTypesStringToEnumAndEnumToString

#include "ConsoleOps.hpp"

#include <gtest/gtest.h>

class ConsoleOpsTest : public ::testing::Test
{
};

TEST_F(ConsoleOpsTest, testloggingTypesStringToEnumAndEnumToString)
{
    std::vector<std::string> enumStringsVec = { "ERROR_LOG", "INFO_LOG", "DEBUG_LOG", "DEFAULT_LOG" };
    std::vector<ConsoleOps::LoggingTypes> enumsVec = {
        ConsoleOps::LoggingTypes::ERROR_LOG,
        ConsoleOps::LoggingTypes::INFO_LOG,
        ConsoleOps::LoggingTypes::DEBUG_LOG,
        ConsoleOps::LoggingTypes::DEFAULT_LOG
    };

    for (size_t idx = 0; idx < enumsVec.size(); idx++)
    {
        EXPECT_EQ(enumsVec[idx], ConsoleOps::loggingTypesStringToEnum(enumStringsVec[idx]));
        EXPECT_EQ(enumStringsVec[idx], ConsoleOps::loggingTypesEnumToString(enumsVec[idx]));
    }
}
