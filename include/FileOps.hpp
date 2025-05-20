#ifndef FILEOPS_HPP
#define FILEOPS_HPP

#include <fstream>
#include <string>
#include <queue>
#include <filesystem>
#include <thread>

using BufferQ = std::queue<std::array<char, 1025>>;
using DataQ = std::queue<std::shared_ptr<std::string>>;

class FileOps
{
    public:
        using StdTupple = std::tuple<std::string, std::string, std::string>;

        FileOps() = delete;
        FileOps(const std::uintmax_t maxFileSize,
                const std::string_view fileName = "", 
                const std::string_view filePath = "", 
                const std::string_view fileExtension = "");
        virtual ~FileOps();
        FileOps(const FileOps&) = delete;
        FileOps& operator=(const FileOps&) = delete;
        FileOps(FileOps&&) = delete;
        FileOps& operator=(FileOps&&) = delete;

        FileOps& setFileName(const std::string_view fileName);
        FileOps& setFilePath(const std::string_view filePath);
        FileOps& setFileExtension(const std::string_view fileExtension);

        inline FileOps& setMaxFileSize(const std::uintmax_t fileSize)   { m_MaxFileSize = fileSize; return *this;           }
        inline std::string getFileName() const                          { return m_FileName;                                }
        inline std::string getFilePath() const                          { return m_FilePath;                                }
        inline std::string getFileExtension() const                     { return m_FileExtension;                           }
        inline std::filesystem::path getFilePathObj() const             { return m_FilePathObj;                             }
        inline std::uintmax_t getMaxFileSize() const                    { return m_MaxFileSize;                             }
        inline DataQ getFileContent() const                             { return m_FileContent;                             }
        inline bool isFileEmpty() const                                 { return std::filesystem::is_empty(m_FilePathObj);  }
        inline bool fileExists() const                                  { return std::filesystem::exists(m_FilePathObj);    }

        std::uintmax_t getFileSize();
        void writeFile(const std::string_view data);
        void appendFile(const std::string_view data);
        bool renameFile(const std::string_view newFileName);
        void readFile();
        bool createFile();
        bool deleteFile();
        bool clearFile();

        static bool isFileEmpty(const std::filesystem::path& file);
        static bool isFileExists(const std::filesystem::path& file);
        static bool removeFile(const std::filesystem::path& file);
        static bool clearFile(const std::filesystem::path& file);
        static bool createFile(const std::filesystem::path& file);
        static const std::vector<std::exception_ptr>& getAllExceptions()    { return m_excpPtrVec; }
    
    private:
        void populateFilePathObj(const StdTupple& fileDetails);
        void keepWatchAndPull();
        void writeToFile(BufferQ&& dataQueue, std::exception_ptr& excpPtr);
        bool pop(BufferQ& data);
        void push(const std::string_view data);

        std::string m_FileName;
        std::string m_FilePath;
        std::string m_FileExtension;
        DataQ m_FileContent;
        BufferQ m_DataRecords;
        std::filesystem::path m_FilePathObj;
        std::uintmax_t m_MaxFileSize;

        std::mutex m_FileOpsMutex;
        std::mutex m_DataRecordsMtx;
        std::condition_variable m_FileOpsCv;
        std::condition_variable m_DataRecordsCv;
        std::atomic_bool m_isFileOpsRunning = false;
        std::atomic_bool m_dataReady = false;
        std::atomic_bool m_shutAndExit = false;
        std::thread m_watcher;

        static std::vector<std::exception_ptr> m_excpPtrVec;
};

#endif // FILEOPS_HPP