//leaks --atExit --list -- ./bin/TestLogger_d --gtest_shuffle --gtest_repeat=3 --gtest_filter="FileOpsTests.*"

#include "FileOps.hpp"

#include <gtest/gtest.h>

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
    expFilePath = "/tmp/test";
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
    expFilePath = "/tmp/test/";
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
    expFilePath = "/tmp/test/";
    fileName = "TestFileFifth";
    expFileExtn = ".log";
    expFilePathObj = std::filesystem::path(expFilePath + fileName);
    {
        FileOps fileOps(maxFileSize, fileName, expFilePath, expFileExtn);
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
}

