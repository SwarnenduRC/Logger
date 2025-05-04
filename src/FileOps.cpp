#include "FileOps.hpp"

#include <exception>
#include <tuple>
#include <memory>
#include <future>
#include <cstdio>
#include <iostream>

#define nullString ""

static constexpr std::string_view DEFAULT_FILE_EXTN = ".txt";
std::vector<std::exception_ptr> FileOps::m_excpPtrVec = {0};

void FileOps::populateFilePathObj(const StdTupple& fileDetails)
{
    std::unique_lock<std::mutex> lock(m_FileOpsMutex);
    m_FileOpsCv.wait(lock, [this] { return !m_isFileOpsRunning; });

    m_isFileOpsRunning = true;

    if (!std::get<0>(fileDetails).empty())
        m_FileName = std::get<0>(fileDetails);

    if (!std::get<1>(fileDetails).empty())
        m_FilePath = std::get<1>(fileDetails);

    if (!std::get<2>(fileDetails).empty())
        m_FileExtension = std::get<2>(fileDetails);

    if (!m_FileName.empty())
    {
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
        else
        {
            m_FileName = m_FileName.substr(0, m_FileName.find_last_of('.'));
            m_FileName += m_FileExtension;
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
    m_isFileOpsRunning = false;
    m_FileOpsCv.notify_one();
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
    auto fileDetails = std::make_tuple(m_FileName, m_FilePath, m_FileExtension);
    populateFilePathObj(fileDetails);
    m_watcher = std::thread(&FileOps::keepWatchAndPull, this);
}

FileOps::~FileOps()
{
    std::unique_lock<std::mutex> dataLock(m_DataRecordsMtx);
    m_shutAndExit = true;
    dataLock.unlock();
    m_DataRecordsCv.notify_one();

    if (m_watcher.joinable())
        m_watcher.join();
}

void FileOps::setFileName(const std::string_view fileName)
{
    if (fileName.empty() || fileName == m_FileName)
        return;

    populateFilePathObj(std::make_tuple(std::string(fileName), nullString, nullString));
}

void FileOps::setFilePath(const std::string_view filePath)
{
    if (filePath.empty() || filePath == m_FilePath)
        return;

    populateFilePathObj(std::make_tuple(nullString, std::string(filePath), nullString));
}

void FileOps::setFileExtension(const std::string_view fileExtension)
{
    if (fileExtension.empty() || fileExtension == m_FileExtension)
        return;

    populateFilePathObj(std::make_tuple(nullString, nullString, std::string(fileExtension)));
}

std::uintmax_t FileOps::getFileSize()
{
    if (fileExists())
    {
        std::scoped_lock<std::mutex> fileLock(m_FileOpsMutex);
        return std::filesystem::file_size(m_FilePathObj);
    }
    return 0;
}

bool FileOps::createFile()
{
    auto retVal = false;
    if (!fileExists())
    {
        std::scoped_lock<std::mutex> lock(m_FileOpsMutex);
        m_isFileOpsRunning = true;
        std::ofstream file(m_FilePathObj);
        if (file.is_open())
        {
            file.close();
            retVal = true;
        }
    }
    m_isFileOpsRunning = false;
    m_FileOpsCv.notify_one();
    return retVal;
}

bool FileOps::createFile(const std::filesystem::path& file)
{
    if (file.empty())
        return false;

    if (m_FilePathObj == file)
    {
        return createFile();
    }
    else
    {
        std::ofstream FILE(file);
        if (FILE.is_open())
        {
            FILE.close();
            return true;
        }
        return false;
    }
}

bool FileOps::deleteFile()
{
    auto retVal = false;
    if (fileExists())
    {
        std::unique_lock<std::mutex> fileLock(m_FileOpsMutex);
        m_FileOpsCv.wait(fileLock, [this] { return !m_isFileOpsRunning; });
        m_isFileOpsRunning = true;
        retVal = std::filesystem::remove(m_FilePathObj);
        m_isFileOpsRunning = false;
        fileLock.unlock();
        m_FileOpsCv.notify_one();
    }
    return retVal;
}

bool FileOps::deleteFile(const std::filesystem::path& file)
{
    if (file.empty())
        return false;

    if (m_FilePathObj == file)
    {
        return deleteFile();
    }
    else
    {
        return std::filesystem::remove(file);
    }
}

bool FileOps::renameFile(const std::string_view newFileName)
{
    if (newFileName.empty())
        return false;

    auto success = false;

    if (fileExists() && newFileName != m_FileName)
    {
        std::unique_lock<std::mutex> lock(m_FileOpsMutex);
        m_FileOpsCv.wait(lock, [this] { return !m_isFileOpsRunning; });
        m_isFileOpsRunning = true;

        std::filesystem::path newPath = m_FilePathObj.parent_path() / newFileName;
        std::filesystem::rename(m_FilePathObj, newPath);
        m_isFileOpsRunning = false;
        lock.unlock();
        m_FileOpsCv.notify_all();
        success = true;
    }
    return success;
}

void FileOps::readFile()
{
    if (m_FilePathObj.empty())
        throw std::runtime_error("File path is empty");

    {
        std::unique_lock<std::mutex> dataLock(m_DataRecordsMtx);
        if (!m_DataRecords.empty())
        {
            m_dataReady = true;
            dataLock.unlock();
            m_DataRecordsCv.notify_one();
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    //BufferQ fileContents;
    std::unique_lock<std::mutex> fileLock(m_FileOpsMutex);
    m_FileOpsCv.wait(fileLock, [this]{ return !m_isFileOpsRunning; });
    m_isFileOpsRunning = true;
    if (std::filesystem::exists(m_FilePathObj))
    {
        std::ifstream file(m_FilePathObj);
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                m_FileContent.emplace(std::make_shared<std::string>(line.c_str()));
                //std::cout << "Read line = " << dataRecord.data() << std::endl;
                //fileContents.emplace(dataRecord);
                line.clear();
            }
            file.close();
        }
        else
        {
            m_isFileOpsRunning = false;
            fileLock.unlock();
            m_FileOpsCv.notify_all();
            throw std::runtime_error("Failed to open file: " + m_FilePathObj.string());
        }
    }
    else
    {
        //std::cout << "File " << m_FilePathObj.string() << " doesn't exist" << std::endl << std::endl;
    }
    m_isFileOpsRunning = false;
    fileLock.unlock();
    m_FileOpsCv.notify_all();
    //return fileContents;
}

void FileOps::writeFile(const std::string_view data)
{
    if (data.empty())
        return;

    if (!fileExists())
    {
        if (createFile())
            push(data);
        else
            throw std::runtime_error("File neither exists nor can be created");
    }
    else
    {
        push(data);
    }
}

void FileOps::appendFile(const std::string_view data)
{
    writeFile(data);
}

bool FileOps::clearFile()
{
    auto retVal = false;
    std::unique_lock<std::mutex> fileLock(m_FileOpsMutex);
    m_FileOpsCv.wait(fileLock, [this]{ return !m_isFileOpsRunning; });
    m_isFileOpsRunning = true;

    if (fileExists())
    {
        std::ofstream file(m_FilePathObj, std::ios::out | std::ios::trunc);
        if (file.is_open())
        {
            file.close();
            retVal = true;
        }
    }
    m_isFileOpsRunning = false;
    fileLock.unlock();
    m_FileOpsCv.notify_all();
    return retVal;
}

void FileOps::push(const std::string_view data)
{
    if (data.empty())
        return;

    auto push = [this](std::array<char, 1025>& dataRecord, const std::string_view data)
    {
        std::copy(data.begin(), data.end(), dataRecord.begin());
        if (data.size() < dataRecord.size())
        {
            auto dataSize = data.size();
            std::fill(dataRecord.begin() + dataSize, dataRecord.end(), '\0');
        }
        m_DataRecords.push(dataRecord);
    };
    
    {
        std::array<char, 1025> dataRecord;
        std::scoped_lock<std::mutex> lock(m_DataRecordsMtx);
        if (data.size() > dataRecord.size())
        {
            std::string dataCopy = data.data();
            while (dataCopy.size() > dataRecord.size())
            {
                push(dataRecord, dataCopy.substr(0, dataRecord.size() - 1));
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
    }
    if (m_DataRecords.size() == 2)
    {
        m_dataReady = true;
        m_DataRecordsCv.notify_one();
    }
}

bool FileOps::pop(BufferQ& data)
{
    if (m_DataRecords.empty())
        return false;

    BufferQ().swap(data);
    data.swap(m_DataRecords);
    m_dataReady = false;

    return true;
}

void FileOps::keepWatchAndPull()
{
    BufferQ dataq;
    do
    {
        std::unique_lock<std::mutex> dataLock(m_DataRecordsMtx);
        m_DataRecordsCv.wait(dataLock, [this]{ return m_dataReady || m_shutAndExit.load(); });

        auto success = pop(dataq);
        dataLock.unlock();
        m_DataRecordsCv.notify_one();
        std::thread writerThread;
        if (success)
        {
            std::exception_ptr excpPtr = nullptr;
            m_excpPtrVec.emplace_back(excpPtr);
            writerThread = std::thread([this, &dataq, &excpPtr](){ writeToFile(std::move(dataq), excpPtr); });
        }
        if (writerThread.joinable())
            writerThread.join();

        if (m_shutAndExit)
            break;
    } while (true);

    //std::cout << std::endl << std::endl << "***********************************" << std::endl << std::endl;
}

void FileOps::writeToFile(BufferQ&& dataQueue, std::exception_ptr& excpPtr)
{
    if (dataQueue.empty())
        return;

    try
    {
        std::string errMsg;
        std::unique_lock<std::mutex> fileLock(m_FileOpsMutex);
        m_FileOpsCv.wait(fileLock, [this]{ return !m_isFileOpsRunning; });
        m_isFileOpsRunning = true;

        std::ofstream file(m_FilePathObj, std::ios::out | std::ios::app);
        if (file.is_open())
        {
            while (!dataQueue.empty())
            {
                auto data = dataQueue.front();
                dataQueue.pop();
                file << data.data() << "\n";
                file.flush();
            }
            file.close();
        }
        else
        {
            std::ostringstream osstr;
            osstr << "LOGGING_ERROR : [";
            osstr << std::this_thread::get_id();
            osstr << "]: File [" << m_FilePathObj << "] can not be opened to log MSG;";
            errMsg = osstr.str();
        }
        m_isFileOpsRunning = false;
        fileLock.unlock();
        m_FileOpsCv.notify_all();

        if (!errMsg.empty())
            throw std::runtime_error(errMsg);
    }
    catch(...)
    {
        excpPtr = std::current_exception();
    }
}

