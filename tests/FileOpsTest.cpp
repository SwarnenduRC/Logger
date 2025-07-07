//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter="FileOpsTests.*"
//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter=FileOpsTests.testWriteFileWith_1_Byte_Hex_DataStream

/*
 * FileOpsTest.cpp
 * Unit tests for FileOps functions using Google Test framework.
 *
 * MIT License
 *
 * Copyright (c) 2024
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
 *
 * This file contains unit tests for the FileOps class, covering file creation,
 * deletion, renaming, reading, writing, appending, and binary data operations.
 * The tests use the Google Test framework.
 */

#include "FileOps.hpp"
#include "CommonFunc.hpp"

#include <bitset>

using namespace logger;

class FileOpsTests : public CommonTestDataGenerator
{
    public:
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
            std::uintmax_t maxFileSize = 1024 * 1000;
            auto fileName = generateRandomFileName();
            FileOps file(maxFileSize, fileName);
            if (isDataStream)
            {
                file.write(bindata);
            }
            else
            {
                for (const auto& data : bindata)
                    file.write(data);
            }
            ASSERT_TRUE(file.fileExists());
            file.readFile();
            auto fileContents = file.getFileContent();
            ASSERT_FALSE(fileContents.empty());
            ASSERT_EQ(bindata.size(), fileContents.size());
            for (const auto& data : bindata)
            {
                auto fileData = fileContents.front();
                fileContents.pop();
                DataType fileDataBin;
                switch (bits)
                {
                    case 8:
                        fileDataBin = static_cast<DataType>(std::bitset<8>(fileData).to_ulong());
                        break;
                    case 16:
                        fileDataBin = static_cast<DataType>(std::bitset<16>(fileData).to_ulong());
                        break;
                    case 32:
                        fileDataBin = static_cast<DataType>(std::bitset<32>(fileData).to_ulong());
                        break;
                    case 64:
                        fileDataBin = static_cast<DataType>(std::bitset<64>(fileData).to_ulong());
                        break;
                    default:
                        ASSERT_TRUE(false);
                        break;
                }
                EXPECT_EQ(data, fileDataBin);
            }
            ASSERT_TRUE(file.deleteFile());
        }
};

TEST_F(FileOpsTests, testDefault)
{
    std::uintmax_t maxFileSize = 1024 * 1024;
    FileOps fileOps(maxFileSize);

    ASSERT_EQ(fileOps.getFileName(), nullString);
    ASSERT_EQ(fileOps.getFilePath(), nullString);
    ASSERT_EQ(fileOps.getFileExtension(), nullString);
    ASSERT_EQ(fileOps.getFilePathObj().string(), nullString);
    ASSERT_EQ(fileOps.getMaxFileSize(), maxFileSize);
    ASSERT_TRUE(fileOps.getFileContent().empty());
}

TEST_F(FileOpsTests, testParamConstructor)
{
    std::uintmax_t maxFileSize = 1024;
    // Test with file name only
    std::string fileName = "TestFile";
    std::string expFileExtn = ".txt";
    fileName += expFileExtn;
    auto expFilePath = std::filesystem::current_path().string() + getPathSeperator();
    std::filesystem::path expFilePathObj(expFilePath + fileName);
    {
        FileOps fileOps(maxFileSize, fileName);
        ASSERT_EQ(fileOps.getFileName(), fileName);
        ASSERT_EQ(fileOps.getMaxFileSize(), maxFileSize);
        EXPECT_EQ(fileOps.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());
    }
    // Test with file name (with extension)
    fileName = "TestFileSecond.log";
    expFileExtn = fileName.substr(fileName.find_last_of('.'));
    expFilePathObj = std::filesystem::path(expFilePath + fileName);
    {
        FileOps fileOps(maxFileSize, fileName);
        ASSERT_EQ(fileOps.getFileName(), fileName);
        ASSERT_EQ(fileOps.getMaxFileSize(), maxFileSize);
        EXPECT_EQ(fileOps.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());
    }
    // Test with file name (with extension) and path 1
    expFilePath = getPathSeperator() + "tmp" + getPathSeperator() + "test";
    expFilePath += getPathSeperator();
    fileName = "TestFileThird.log";
    expFileExtn = fileName.substr(fileName.find_last_of('.'));
    expFilePathObj = std::filesystem::path(expFilePath + fileName);
    {
        FileOps fileOps(maxFileSize, fileName, expFilePath);
        ASSERT_EQ(fileOps.getFileName(), fileName);
        ASSERT_EQ(fileOps.getMaxFileSize(), maxFileSize);
        EXPECT_EQ(fileOps.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());
    }
    // Test with file name (with extension) and path 2
    expFilePath = getPathSeperator() + "tmp" + getPathSeperator() + "test" + getPathSeperator();
    fileName = "TestFileFourth.log";
    expFileExtn = fileName.substr(fileName.find_last_of('.'));
    expFilePathObj = std::filesystem::path(expFilePath + fileName);
    {
        FileOps fileOps(maxFileSize, fileName, expFilePath);
        ASSERT_EQ(fileOps.getFileName(), fileName);
        ASSERT_EQ(fileOps.getMaxFileSize(), maxFileSize);
        EXPECT_EQ(fileOps.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());
    }
    fileName = "TestFileFifth";
    expFileExtn = ".log";
    {
        FileOps fileOps(maxFileSize, fileName, expFilePath, expFileExtn);

        expFilePathObj = std::filesystem::path(expFilePath + fileName + expFileExtn);
        fileName += expFileExtn;

        ASSERT_EQ(fileOps.getFileName(), fileName);
        ASSERT_EQ(fileOps.getMaxFileSize(), maxFileSize);
        EXPECT_EQ(fileOps.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());
    }
}

TEST_F(FileOpsTests, testSetFileName)
{
    std::uintmax_t maxFileSize = 1024;
    std::string fileName = "TestFile";
    std::string expFileExtn = ".txt";
    auto expFilePath = std::filesystem::current_path().string() + getPathSeperator();
    {
        FileOps fileOps(maxFileSize);
        fileOps.setFileName(fileName);
        fileName += expFileExtn;
        std::filesystem::path expFilePathObj(expFilePath + fileName);

        ASSERT_EQ(fileOps.getFileName(), fileName);
        EXPECT_EQ(fileOps.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());
    }
    fileName = "TestFileSecond.log";
    expFileExtn = fileName.substr(fileName.find_last_of('.'));
    {
        FileOps fileOps(maxFileSize);
        fileOps.setFileName(fileName);
        std::filesystem::path expFilePathObj(expFilePath + fileName);

        ASSERT_EQ(fileOps.getFileName(), fileName);
        EXPECT_EQ(fileOps.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());
    }
    fileName = getPathSeperator() + "tmp" + getPathSeperator() + "test" + getPathSeperator() + "TestFileSecond.log";
    expFileExtn = fileName.substr(fileName.find_last_of('.'));
    expFilePath = fileName.substr(0, fileName.find_last_of('/') + 1);
    {
        FileOps fileOps(maxFileSize);
        fileOps.setFileName(fileName);
        std::filesystem::path expFilePathObj(fileName);
        auto expFileName = fileName.substr(fileName.find_last_of('/') + 1);

        ASSERT_EQ(fileOps.getFileName(), expFileName);
        EXPECT_EQ(fileOps.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());
    }
}

TEST_F(FileOpsTests, testSetFilePath)
{
    std::uintmax_t maxFileSize = 1024;
    std::string fileName = "TestFile.txt";
    auto expFileExtn = fileName.substr(fileName.find_last_of('.'));
    auto expFilePath = std::filesystem::current_path().string() + getPathSeperator();
    {
        FileOps fileOps(maxFileSize, fileName);
        fileOps.setFilePath(expFilePath);
        std::filesystem::path expFilePathObj(expFilePath + fileName);

        ASSERT_EQ(fileOps.getFileName(), fileName);
        EXPECT_EQ(fileOps.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());
    }
    {
        auto expFilePath = getPathSeperator() + "tmp" + getPathSeperator() + "test";
        FileOps fileOps(maxFileSize, fileName);
        fileOps.setFilePath(expFilePath);
        std::filesystem::path expFilePathObj(expFilePath + getPathSeperator() + fileName);

        ASSERT_EQ(fileOps.getFileName(), fileName);
        EXPECT_EQ(fileOps.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOps.getFilePath(), std::string(expFilePath + getPathSeperator()));
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());
    }
    {
        FileOps fileOps(maxFileSize, fileName);
        auto expFilePath = getPathSeperator() + "tmp" + getPathSeperator() + "test" + getPathSeperator();
        fileOps.setFilePath(expFilePath);
        std::filesystem::path expFilePathObj(expFilePath + fileName);

        ASSERT_EQ(fileOps.getFileName(), fileName);
        EXPECT_EQ(fileOps.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());
    }
}

TEST_F(FileOpsTests, testSetFileExtension)
{
    std::uintmax_t maxFileSize = 1024;
    std::string fileName = "TestFile";
    std::string expFileExtn = ".txt";
    auto expFilePath = std::filesystem::current_path().string() + getPathSeperator();
    {
        FileOps fileOps(maxFileSize);
        fileOps.setFileExtension(expFileExtn).setFileName(fileName);
        fileName += expFileExtn;
        std::filesystem::path expFilePathObj(expFilePath + fileName);

        ASSERT_EQ(fileOps.getFileName(), fileName);
        EXPECT_EQ(fileOps.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());

        fileOps.setFileExtension(".log");
        fileName = "TestFile.log";
        expFilePathObj = std::filesystem::path(expFilePath + fileName);

        ASSERT_EQ(fileOps.getFileName(), fileName);
        EXPECT_EQ(fileOps.getFileExtension(), ".log");
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
    }
}

TEST_F(FileOpsTests, testCreateAndDeleteFile)
{
    std::uintmax_t maxFileSize = 1024;
    std::string fileName = "TestFile.txt";
    auto expFilePath = std::filesystem::current_path().string() + getPathSeperator();
    {
        FileOps fileOps(maxFileSize, fileName);
        fileOps.createFile();
        std::filesystem::path expFilePathObj(expFilePath + fileName);

        ASSERT_EQ(fileOps.getFileName(), fileName);
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());

        ASSERT_TRUE(fileOps.fileExists());
        ASSERT_TRUE(fileOps.deleteFile());
        ASSERT_FALSE(fileOps.fileExists());
        ASSERT_FALSE(fileOps.deleteFile());
    }
    {
        FileOps fileOps(maxFileSize);
        std::filesystem::path expFilePathObj(expFilePath + fileName);
        FileOps::createFile(expFilePathObj);
        ASSERT_TRUE(FileOps::fileExists(expFilePathObj));
        ASSERT_TRUE(FileOps::removeFile(expFilePathObj));
        ASSERT_FALSE(FileOps::fileExists(expFilePathObj));
    }
}

TEST_F(FileOpsTests, testRenameFile)
{
    std::uintmax_t maxFileSize = 1024;
    std::string fileName = "TestFile.txt";
    auto expFilePath = std::filesystem::current_path().string() + getPathSeperator();
    {
        FileOps fileOps(maxFileSize, fileName);
        fileOps.createFile();
        std::filesystem::path expFilePathObj(expFilePath + fileName);

        ASSERT_EQ(fileOps.getFileName(), fileName);
        EXPECT_EQ(fileOps.getFilePath(), expFilePath);
        EXPECT_EQ(fileOps.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOps.getFileContent().empty());

        auto newFileName = "TestFileRenamed.txt";
        ASSERT_TRUE(fileOps.renameFile(newFileName));
        ASSERT_FALSE(FileOps::fileExists(expFilePathObj));
        EXPECT_TRUE(FileOps::fileExists(newFileName));
        std::filesystem::path newFilePathObj(expFilePath + newFileName);
        ASSERT_TRUE(FileOps::removeFile(newFilePathObj));
        ASSERT_FALSE(fileOps.fileExists());
    }
}

TEST_F(FileOpsTests, testReadWrite)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 255;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    auto text = generateRandomText(maxTextSize);
    file.write(text);
    ASSERT_TRUE(file.fileExists());
    EXPECT_FALSE(file.isEmpty());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    auto data = fileContents.front();
    EXPECT_EQ(text, data);
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testAppendFile)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    std::vector<std::string> dataQueue;
    auto text = generateRandomText(maxTextSize);
    dataQueue.push_back(text);
    file.write(text);
    ASSERT_TRUE(file.fileExists());
    for (auto cnt = 0; cnt < 200; ++cnt)
    {
        text = generateRandomText(maxTextSize);
        file.append(text);
        dataQueue.push_back(text);
    }
    EXPECT_FALSE(file.isEmpty());
    file.readFile();
    EXPECT_FALSE(file.isEmpty());
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    for (const auto& data : dataQueue)
    {
        std::string fileData = fileContents.front();
        EXPECT_EQ(data, fileData);
        fileContents.pop();
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteFileWith_1_ByteBinaryData)
{
    std::uintmax_t maxTextSize = 1024;
    testBinaryDataAndStream<uint8_t>(8, false, generateRandomBinary_1_Byte_Data(maxTextSize));
}

TEST_F(FileOpsTests, testWriteFileWith_1_ByteBinaryDataStream)
{
    std::uintmax_t maxTextSize = 1024;
    testBinaryDataAndStream<uint8_t>(8, true, generateRandomBinary_1_Byte_Data(maxTextSize));
}

TEST_F(FileOpsTests, testWriteFileWith_2_BytesBinaryData)
{
    std::uintmax_t maxTextSize = 1024;
    testBinaryDataAndStream<uint16_t>(16, false, generateRandomBinary_2_Bytes_Data(maxTextSize));
}

TEST_F(FileOpsTests, testWriteFileWith_2_ByteBinaryDataStream)
{
    std::uintmax_t maxTextSize = 1024;
    testBinaryDataAndStream<uint16_t>(16, true, generateRandomBinary_2_Bytes_Data(maxTextSize));
}

TEST_F(FileOpsTests, testWriteFileWith_4_BytesBinaryData)
{
    std::uintmax_t maxTextSize = 1024;
    testBinaryDataAndStream<uint32_t>(32, false, generateRandomBinary_4_Bytes_Data(maxTextSize));
}

TEST_F(FileOpsTests, testWriteFileWith_4_ByteBinaryDataStream)
{
    std::uintmax_t maxTextSize = 1024;
    testBinaryDataAndStream<uint32_t>(32, true, generateRandomBinary_4_Bytes_Data(maxTextSize));
}

TEST_F(FileOpsTests, testWriteFileWith_8_BytesBinaryData)
{
    std::uintmax_t maxTextSize = 1024;
    testBinaryDataAndStream<uint64_t>(64, false, generateRandomBinary_8_Bytes_Data(maxTextSize));
}

TEST_F(FileOpsTests, testWriteFileWith_8_ByteBinaryDataStream)
{
    std::uintmax_t maxTextSize = 1024;
    testBinaryDataAndStream<uint64_t>(64, true, generateRandomBinary_8_Bytes_Data(maxTextSize));
}

TEST_F(FileOpsTests, testWriteFileWith_1_Byte_Hex_DataStream)
{
    std::uintmax_t maxTextSize = 1024;
    RandomHexGenerator hexGenerator;
    std::vector<uint8_t> hexData(maxTextSize);
    for (size_t idx = 0; idx < hexData.size(); ++idx)
        hexData[idx] = hexGenerator.get8();

    testBinaryDataAndStream<uint8_t>(8, true, hexData);
}

TEST_F(FileOpsTests, testWriteFileWith_2_Bytes_Hex_DataStream)
{
    std::uintmax_t maxTextSize = 1024;
    RandomHexGenerator hexGenerator;
    std::vector<uint16_t> hexData(maxTextSize);
    for (size_t idx = 0; idx < hexData.size(); ++idx)
        hexData[idx] = hexGenerator.get16();

    testBinaryDataAndStream<uint16_t>(16, true, hexData);
}

TEST_F(FileOpsTests, testWriteFileWith_4_Bytes_Hex_DataStream)
{
    std::uintmax_t maxTextSize = 1024;
    RandomHexGenerator hexGenerator;
    std::vector<uint32_t> hexData(maxTextSize);
    for (size_t idx = 0; idx < hexData.size(); ++idx)
        hexData[idx] = hexGenerator.get32();

    testBinaryDataAndStream<uint32_t>(32, true, hexData);
}

TEST_F(FileOpsTests, testWriteFileWith_8_Bytes_Hex_DataStream)
{
    std::uintmax_t maxTextSize = 1024;
    RandomHexGenerator hexGenerator;
    std::vector<uint64_t> hexData(maxTextSize);
    for (size_t idx = 0; idx < hexData.size(); ++idx)
        hexData[idx] = hexGenerator.get64();

    testBinaryDataAndStream<uint64_t>(64, true, hexData);
}

TEST_F(FileOpsTests, testWriteLargeDataChunk)
{
    std::uintmax_t maxFileSize = 4096 * 1000;
    std::uintmax_t maxTextSize = 3080;
    std::size_t maxLineLen = 4096;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    std::vector<std::string> dataQueue;
    for (auto cnt = 0; cnt < 200; ++cnt)
    {
        auto text = generateRandomText(maxTextSize);
        file.append(text);
        while (text.size() > maxLineLen)
        {
            auto subText = text.substr(0, maxLineLen);
            dataQueue.push_back(subText);
            text = text.substr(maxLineLen + 1);
        }
        dataQueue.push_back(text);
    }
    EXPECT_FALSE(file.isEmpty());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    for (const auto& data : dataQueue)
    {
        std::string fileData = fileContents.front();
        EXPECT_EQ(data, fileData);
        fileContents.pop();
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testReadFileBytesRange)
{
    std::uintmax_t maxFileSize = 1024;
    std::uintmax_t maxTextSize = 255;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    auto text = generateRandomText(maxTextSize);
    file.write(text);
    ASSERT_TRUE(file.fileExists());
    ASSERT_FALSE(file.isEmpty());

    std::vector<char> readBuf = {0};
    ASSERT_TRUE(FileOps::readFileByteRange(file, 0, 55, readBuf));
    for (size_t idx = 0; idx < readBuf.size(); ++idx)
        EXPECT_EQ(readBuf[idx], text[idx]);

    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testReadFileBytesRangeExceptionCase)
{
    std::uintmax_t maxFileSize = 1024;
    std::uintmax_t maxTextSize = 255;
    {
        auto fileName = generateRandomFileName();
        FileOps file(maxFileSize, fileName);
        auto text = generateRandomText(maxTextSize);
        file.write(text);
        ASSERT_TRUE(file.fileExists());
        ASSERT_FALSE(file.isEmpty());

        std::vector<char> readBuf = {0};
        ASSERT_FALSE(FileOps::readFileByteRange(file, 100, 1025, readBuf));

        auto allExceptions = file.getAllExceptions();
        EXPECT_FALSE(allExceptions.empty());
        EXPECT_EQ(static_cast<size_t>(1), allExceptions.size());
        ASSERT_TRUE(file.deleteFile());
    }
    {
        auto fileName = generateRandomFileName();
        FileOps file(maxFileSize, fileName);
        auto text = generateRandomText(maxTextSize);
        file.write(text);
        ASSERT_TRUE(file.fileExists());
        ASSERT_FALSE(file.isEmpty());

        std::vector<char> readBuf = {0};
        ASSERT_FALSE(FileOps::readFileByteRange(file, 100, 99, readBuf));

        auto allExceptions = file.getAllExceptions();
        EXPECT_FALSE(allExceptions.empty());
        EXPECT_EQ(static_cast<size_t>(1), allExceptions.size());
        ASSERT_TRUE(file.deleteFile());
    }
}

TEST_F(FileOpsTests, testReadFileLineRange)
{
    std::uintmax_t maxFileSize = 4096 * 1000;
    std::uintmax_t maxTextSize = 3080;
    std::size_t maxLineLen = 4096;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    std::vector<std::string> dataQueue;
    for (auto cnt = 0; cnt < 200; ++cnt)
    {
        auto text = generateRandomText(maxTextSize);
        file.append(text);
        while (text.size() > maxLineLen)
        {
            auto subText = text.substr(0, maxLineLen);
            dataQueue.push_back(subText);
            text = text.substr(maxLineLen + 1);
        }
        dataQueue.push_back(text);
    }
    ASSERT_TRUE(file.fileExists());
    ASSERT_FALSE(file.isEmpty());
    std::vector<std::string> readBuf;
    size_t startLineNo = 5;
    size_t endLineNo = 15;
    ASSERT_TRUE(FileOps::readFileLineRange(file, startLineNo, endLineNo, readBuf));
    EXPECT_FALSE(readBuf.empty());
    EXPECT_EQ((endLineNo - startLineNo + 1), readBuf.size()); //+1 for inclusive range
    auto dataQueueItr = dataQueue.cbegin();
    for (size_t begin = 1; begin < startLineNo; ++begin)
        ++dataQueueItr;

    for (size_t idx = 0; idx < readBuf.size(); ++idx)
    {
        EXPECT_EQ(readBuf[idx], *dataQueueItr);
        ++dataQueueItr;
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testReadFileLineRangeExceptionCase)
{
    std::uintmax_t maxFileSize = 1024;
    std::uintmax_t maxTextSize = 255;
    {
        auto fileName = generateRandomFileName();
        FileOps file(maxFileSize, fileName);
        auto text = generateRandomText(maxTextSize);
        file.write(text);
        ASSERT_TRUE(file.fileExists());
        ASSERT_FALSE(file.isEmpty());

        std::vector<std::string> readBuf;
        size_t startLineNo = 15;
        size_t endLineNo = 5;
        ASSERT_FALSE(FileOps::readFileLineRange(file, startLineNo, endLineNo, readBuf));

        auto allExceptions = file.getAllExceptions();
        EXPECT_FALSE(allExceptions.empty());
        EXPECT_EQ(static_cast<size_t>(1), allExceptions.size());
        ASSERT_TRUE(file.deleteFile());
    }
    {
        auto fileName = generateRandomFileName();
        FileOps file(maxFileSize, fileName);
        auto text = generateRandomText(maxTextSize);
        file.createFile();
        ASSERT_TRUE(file.fileExists());
        ASSERT_TRUE(file.isEmpty());

        std::vector<std::string> readBuf;
        size_t startLineNo = 15;
        size_t endLineNo = 25;
        ASSERT_FALSE(FileOps::readFileLineRange(file, startLineNo, endLineNo, readBuf));

        auto allExceptions = file.getAllExceptions();
        EXPECT_FALSE(allExceptions.empty());
        EXPECT_EQ(static_cast<size_t>(1), allExceptions.size());
        ASSERT_TRUE(file.deleteFile());
    }
}

TEST_F(FileOpsTests, testClearFile)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    std::vector<std::string> dataQueue;
    auto text = generateRandomText(maxTextSize);
    dataQueue.push_back(text);
    file.write(text);
    ASSERT_TRUE(file.fileExists());
    for (auto cnt = 0; cnt < 2; ++cnt)
    {
        text = generateRandomText(maxTextSize);
        file.append(text);
        dataQueue.push_back(text);
    }
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    std::vector<std::string> readDataQueue = {};
    while (!fileContents.empty())
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        readDataQueue.emplace_back(fileData);
    }
    for (size_t idx = 0; idx < readDataQueue.size(); ++idx)
        EXPECT_EQ(dataQueue[idx], readDataQueue[idx]);

    EXPECT_TRUE(fileContents.empty());
    DataQ().swap(fileContents);
    for (size_t idx = 0; idx < 3; idx++)
        fileContents.emplace(readDataQueue[idx]);

    ASSERT_FALSE(fileContents.empty());

    file.clearFile();
    file.readFile();
    fileContents = file.getFileContent();
    ASSERT_TRUE(fileContents.empty());
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testGetFileSize)
{
    std::uintmax_t maxFileSize = 1024 * 50;
    std::uintmax_t maxTextSize = 25;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    std::vector<std::string> dataQueue;
    size_t dataWrittenByteCnt = 0;
    size_t newLineCharAdjst = 1;
    for (auto cnt = 0; cnt < 100; ++cnt)
    {
        auto text = generateRandomText(maxTextSize);
        file.append(text);
        auto currFileSize = file.getFileSize();
        dataWrittenByteCnt += text.size();
        // dataWrittenByteCnt + newLineCharAdjst --> Because while writting to a file
        // we add a new line char after each line written
        EXPECT_NEAR(currFileSize, dataWrittenByteCnt + newLineCharAdjst, 26) << " Text = " << text << std::endl;
        dataQueue.push_back(text);
        ++newLineCharAdjst; // Increment it for next new line
    }
    EXPECT_FALSE(file.isEmpty());
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testFileLimitExceed)
{
    std::uintmax_t maxFileSize = 1024;
    std::uintmax_t maxTextSize = 3072;
    auto fileName = generateRandomFileName();
    auto expFilePath = std::filesystem::current_path().string() + getPathSeperator();
    FileOps file(maxFileSize, fileName);
    std::vector<std::string> dataQueue;
    for (auto cnt = 0; cnt < 400; ++cnt)
    {
        auto text = generateRandomText(maxTextSize);
        file.append(text);
        dataQueue.push_back(text);
    }
    size_t cnt = 0;
    for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
    {
        if (entry.path().extension() == ".txt" && entry.is_regular_file())
            ++cnt;
    }
    EXPECT_GT(cnt, static_cast<size_t>(1));
    for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
    {
        if (entry.path().extension() == ".txt" && entry.is_regular_file())
            ASSERT_TRUE(FileOps::removeFile(entry.path()));
    }
    size_t newCnt = 0;
    cnt = newCnt;
    for (const auto& entry : std::filesystem::directory_iterator(std::filesystem::current_path()))
    {
        if (entry.path().extension() == ".txt" && entry.is_regular_file())
            ++newCnt;
    }
    EXPECT_EQ(cnt, newCnt);
}
