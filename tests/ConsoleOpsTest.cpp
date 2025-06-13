//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter="ConsoleOpsTest.*"
//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter=ConsoleOpsTest.testloggingTypesStringToEnumAndEnumToString

#include "ConsoleOps.hpp"

#include "CommonFunc.hpp"

class ConsoleOpsTest : public CommonTestDataGenerator
{
    public:
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

class ConsoleOpsTestClass : public ConsoleOps
{
    public:
        ConsoleOpsTestClass() : ConsoleOps() {}
        ~ConsoleOpsTestClass() = default;

        ConsoleOpsTestClass(const ConsoleOpsTestClass& rhs) = delete;
        ConsoleOpsTestClass(ConsoleOpsTestClass&& rhs) = delete;
        ConsoleOpsTestClass& operator=(const ConsoleOpsTestClass& rhs) = delete;
        ConsoleOpsTestClass& operator=(ConsoleOpsTestClass&& rhs) = delete;

        inline void setTestingModeOn() { m_testing = true; }
        inline void setTestingModeOff() { m_testing = false; }
        inline const std::ostringstream& getTestStringStreamFromConsole() const { return m_testStringStream; }
        inline const std::string getClassId() const override { return "ConsoleOpsTest"; }
        void writeDataTo(const std::string_view data) override { ::ConsoleOps::writeDataTo(data); }
};

TEST_F(ConsoleOpsTest, testWriteToConsole)
{
    std::uintmax_t maxTextSize = 255;
    auto text = generateRandomText(maxTextSize);
    ConsoleOpsTestClass testObj;
    testObj.setTestingModeOn();
    testObj.write(text);
    auto& result = testObj.getTestStringStreamFromConsole();
    EXPECT_EQ(text, result.str());
    testObj.setTestingModeOff();
}

