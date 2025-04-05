#ifndef FILEOPS_HPP
#define FILEOPS_HPP

#include <fstream>
#include <string>
#include <vector>
#include <filesystem>
#include <mutex>
#include <condition_variable>

using BufferQ = std::queue<std::array<char, 1024>>;

class FileOps
{
    public:
        using StdTupple = std::tuple<std::string, std::string, std::string>;

        FileOps() = delete;
        FileOps(const std::uintmax_t maxFileSize,
                const std::string_view fileName = "", 
                const std::string_view filePath = "", 
                const std::string_view fileExtension = "");
        ~FileOps();
        FileOps(const FileOps&);
        FileOps& operator=(const FileOps&);
        FileOps(FileOps&&) noexcept;
        FileOps& operator=(FileOps&&) noexcept;

        void setFileName(const std::string_view fileName);
        void setFilePath(const std::string_view filePath);
        void setFileExtension(const std::string_view fileExtension);

        inline void setMaxFileSize(const std::uintmax_t fileSize)       { m_MaxFileSize = fileSize;                         }
        inline std::string getFileName() const                          { return m_FileName;                                }
        inline std::string getFilePath() const                          { return m_FilePath;                                }
        inline std::string getFileExtension() const                     { return m_FileExtension;                           }
        inline std::filesystem::path getFilePathObj() const             { return m_FilePathObj;                             }
        inline std::uintmax_t getMaxFileSize() const                    { return m_MaxFileSize;                             }
        inline BufferQ getFileContent()                                 { return m_FileContent;                             }
        inline bool isFileEmpty() const                                 { return std::filesystem::is_empty(m_FilePathObj);  }
        inline std::uintmax_t getFileSize() const                       { return std::filesystem::file_size(m_FilePathObj); }

        void writeFile(const std::string_view data);
        void appendFile(const std::string_view data);
        bool renameFile(const std::string_view newFileName);
        void readFile();
        bool createFile();
        bool deleteFile();
        bool clearFile();
    
    private:
        void populateFilePathObj(const StdTupple& fileDetails);
        void writeDataToFile();
        bool pop(std::array<char, 1024>& dataRecord);
        void push(const std::string_view data);

        std::string m_FileName;
        std::string m_FilePath;
        std::string m_FileExtension;
        BufferQ m_FileContent;
        BufferQ m_DataRecords;
        std::filesystem::path m_FilePathObj;
        std::uintmax_t m_MaxFileSize;

        std::mutex m_FileOpsMutex;
        std::mutex m_DataRecordsMtx;
        std::condition_variable m_FileOpsCv;
        std::condition_variable m_DataRecordsCv;
        std::atomic_bool m_isFileOpsRunning = false;
        std::atomic_bool m_StopAndExit = false;
};

#endif // FILEOPS_HPP