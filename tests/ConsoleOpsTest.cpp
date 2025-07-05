/*
 * MIT License
 * 
 * Copyright (c) 2024 Swarnendu Chandra
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file ConsoleOpsTest.cpp
 * @brief Unit tests for the ConsoleOps class.
 *
 * This file contains tests that verify the functionality of the ConsoleOps class,
 * including writing text and binary data to the console, handling different data types,
 * and ensuring correct output formatting. The tests use randomized data to validate
 * robustness and correctness of the ConsoleOps implementation.
 */

//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter="ConsoleOpsTest.*"
//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter=ConsoleOpsTest.testloggingTypesStringToEnumAndEnumToString

#include "ConsoleOps.hpp"
#include "CommonFunc.hpp"

#include <bitset>

using namespace logger;

/**
 * @brief A test class for ConsoleOps
 * * This class is used to test the ConsoleOps class.
 * * It inherits from ConsoleOps and overrides the necessary methods
 * * to enable testing.
 */
class ConsoleOpsTestClass : public ConsoleOps
{
    public:
        ConsoleOpsTestClass() : ConsoleOps() {}
        ~ConsoleOpsTestClass() = default;

        ConsoleOpsTestClass(const ConsoleOpsTestClass& rhs) = delete;
        ConsoleOpsTestClass(ConsoleOpsTestClass&& rhs) = delete;
        ConsoleOpsTestClass& operator=(const ConsoleOpsTestClass& rhs) = delete;
        ConsoleOpsTestClass& operator=(ConsoleOpsTestClass&& rhs) = delete;
        /**
         * @brief Set the Testing Mode ON of the ConsoleOps object
         * * This function is used to set the testing mode ON for the ConsoleOps object.
         * * When the testing mode is ON, the ConsoleOps object will write the data to a string stream
         */
        inline void setTestingModeOn() { m_testing = true; }
        /**
         * @brief Set the Testing Mode OFF of the ConsoleOps object
         * * This function is used to set the testing mode OFF for the ConsoleOps object.
         * * When the testing mode is OFF, the ConsoleOps object will write the data to the console
         */
        inline void setTestingModeOff() { m_testing = false; }
        /**
         * @brief Get the test string stream from the ConsoleOps object
         * * This function is used to get the test string stream from the ConsoleOps object.
         * * When the testing mode is ON, the ConsoleOps object will write the data to a string stream
         * * which can be accessed using this function.
         */
        inline const std::ostringstream& getTestStringStreamFromConsole() const { return m_testStringStream; }
        /**
         * @brief Get the Class Id for the ConsoleOpsTestClass object
         * * This function is used to get the class id of the ConsoleOpsTestClass object.
         * * It overrides the getClassId() function of the ConsoleOps class.
         *
         * @return std::string The class id of the ConsoleOpsTestClass object
         */
        inline const std::string getClassId() const override { return "ConsoleOpsTest"; }
        /**
         * @brief Write data to the out stream object
         * * This function is used to write the data to the out stream object.
         * * It overrides the writeDataTo() function of the ConsoleOps class.
         *
         * @param [in] data The data to be written to the out stream object
         */
        inline void writeDataTo(const std::string_view data) override { ::ConsoleOps::writeDataTo(data); }
};

/**
 * @brief Test Fixture for ConsoleOps
 * * This class is used to test the ConsoleOps class.
 * * It inherits from CommonTestDataGenerator to generate random data for testing.
 * * It provides various utility functions to generate random text, binary data, and hex data streams.
 * * It also provides functions to test the binary data validity and write data to the console.
 * * @note This class is used to test the ConsoleOps class and its derived classes.
 */
class ConsoleOpsTest : public CommonTestDataGenerator
{
    public:
        /**
         * @brief Genrate a random text string of specified length
         * * This function generates a random text string of specified length using a predefined character set.
         * * The character set includes digits, uppercase and lowercase letters, and some common punctuation marks.
         * * The function uses a random number generator to select characters from the character set.
         * 
         * @param textLen The length of the random text string to be generated
         * @note The function uses std::random_device for seeding the random number generator and std::mt19937 for generating random numbers.
         * @note The function reserves memory for the result string to avoid multiple allocations.
         * @note The character set is defined as a static constant array to avoid re-initialization on each function call.
         * @note The function returns a std::string containing the generated random text.
         * @return std::string The generated random text string of specified length
         * @see CommonTestDataGenerator::generateRandomText() for more details
         * @see This function is used to generate random text for testing purposes in ConsoleOpsTest.
         */
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
        /**
         * @brief Test Hex Data Stream
         * * This function generates a random hex data stream of specified bits and writes it to the console.
         * * It uses the RandomHexGenerator class to generate random hex data.
         * * The function then validates the written data by comparing it with the generated hex data.
         *
         * @param bits The number of bits for the hex data (8, 16, 32, or 64)
         */
        template<typename DataType>
        static void testHexDataStream(const int bits)
        {
            std::uintmax_t maxTextSize = 1024;
            RandomHexGenerator hexGenerator;
            std::vector<uint8_t> hexData(maxTextSize);
            for (size_t idx = 0; idx < hexData.size(); ++idx)
            {
                if (8 == bits)
                    hexData[idx] = hexGenerator.get8();
                else if (16 == bits)
                    hexData[idx] = hexGenerator.get16();
                else if (32 == bits)
                    hexData[idx] = hexGenerator.get32();
                else if (64 == bits)
                    hexData[idx] = hexGenerator.get64();
                else
                    ASSERT_FALSE(true);
            }

            ConsoleOpsTestClass testObj;
            testObj.setTestingModeOn();
            testObj.write(hexData);

            const auto& consoleContents = testObj.getTestStringStreamFromConsole();
            testObj.setTestingModeOff();
            testBinaryDataValidity<DataType, uint8_t>(consoleContents, hexData, 64);
        }
        /**
         * @brief Test Binary Data Validity
         * * This function validates the binary data written to the console by comparing it with the expected binary data.
         * * It reads the console output line by line and converts each line to a binary representation.
         * * The function then compares the converted binary data with the expected binary data.
         *
         * @param consoleContents The console output as a string stream
         * @param binData The expected binary data as a vector
         * @param bits The number of bits for the binary data (8, 16, 32, or 64)
         */
        template<typename DataType, typename vecDataType>
        static void testBinaryDataValidity(const std::ostringstream& consoleContents, 
                                            const std::vector<vecDataType>& binData,
                                            const int bits)
        {
            std::istringstream iss(consoleContents.str());
            std::string line;
            size_t consoleLineCnt = 0;
            auto dataItr = binData.begin();
            while (std::getline(iss, line))
            {
                ASSERT_FALSE(line.empty());
                DataType consoleDataBin = 0;
                switch (bits)
                {
                    case 8:
                        consoleDataBin = static_cast<DataType>(std::bitset<8>(line).to_ulong());
                        break;
                    case 16:
                        consoleDataBin = static_cast<DataType>(std::bitset<16>(line).to_ulong());
                        break;
                    case 32:
                        consoleDataBin = static_cast<DataType>(std::bitset<32>(line).to_ulong());
                        break;
                    case 64:
                        consoleDataBin = static_cast<DataType>(std::bitset<64>(line).to_ulong());
                        break;
                    default:
                        ASSERT_TRUE(false);
                        break;
                }
                EXPECT_EQ(consoleDataBin, *dataItr);
                line.clear();
                ++dataItr;
                ++consoleLineCnt;
            }
            EXPECT_EQ(consoleLineCnt, binData.size());
        }
        /**
         * @brief Test Binary Data and Stream
         * * This function tests writing binary data to the console or a data stream.
         * * It uses the ConsoleOpsTestClass to write the binary data and then validates the written data.
         *
         * @param bits The number of bits for the binary data (8, 16, 32, or 64)
         * @param isDataStream Whether to write the data as a stream or not
         * @param bindata The binary data to be written as a vector
         */
        template<typename DataType>
        static void testBinaryDataAndStream(const int bits, 
                                            const bool isDataStream,
                                            const std::vector<DataType>& bindata)
        {
            ConsoleOpsTestClass testObj;
            testObj.setTestingModeOn();
            if (isDataStream) // If is data stream then write the data at one go
            // otherwise write the data one by one
            {
                testObj.write(bindata);
            }
            else
            {
                for (const auto& data : bindata)
                    testObj.write(data);
            }

            const auto& consoleContents = testObj.getTestStringStreamFromConsole();
            testBinaryDataValidity<DataType, DataType>(consoleContents, bindata, bits);
            testObj.setTestingModeOff();
        }
};

TEST_F(ConsoleOpsTest, testWriteToConsole)
{
    std::uintmax_t maxTextSize = 255;
    auto text = generateRandomText(maxTextSize);
    ConsoleOpsTestClass testObj;
    testObj.setTestingModeOn();
    testObj.write(text);
    auto& result = testObj.getTestStringStreamFromConsole();
    text += "\n";   // A new line character as the outstream object will have one as well
    EXPECT_EQ(text, result.str());
    testObj.setTestingModeOff();
}

TEST_F(ConsoleOpsTest, testWriteToConsoleInLoop)
{
    std::uintmax_t maxTextSize = 255;
    ConsoleOpsTestClass testObj;
    testObj.setTestingModeOn();
    std::vector<std::string> textQueue;
    for (auto cnt = 0; cnt < 200; ++cnt)
    {
        auto text = generateRandomText(maxTextSize);
        testObj.write(text);
        textQueue.emplace_back(text);
    }
    auto textItr = textQueue.begin();
    auto& consoleTexts = testObj.getTestStringStreamFromConsole();
    std::istringstream iss(consoleTexts.str());
    std::string line;
    while (std::getline(iss, line))
    {
        ASSERT_FALSE(line.empty());
        EXPECT_EQ(line, *textItr);
        line.clear();
        ++textItr;
    }
    testObj.setTestingModeOff();
}

TEST_F(ConsoleOpsTest, testWriteConsoleWith_1_ByteBinaryData)
{
    std::uintmax_t maxTextSize = 1024;
    auto bindata = generateRandomBinary_1_Byte_Data(maxTextSize);
    testBinaryDataAndStream<uint8_t>(16, false, bindata);
}

TEST_F(ConsoleOpsTest, testWriteConsoleWith_1_ByteBinaryDataStream)
{
    std::uintmax_t maxTextSize = 1024;
    auto bindata = generateRandomBinary_1_Byte_Data(maxTextSize);
    testBinaryDataAndStream<uint8_t>(16, true, bindata);
}

TEST_F(ConsoleOpsTest, testWriteConsoleWith_2_ByteBinaryData)
{
    std::uintmax_t maxTextSize = 1024;
    auto bindata = generateRandomBinary_2_Bytes_Data(maxTextSize);
    testBinaryDataAndStream<uint16_t>(16, false, bindata);
}

TEST_F(ConsoleOpsTest, testWriteConsoleWith_2_ByteBinaryDataStream)
{
    std::uintmax_t maxTextSize = 1024;
    auto bindata = generateRandomBinary_2_Bytes_Data(maxTextSize);
    testBinaryDataAndStream<uint16_t>(16, true, bindata);
}

TEST_F(ConsoleOpsTest, testWriteConsoleWith_4_BytesBinaryData)
{
    std::uintmax_t maxTextSize = 1024;
    auto bindata = generateRandomBinary_4_Bytes_Data(maxTextSize);
    testBinaryDataAndStream<uint32_t>(32, false, bindata);
}

TEST_F(ConsoleOpsTest, testWriteConsoleWith_4_ByteBinaryDataStream)
{
    std::uintmax_t maxTextSize = 1024;
    auto bindata = generateRandomBinary_4_Bytes_Data(maxTextSize);
    testBinaryDataAndStream<uint32_t>(32, true, bindata);
}

TEST_F(ConsoleOpsTest, testWriteConsoleWith_8_BytesBinaryData)
{
    std::uintmax_t maxTextSize = 1024;
    auto bindata = generateRandomBinary_8_Bytes_Data(maxTextSize);
    testBinaryDataAndStream<uint64_t>(64, false, bindata);
}

TEST_F(ConsoleOpsTest, testWriteFileWith_8_ByteBinaryDataStream)
{
    std::uintmax_t maxTextSize = 1024;
    auto bindata = generateRandomBinary_8_Bytes_Data(maxTextSize);
    testBinaryDataAndStream<uint64_t>(64, true, bindata);
}

TEST_F(ConsoleOpsTest, testWriteConsoleWith_1_Byte_Hex_DataStream)
{
    testHexDataStream<uint8_t>(8);
}

TEST_F(ConsoleOpsTest, testWriteConsoleWith_2_Bytes_Hex_DataStream)
{
    testHexDataStream<uint16_t>(16);
}

TEST_F(ConsoleOpsTest, testWriteConsoleWith_4_Bytes_Hex_DataStream)
{
    testHexDataStream<uint32_t>(32);
}

TEST_F(ConsoleOpsTest, testWriteConsoleWith_8_Bytes_Hex_DataStream)
{
    testHexDataStream<uint64_t>(64);
}

