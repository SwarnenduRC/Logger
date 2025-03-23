#include "FileOps.hpp"

#include <exception>

static constexpr std::string_view DEFAULT_FILE_EXTN = ".txt";

void FileOps::populateFilePathObj()
{
    if (!m_FileName.empty())
    {
        std::unique_lock<std::mutex> lock(m_FileOpsMutex);
        m_FileOpsCv.wait(lock, [this] { return !m_isFileOpsRunning; });

        if (m_FileExtension.empty())
        {
            if (m_FileName.find('.') != std::string::npos)
            {
                m_FileExtension = m_FileName.substr(m_FileName.find_last_of('.'));
            }
            else
            {
                m_FileExtension = DEFAULT_FILE_EXTN;
                m_FileName += m_FileExtension;
            }
        }
        
        auto getSeparator = []()
        {
            #ifdef _WIN32
                return "\\";
            #elif __linux__ || __APPLE__ || __FreeBSD__ || __OpenBSD__ || __NetBSD__ || __MACH__
                return "/";
            #endif
        };

        if (m_FilePath.empty())
        {
            auto pathSeparator = m_FileName.find_last_of('/');
            if (pathSeparator == std::string::npos)
                pathSeparator = m_FileName.find_last_of('\\');

            if (pathSeparator != std::string::npos)
            {
                m_FilePath = m_FileName.substr(0, pathSeparator + 1);
                m_FileName = m_FileName.substr(pathSeparator + 1);
            }
            else
            {
                m_FilePath = std::filesystem::current_path().string();
                m_FilePath += getSeparator();
            }
        }
        else
        {
            if (m_FilePath.back() != '/' && m_FilePath.back() != '\\')
            {
                m_FilePath += getSeparator();
            }
        }
        m_FilePathObj = std::filesystem::path(m_FilePath + m_FileName);
    }
    m_FileOpsCv.notify_all();
}

FileOps::FileOps(const std::uintmax_t maxFileSize,
                 const std::string_view fileName, 
                 const std::string_view filePath, 
                 const std::string_view fileExtension)
    : m_FileName(fileName)
    , m_FilePath(filePath)
    , m_FileExtension(fileExtension)
    , m_DataRecords()
    , m_MaxFileSize(maxFileSize)
{
    populateFilePathObj();
}

FileOps::~FileOps()
{
    std::unique_lock<std::mutex> lock(m_FileOpsMutex);
    m_FileOpsCv.wait(lock, [this] { return !m_isFileOpsRunning; });
}

FileOps::FileOps(const FileOps& other)
    : m_FileName(other.m_FileName)
    , m_FilePath(other.m_FilePath)
    , m_FileExtension(other.m_FileExtension)
    , m_FileContent(other.m_FileContent)
    , m_DataRecords(other.m_DataRecords)
    , m_FilePathObj(other.m_FilePathObj)
    , m_MaxFileSize(other.m_MaxFileSize)
{
}

FileOps& FileOps::operator=(const FileOps& other)
{
    std::unique_lock<std::mutex> lock(m_FileOpsMutex);
    m_FileOpsCv.wait(lock, [this] { return !m_isFileOpsRunning; });

    if (this != &other)
    {
        m_FileName = other.m_FileName;
        m_FilePath = other.m_FilePath;
        m_FileExtension = other.m_FileExtension;
        m_FileContent = other.m_FileContent;
        m_DataRecords = other.m_DataRecords;
        m_FilePathObj = other.m_FilePathObj;
        m_MaxFileSize = other.m_MaxFileSize;
    }
    m_FileOpsCv.notify_all();
    return *this;
}

FileOps::FileOps(FileOps&& other) noexcept
    : m_FileName(std::move(other.m_FileName))
    , m_FilePath(std::move(other.m_FilePath))
    , m_FileExtension(std::move(other.m_FileExtension))
    , m_FileContent(std::move(other.m_FileContent))
    , m_DataRecords(std::move(other.m_DataRecords))
    , m_FilePathObj(std::move(other.m_FilePathObj))
    , m_MaxFileSize(other.m_MaxFileSize)
{
    other.m_MaxFileSize = 0;
}

FileOps& FileOps::operator=(FileOps&& other) noexcept
{
    std::unique_lock<std::mutex> lock(m_FileOpsMutex);
    m_FileOpsCv.wait(lock, [this] { return !m_isFileOpsRunning; });

    if (this != &other)
    {
        m_FileName = std::move(other.m_FileName);
        m_FilePath = std::move(other.m_FilePath);
        m_FileExtension = std::move(other.m_FileExtension);
        m_FileContent = std::move(other.m_FileContent);
        m_DataRecords = std::move(other.m_DataRecords);
        m_FilePathObj = std::move(other.m_FilePathObj);
        m_MaxFileSize = other.m_MaxFileSize;
        other.m_MaxFileSize = 0;
    }
    m_FileOpsCv.notify_all();
    return *this;
}

void FileOps::setFileName(const std::string_view fileName)
{
    if (fileName.empty() || fileName == m_FileName)
        return;

    m_FileName = fileName;
    if (fileName.find('.') != std::string::npos)
    {
        m_FileExtension = fileName.substr(fileName.find_last_of('.'));
    }
    else
    {
        m_FileExtension = DEFAULT_FILE_EXTN;
        m_FileName += m_FileExtension;
    }
    populateFilePathObj();
}

void FileOps::setFilePath(const std::string_view filePath)
{
    if (filePath.empty() || filePath == m_FilePath)
        return;

    m_FilePath = filePath;
    populateFilePathObj();
}

void FileOps::setFileExtension(const std::string_view fileExtension)
{
    if (fileExtension.empty() || fileExtension == m_FileExtension)
        return;

    m_FileExtension = fileExtension;
    populateFilePathObj();
}

bool FileOps::createFile()
{
    std::scoped_lock<std::mutex> lock(m_FileOpsMutex);
    if (!std::filesystem::exists(m_FilePathObj))
    {
        m_isFileOpsRunning = true;
        std::ofstream file(m_FilePathObj);
        if (file.is_open())
        {
            file.close();
            m_isFileOpsRunning = false;
        }
        else
        {
            m_isFileOpsRunning = false;
            throw std::runtime_error("Failed to create file: " + m_FilePathObj.string());
        }
    }
    return true;
}

bool FileOps::deleteFile()
{
    auto retVal = false;
    std::scoped_lock<std::mutex> lock(m_FileOpsMutex);
    m_isFileOpsRunning = true;
    if (std::filesystem::exists(m_FilePathObj))
    {
        retVal = std::filesystem::remove(m_FilePathObj);
        m_isFileOpsRunning = false;
    }
    return retVal;
}

bool FileOps::renameFile(const std::string_view newFileName)
{
    if (newFileName.empty())
        return false;

    std::unique_lock<std::mutex> lock(m_FileOpsMutex);
    m_FileOpsCv.wait(lock, [this] { return !m_isFileOpsRunning; });

    std::filesystem::path newPath = m_FilePathObj.parent_path() / newFileName;
    if (std::filesystem::exists(m_FilePathObj))
    {
        m_isFileOpsRunning = true;
        std::filesystem::rename(m_FilePathObj, newPath);
        createFile();
        m_isFileOpsRunning = false;
    }
    else
    {
        m_isFileOpsRunning = false;
        m_FileOpsCv.notify_one();
        return false;
    }
    m_FileOpsCv.notify_one();
    return true;
}

void FileOps::readFile()
{
    if (m_FilePathObj.empty())
        throw std::runtime_error("File path is empty");

    std::ifstream file(m_FilePathObj);
    if (file.is_open())
    {
        std::string line;
        std::array<char, 1024> dataRecord;
        BufferQ().swap(m_FileContent);
        while (std::getline(file, line))
        {
            std::copy(line.begin(), line.end(), dataRecord.begin());
            m_FileContent.push(dataRecord);
        }
        file.close();
    }
    else
    {
        throw std::runtime_error("Failed to open file: " + m_FilePathObj.string());
    }
}

void FileOps::writeFile(const std::string_view data)
{
    if (data.empty())
        return;

    push(data);
}

void FileOps::appendFile(const std::string_view data)
{
    writeFile(data);
}

bool FileOps::clearFile()
{
    std::scoped_lock<std::mutex> lock(m_FileOpsMutex);
    if (std::filesystem::exists(m_FilePathObj))
    {
        m_isFileOpsRunning = true;
        std::ofstream file(m_FilePathObj, std::ios::out | std::ios::trunc);
        if (file.is_open())
        {
            file.close();
            m_isFileOpsRunning = false;
        }
        else
        {
            m_isFileOpsRunning = false;
            throw std::runtime_error("Failed to open file: " + m_FilePathObj.string());
        }
    }
    return true;
}

void FileOps::push(const std::string_view data)
{
    if (data.empty())
        return;

    auto push = [this](std::array<char, 1024>& dataRecord, const std::string_view data)
    {
        std::copy(data.begin(), data.end(), dataRecord.begin());
        m_DataRecords.push(dataRecord);
    };
    
    std::array<char, 1024> dataRecord;
    std::scoped_lock<std::mutex> lock(m_DataRecordsMtx);
    if (data.size() > dataRecord.size())
    {
        std::string dataCopy = data.data();
        while (dataCopy.size() > dataRecord.size())
        {
            push(dataRecord, dataCopy.substr(0, dataRecord.size()));
            dataRecord.fill('\0');
            dataCopy = dataCopy.substr(dataRecord.size());
        }
        if (!dataCopy.empty())
        {
            push(dataRecord, dataCopy);
        }
    }
    else
    {
        push(dataRecord, data);
    }
    m_DataRecordsCv.notify_one();
}

bool FileOps::pop(std::array<char, 1024>& dataRecord)
{
    std::unique_lock<std::mutex> lock(m_DataRecordsMtx);
    m_DataRecordsCv.wait(lock, [this] { return m_StopAndExit || !m_DataRecords.empty(); });

    if (m_StopAndExit && m_DataRecords.empty())
        return false;
        
    dataRecord = std::move(m_DataRecords.front());
    m_DataRecords.pop();
    return true;
}

void FileOps::writeDataToFile()
{
    if (m_FilePathObj.empty())
        throw std::runtime_error("File path is empty");
    
    if (!std::filesystem::exists(m_FilePathObj))
        createFile();
    
    std::array<char, 1024> dataRecord;
    std::ofstream file(m_FilePathObj, std::ios::out | std::ios::app | std::ios::binary);
    while (pop(dataRecord))
    {
        if (file.is_open())
        {
            m_isFileOpsRunning = true;
            std::scoped_lock<std::mutex> lock(m_FileOpsMutex);
            file.write(dataRecord.data(), dataRecord.size());
            file.write("\n", 1);
            m_isFileOpsRunning = false;
        }
        else
        {
            m_isFileOpsRunning = false;
            throw std::runtime_error("Failed to open file: " + m_FilePathObj.string());
        }
    }
    file.close();
}

