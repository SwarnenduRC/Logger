//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter="ConsoleOpsTest.*"
//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter=ConsoleOpsTest.testloggingTypesStringToEnumAndEnumToString

#include "ConsoleOps.hpp"

#include <gtest/gtest.h>

class ConsoleOpsTest : public ::testing::Test
{
};

