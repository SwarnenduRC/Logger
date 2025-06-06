//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter="ConsoleOpsTest.*"
//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter=ConsoleOpsTest.testloggingTypesStringToEnumAndEnumToString

#include "ConsoleOps.hpp"

#include <gtest/gtest.h>

#include <random>

class ConsoleOpsTest : public ::testing::Test, ConsoleOps
{
    static std::string generateRandomText(const size_t textLen)
    {
        static const char charset[] =
        {
            "0123456789"
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            " .,!?;:-_+=()[]{}<>|/@#$%^&*~`"
        };
        std::random_device rd;   // Random seed
        std::mt19937 gen(rd());  // Mersenne Twister engine
        std::uniform_int_distribution<> dist(0, sizeof(charset) - 2); // -2 for null terminator

        std::string result;
        result.reserve(textLen);
        for (size_t i = 0; i < textLen; ++i)
        {
            result += charset[dist(gen)];
        }
        return result;
    }
};

