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

#include <gtest/gtest.h>

#include <random>

#define nullString ""

class FileOpsTests : public ::testing::Test
{
    public:
        static std::string getPathSeperator()
        {
            static std::string pathSep = nullString;
            if (!pathSep.empty())
                return pathSep;

#ifdef _WIN32
            pathSep += "\\"
#elif __linux__ || __APPLE__ || __FreeBSD__ || __OpenBSD__ || __NetBSD__ || __MACH__
            pathSep += "/";
#endif
            return pathSep;
        }
        static std::string generateRandomText(const size_t textLen)
        {
            static const char charset[] =
            {
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                //" .,!?;:-_+=()[]{}<>|/@#$%^&*~`"
                //As the same function is used to genrate random file names thus
                // better to avoid special char set otherwise in some rare cases
                // it may failed to create files with some special chars in the name
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
        static std::vector<uint8_t> generateRandomBinary_1_Byte_Data(const size_t byteLen)
        {
            std::random_device rd;   // Random seed
            std::mt19937 gen(rd());  // Mersenne Twister engine
            std::uniform_int_distribution<> dist(0, 255); //One byte

            std::vector<uint8_t> binaryStream(byteLen);
            for (size_t idx = 0; idx < byteLen; ++idx)
                binaryStream[idx] = static_cast<uint8_t>(dist(gen));

            return binaryStream;
        }
        static std::vector<uint16_t> generateRandomBinary_2_Bytes_Data(const size_t byteLen)
        {
            std::random_device rd;   // Random seed
            std::mt19937 gen(rd());  // Mersenne Twister engine
            std::uniform_int_distribution<> dist(0, 511); //Two bytes

            std::vector<uint16_t> binaryStream(byteLen);
            for (size_t idx = 0; idx < byteLen; ++idx)
                binaryStream[idx] = static_cast<uint16_t>(dist(gen));

            return binaryStream;
        }
        static std::vector<uint32_t> generateRandomBinary_4_Bytes_Data(const size_t byteLen)
        {
            std::random_device rd;   // Random seed
            std::mt19937 gen(rd());  // Mersenne Twister engine
            std::uniform_int_distribution<> dist(0, 1023); //Four bytes

            std::vector<uint32_t> binaryStream(byteLen);
            for (size_t idx = 0; idx < byteLen; ++idx)
                binaryStream[idx] = static_cast<uint16_t>(dist(gen));

            return binaryStream;
        }
        static std::vector<uint64_t> generateRandomBinary_8_Bytes_Data(const size_t byteLen)
        {
            std::random_device rd;   // Random seed
            std::mt19937 gen(rd());  // Mersenne Twister engine
            std::uniform_int_distribution<> dist(0, 2047); //Eight bytes

            std::vector<uint64_t> binaryStream(byteLen);
            for (size_t idx = 0; idx < byteLen; ++idx)
                binaryStream[idx] = static_cast<uint16_t>(dist(gen));

            return binaryStream;
        }
        static std::string generateRandomFileName(const std::string& prefix = "tmp_", const std::string& extension = ".txt")
        {
            std::string randomPart = generateRandomText(8);  // 8-char random string
            return prefix + randomPart + extension;
        }
};
class RandomHexGenerator
{
    public:
        RandomHexGenerator()
            : rng(std::random_device{}()) {}

        uint8_t  get8()  { return get<uint8_t>(); }
        uint16_t get16() { return get<uint16_t>(); }
        uint32_t get32() { return get<uint32_t>(); }
        uint64_t get64() { return get<uint64_t>(); }

    private:
        std::mt19937 rng;

        template<typename T>
        T get()
        {
            std::uniform_int_distribution<uint64_t> dist(0, static_cast<uint64_t>(std::numeric_limits<T>::max()));
            return static_cast<T>(dist(rng));
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
    file.writeFile(text);
    ASSERT_TRUE(file.fileExists());

    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    auto data = fileContents.front();
    EXPECT_EQ(text, data->c_str());
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
    file.writeFile(text);
    ASSERT_TRUE(file.fileExists());
    for (auto cnt = 0; cnt < 200; ++cnt)
    {
        text = generateRandomText(maxTextSize);
        file.appendFile(text);
        dataQueue.push_back(text);
    }
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    for (const auto& data : dataQueue)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        EXPECT_EQ(data, fileData->c_str());
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteFileWith_1_ByteBinaryData)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    auto bindata = generateRandomBinary_1_Byte_Data(maxTextSize);
    for (const auto& data : bindata)
        file.writeFile(data);

    ASSERT_TRUE(file.fileExists());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    ASSERT_EQ(bindata.size(), fileContents.size());
    for (const auto& data : bindata)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        auto fileDataBin = static_cast<uint8_t>(std::bitset<8>(*fileData).to_ulong());
        EXPECT_EQ(data, fileDataBin);
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteFileWith_1_ByteBinaryDataStream)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    auto bindata = generateRandomBinary_1_Byte_Data(maxTextSize);
    file.writeFile(bindata);

    ASSERT_TRUE(file.fileExists());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    ASSERT_EQ(bindata.size(), fileContents.size());
    for (const auto& data : bindata)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        auto fileDataBin = static_cast<uint8_t>(std::bitset<8>(*fileData).to_ulong());
        EXPECT_EQ(data, fileDataBin);
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteFileWith_2_BytesBinaryData)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    auto bindata = generateRandomBinary_2_Bytes_Data(maxTextSize);
    for (const auto& data : bindata)
        file.writeFile(data);

    ASSERT_TRUE(file.fileExists());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    ASSERT_EQ(bindata.size(), fileContents.size());
    for (const auto& data : bindata)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        auto fileDataBin = static_cast<uint16_t>(std::bitset<16>(*fileData).to_ulong());
        EXPECT_EQ(data, fileDataBin);
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteFileWith_2_ByteBinaryDataStream)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    auto bindata = generateRandomBinary_2_Bytes_Data(maxTextSize);
    file.writeFile(bindata);

    ASSERT_TRUE(file.fileExists());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    ASSERT_EQ(bindata.size(), fileContents.size());
    for (const auto& data : bindata)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        auto fileDataBin = static_cast<uint16_t>(std::bitset<16>(*fileData).to_ulong());
        EXPECT_EQ(data, fileDataBin);
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteFileWith_4_BytesBinaryData)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    auto bindata = generateRandomBinary_4_Bytes_Data(maxTextSize);
    for (const auto& data : bindata)
        file.writeFile(data);

    ASSERT_TRUE(file.fileExists());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    ASSERT_EQ(bindata.size(), fileContents.size());
    for (const auto& data : bindata)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        auto fileDataBin = static_cast<uint32_t>(std::bitset<32>(*fileData).to_ulong());
        EXPECT_EQ(data, fileDataBin);
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteFileWith_4_ByteBinaryDataStream)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    auto bindata = generateRandomBinary_4_Bytes_Data(maxTextSize);
    file.writeFile(bindata);

    ASSERT_TRUE(file.fileExists());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    ASSERT_EQ(bindata.size(), fileContents.size());
    for (const auto& data : bindata)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        auto fileDataBin = static_cast<uint32_t>(std::bitset<32>(*fileData).to_ulong());
        EXPECT_EQ(data, fileDataBin);
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteFileWith_8_BytesBinaryData)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    auto bindata = generateRandomBinary_8_Bytes_Data(maxTextSize);
    for (const auto& data : bindata)
        file.writeFile(data);

    ASSERT_TRUE(file.fileExists());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    ASSERT_EQ(bindata.size(), fileContents.size());
    for (const auto& data : bindata)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        auto fileDataBin = static_cast<uint64_t>(std::bitset<64>(*fileData).to_ulong());
        EXPECT_EQ(data, fileDataBin);
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteFileWith_8_ByteBinaryDataStream)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    auto bindata = generateRandomBinary_8_Bytes_Data(maxTextSize);
    file.writeFile(bindata);

    ASSERT_TRUE(file.fileExists());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    ASSERT_EQ(bindata.size(), fileContents.size());
    for (const auto& data : bindata)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        auto fileDataBin = static_cast<uint64_t>(std::bitset<64>(*fileData).to_ulong());
        EXPECT_EQ(data, fileDataBin);
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteFileWith_1_Byte_Hex_DataStream)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    RandomHexGenerator hexGenerator;
    std::vector<uint8_t> hexData(maxTextSize);
    for (size_t idx = 0; idx < hexData.size(); ++idx)
        hexData[idx] = hexGenerator.get8();

    file.writeFile(hexData);

    ASSERT_TRUE(file.fileExists());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    ASSERT_EQ(hexData.size(), fileContents.size());
    for (const auto& data : hexData)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        auto fileDataBin = static_cast<uint8_t>(std::bitset<8>(*fileData).to_ulong());
        EXPECT_EQ(data, fileDataBin);
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteFileWith_2_Bytes_Hex_DataStream)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    RandomHexGenerator hexGenerator;
    std::vector<uint16_t> hexData(maxTextSize);
    for (size_t idx = 0; idx < hexData.size(); ++idx)
        hexData[idx] = hexGenerator.get16();

    file.writeFile(hexData);

    ASSERT_TRUE(file.fileExists());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    ASSERT_EQ(hexData.size(), fileContents.size());
    for (const auto& data : hexData)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        auto fileDataBin = static_cast<uint16_t>(std::bitset<16>(*fileData).to_ulong());
        EXPECT_EQ(data, fileDataBin);
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteFileWith_4_Bytes_Hex_DataStream)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    RandomHexGenerator hexGenerator;
    std::vector<uint32_t> hexData(maxTextSize);
    for (size_t idx = 0; idx < hexData.size(); ++idx)
        hexData[idx] = hexGenerator.get32();

    file.writeFile(hexData);

    ASSERT_TRUE(file.fileExists());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    ASSERT_EQ(hexData.size(), fileContents.size());
    for (const auto& data : hexData)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        auto fileDataBin = static_cast<uint32_t>(std::bitset<32>(*fileData).to_ulong());
        EXPECT_EQ(data, fileDataBin);
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteFileWith_8_Bytes_Hex_DataStream)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    RandomHexGenerator hexGenerator;
    std::vector<uint64_t> hexData(maxTextSize);
    for (size_t idx = 0; idx < hexData.size(); ++idx)
        hexData[idx] = hexGenerator.get32();

    file.writeFile(hexData);

    ASSERT_TRUE(file.fileExists());
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    ASSERT_EQ(hexData.size(), fileContents.size());
    for (const auto& data : hexData)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        auto fileDataBin = static_cast<uint64_t>(std::bitset<64>(*fileData).to_ulong());
        EXPECT_EQ(data, fileDataBin);
    }
    ASSERT_TRUE(file.deleteFile());
}

TEST_F(FileOpsTests, testWriteLargeDataChunk)
{
    std::uintmax_t maxFileSize = 1024 * 1000;
    std::uintmax_t maxTextSize = 3080;
    std::size_t maxLineLen = 1024;
    auto fileName = generateRandomFileName();
    FileOps file(maxFileSize, fileName);
    std::vector<std::string> dataQueue;
    for (auto cnt = 0; cnt < 200; ++cnt)
    {
        auto text = generateRandomText(maxTextSize);
        file.appendFile(text);
        while (text.size() > maxLineLen)
        {
            auto subText = text.substr(0, maxLineLen);
            dataQueue.push_back(subText);
            text = text.substr(maxLineLen + 1);
        }
        dataQueue.push_back(text);
    }
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    for (const auto& data : dataQueue)
    {
        auto fileData = fileContents.front();
        fileContents.pop();
        EXPECT_EQ(data, fileData->c_str());
    }
    ASSERT_TRUE(file.deleteFile());
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
    file.writeFile(text);
    ASSERT_TRUE(file.fileExists());
    for (auto cnt = 0; cnt < 2; ++cnt)
    {
        text = generateRandomText(maxTextSize);
        file.appendFile(text);
        dataQueue.push_back(text);
    }
    file.readFile();
    auto fileContents = file.getFileContent();
    ASSERT_FALSE(fileContents.empty());
    std::vector<std::string> readDataQueue;
    while (!fileContents.empty())
    {
        auto fileData = *(fileContents.front());
        fileContents.pop();
        readDataQueue.emplace_back(fileData);
    }
    for (size_t idx = 0; idx < readDataQueue.size(); ++idx)
        EXPECT_EQ(dataQueue[idx], readDataQueue[idx]);

    EXPECT_TRUE(fileContents.empty());
    for (size_t idx = 0; idx < 3; idx++)
        fileContents.emplace(std::make_shared<std::string>(readDataQueue[idx]));

    ASSERT_FALSE(fileContents.empty());

    file.clearFile();
    file.readFile();
    fileContents = file.getFileContent();
    ASSERT_TRUE(fileContents.empty());
    ASSERT_TRUE(file.deleteFile());
}

double findMedianSortedArrays(std::vector<int>& nums1, std::vector<int>& nums2) 
{
    if (nums1.size() > nums2.size())
        std::swap(nums1, nums2);

    auto low = 0;
    int high = nums1.size();

    while (low <= high)
    {
        int median1 = (low + high) / 2;
        int median2 = ((nums1.size() + nums2.size() + 1) / 2) - median1;
        // Last element from the left half of the merged array
        // which is from the frist array
        int l1 = (median1 <= 0) ? INT_MIN : nums1[median1 - 1];
        // First element from the right half of the merged array
        // which is from the frist array
        int r1 = (median1 >= static_cast<int>(nums1.size())) ? INT_MAX : nums1[median1];
        // Last element from the left half of the merged array
        // which is from the second array
        int l2 = (median2 <= 0) ? INT_MIN : nums2[median2 - 1];
        // First element from the right half of the merged array
        // which is from the second array
        int r2 = (median2 >= static_cast<int>(nums2.size())) ? INT_MAX : nums2[median2];

        if (l1 <= r2 && l2 <= r1)
        {
            if (((nums1.size() + nums2.size()) % 2) == 0)
                return static_cast<double>((std::max(l1, l2) + std::min(r1, r2)) / 2.0);
            else
                return static_cast<double>(std::max(l1, l2));
        }
        else if (l2 > r1)
        {
            low = median1 + 1;
        }
        else
        {
            high = median1 - 1;
        }
    }
    return 0.0;
}

TEST_F(FileOpsTests, testEtc)
{
    std::vector<int> nums2 = {4};
    std::vector<int> nums1 = {1, 2, 3, 5, 6};
    EXPECT_EQ(3.5, findMedianSortedArrays(nums1, nums2));
}

