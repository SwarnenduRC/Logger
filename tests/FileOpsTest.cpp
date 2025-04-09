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
        FileOps fileOps(maxFileSize, fileName);
        fileOps.setFileExtension(expFileExtn);
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

TEST_F(FileOpsTests, testCopyConstructor)
{
    std::uintmax_t maxFileSize = 1024;
    auto fileName = getPathSeperator() + "tmp" + getPathSeperator() + "test" + getPathSeperator() + "TestFile.log";
    auto expFileExtn = fileName.substr(fileName.find_last_of('.'));
    auto expFilePath = fileName.substr(0, fileName.find_last_of(getPathSeperator()) + 1);
    {
        FileOps fileOps(maxFileSize);
        fileOps.setFileName(fileName);

        FileOps fileOpsCopy = fileOps;
        std::filesystem::path expFilePathObj(fileName);
        auto expFileName = fileName.substr(fileName.find_last_of(getPathSeperator()) + 1);

        ASSERT_EQ(fileOpsCopy.getFileName(), expFileName);
        EXPECT_EQ(fileOpsCopy.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOpsCopy.getFilePath(), expFilePath);
        EXPECT_EQ(fileOpsCopy.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOpsCopy.getFileContent().empty());
    }
}

TEST_F(FileOpsTests, testMoveConstructor)
{
    std::uintmax_t maxFileSize = 1024;
    auto fileName = getPathSeperator() + "tmp" + getPathSeperator() + "test" + getPathSeperator() + "TestFile.log";
    auto expFileExtn = fileName.substr(fileName.find_last_of('.'));
    auto expFilePath = fileName.substr(0, fileName.find_last_of(getPathSeperator()) + 1);
    {
        FileOps fileOps(maxFileSize);
        fileOps.setFileName(fileName);

        FileOps fileOpsCopy = std::move(fileOps);
        std::filesystem::path expFilePathObj(fileName);
        auto expFileName = fileName.substr(fileName.find_last_of(getPathSeperator()) + 1);

        ASSERT_EQ(fileOpsCopy.getFileName(), expFileName);
        EXPECT_EQ(fileOpsCopy.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOpsCopy.getFilePath(), expFilePath);
        EXPECT_EQ(fileOpsCopy.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOpsCopy.getFileContent().empty());

        ASSERT_EQ(fileOps.getFileName(), nullString);
        EXPECT_EQ(fileOps.getFileExtension(), nullString);
        EXPECT_EQ(fileOps.getFilePath(), nullString);
        EXPECT_EQ(fileOps.getFilePathObj().string(), nullString);
        ASSERT_EQ(static_cast<int>(fileOps.getMaxFileSize()), 0);
    }
}

TEST_F(FileOpsTests, testCopyAssignment)
{
    std::uintmax_t maxFileSize = 1024;
    auto fileName = getPathSeperator() + "tmp" + getPathSeperator() + "test" + getPathSeperator() + "TestFile.log";
    auto expFileExtn = fileName.substr(fileName.find_last_of('.'));
    auto expFilePath = fileName.substr(0, fileName.find_last_of(getPathSeperator()) + 1);
    {
        FileOps fileOps(maxFileSize);
        fileOps.setFileName(fileName);

        FileOps fileOpsCopy(maxFileSize);
        fileOpsCopy = fileOps;
        std::filesystem::path expFilePathObj(fileName);
        auto expFileName = fileName.substr(fileName.find_last_of('/') + 1);

        ASSERT_EQ(fileOpsCopy.getFileName(), expFileName);
        EXPECT_EQ(fileOpsCopy.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOpsCopy.getFilePath(), expFilePath);
        EXPECT_EQ(fileOpsCopy.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOpsCopy.getFileContent().empty());
    }
}

TEST_F(FileOpsTests, testMoveAssignment)
{
    std::uintmax_t maxFileSize = 1024;
    auto fileName = getPathSeperator() + "tmp" + getPathSeperator() + "test" + getPathSeperator() + "TestFile.log";
    auto expFileExtn = fileName.substr(fileName.find_last_of('.'));
    auto expFilePath = fileName.substr(0, fileName.find_last_of(getPathSeperator()) + 1);
    {
        FileOps fileOps(maxFileSize);
        fileOps.setFileName(fileName);

        FileOps fileOpsCopy(maxFileSize);
        fileOpsCopy = std::move(fileOps);
        std::filesystem::path expFilePathObj(fileName);
        auto expFileName = fileName.substr(fileName.find_last_of(getPathSeperator()) + 1);

        ASSERT_EQ(fileOpsCopy.getFileName(), expFileName);
        EXPECT_EQ(fileOpsCopy.getFileExtension(), expFileExtn);
        EXPECT_EQ(fileOpsCopy.getFilePath(), expFilePath);
        EXPECT_EQ(fileOpsCopy.getFilePathObj().string(), expFilePathObj.string());
        ASSERT_TRUE(fileOpsCopy.getFileContent().empty());

        ASSERT_EQ(fileOps.getFileName(), nullString);
        EXPECT_EQ(fileOps.getFileExtension(), nullString);
        EXPECT_EQ(fileOps.getFilePath(), nullString);
        EXPECT_EQ(fileOps.getFilePathObj().string(), nullString);
        ASSERT_EQ(static_cast<int>(fileOps.getMaxFileSize()), 0);
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

        ASSERT_TRUE(std::filesystem::exists(fileOps.getFilePathObj()));
        ASSERT_TRUE(fileOps.deleteFile());
        ASSERT_FALSE(std::filesystem::exists(fileOps.getFilePathObj()));
        ASSERT_FALSE(fileOps.deleteFile());
    }
    {
        FileOps fileOps(maxFileSize);
        std::filesystem::path expFilePathObj(expFilePath + fileName);
        fileOps.createFile(expFilePathObj);
        ASSERT_TRUE(std::filesystem::exists(expFilePathObj));
        ASSERT_TRUE(fileOps.deleteFile(expFilePathObj));
        ASSERT_FALSE(std::filesystem::exists(expFilePathObj));
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
        ASSERT_FALSE(std::filesystem::exists(fileOps.getFilePathObj()));
        EXPECT_TRUE(std::filesystem::exists(expFilePath + newFileName));
        std::filesystem::path newFilePathObj(expFilePath + newFileName);
        ASSERT_TRUE(fileOps.deleteFile(newFilePathObj));
        ASSERT_FALSE(std::filesystem::exists(newFilePathObj));
    }
}

double findMedianSortedArrays(std::vector<int>& nums1, std::vector<int>& nums2) 
{
    auto singleArrCase = [](const std::vector<int> nums)
    {
        int low = 0;
        int high = nums.size();
        auto mid = (low + high) / 2;

        if (nums.size() % 2 == 0)
            return static_cast<float>((nums[mid] + nums[mid - 1]) / 2.0);
        else
            return static_cast<float>(nums[mid]);
    };
    if (nums1.empty())
    {
        return singleArrCase(nums2);
    }
    else if (nums2.empty())
    {
        return singleArrCase(nums1);
    }
    else if (nums1.size() > nums2.size())
    {
        findMedianSortedArrays(nums2, nums1);
    }

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

