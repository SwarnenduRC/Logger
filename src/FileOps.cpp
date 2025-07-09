/*
 * FileOps.cpp
 *
 * Implementation of the FileOps class for thread-safe file operations.
 *
 * MIT License
 *
 * Copyright (c) 2025 Swarnendu RC
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
#include "FileOps.hpp"
#include "Clock.hpp"

#include <tuple>
#include <memory>
#include <functional>

using namespace logger;

// Necessary constants and static variables
static constexpr std::string_view nullString = "";
static constexpr std::string_view DEFAULT_FILE_EXTN = ".txt";

/*static*/ bool FileOps::isFileEmpty(const std::filesystem::path& file) noexcept
{
    if (fileExists(file))
    {
        std::ifstream fileStream(file, std::ios::binary);
        if (fileStream.is_open())
            return fileStream.peek() == std::ifstream::traits_type::eof();
    }
    return false;
}

/*static*/ bool FileOps::fileExists(const std::filesystem::path& file) noexcept
{
    if (file.empty())
        return false;

    return std::filesystem::exists(file);
}

/*static*/ bool FileOps::removeFile(const std::filesystem::path& file) noexcept
{
    if (fileExists(file))
        return std::filesystem::remove(file);
    else
        return false;
}

/*static*/ bool FileOps::clearFile(const std::filesystem::path& file) noexcept
{
    if (fileExists(file))
    {
        std::ofstream outFile(file, std::ios::out | std::ios::trunc);
        if (outFile.is_open())
        {
            outFile.close();
            return true;
        }
    }
    return false;
}

/*static*/ bool FileOps::createFile(const std::filesystem::path& file) noexcept
{
    if (file.empty() || fileExists(file))
        return false;
    
    std::ofstream FILE(file);
    if (FILE.is_open())
    {
        FILE.close();
        return true;
    }
    return false;
}

/*static*/ bool FileOps::readFileByteRange(FileOps& file,
                                    const std::streampos start,
                                    const std::streampos end,
                                    std::vector<char>& outBuff)
{
    try
    {
        if (file.isEmpty())
            throw std::runtime_error("File " + file.getFilePathObj().string() + " empty to read");

        std::streampos fileSize = file.getFileSize();
        if (start > fileSize || end > fileSize || start > end)
        {
            if (start > fileSize)
                throw std::runtime_error("Out of bound: Start pos is greater than file size");
            else if (end > fileSize)
                throw std::runtime_error("Out of bound: End pos is greater than file size");
            else
                throw std::runtime_error("Out of bound: Start pos is greater than end pos");
        }

        std::ifstream ifile(file.getFilePathObj(), std::ios::binary);
        if (!ifile)
            throw std::runtime_error("File " + file.getFilePathObj().string() + " can't be opened for reading");

        auto bytesToRead = end - start;
        outBuff.clear();
        outBuff.resize(bytesToRead);
        ifile.seekg(start, std::ios::beg);
        ifile.read(outBuff.data(), bytesToRead);

        if (!ifile)
            throw std::runtime_error("File " + file.getFilePathObj().string() + " can't be read even after opening");

        return true;
    }
    catch(...)
    {
        auto excpPtr = std::current_exception();
        file.addRaisedException(excpPtr);
    }
    return false;
}

/*static*/bool FileOps::readFileLineRange(FileOps& file,
                                        const size_t startLineNo,
                                        const size_t endLineNo,
                                        std::vector<std::string>& outBuf)
{
    try
    {
        if (file.isEmpty())
            throw std::runtime_error("File " + file.getFilePathObj().string() + " empty to read");

        if (startLineNo > endLineNo)
            throw std::runtime_error("Out of bound: Start pos is greater than end pos");

        std::ifstream ifile(file.getFilePathObj(), std::ios::binary);
        if (!ifile)
            throw std::runtime_error("File " + file.getFilePathObj().string() + " can't be opened for reading");

        outBuf.clear();
        std::string readLine;
        size_t readLineCnt = 0;
        while (std::getline(ifile, readLine))
        {
            ++readLineCnt;
            if (readLineCnt >= startLineNo)
            {
                if (readLineCnt <= endLineNo)
                    outBuf.emplace_back(readLine);
            }
            readLine.clear();
            if (readLineCnt > endLineNo)
                break;
        }
        if (!ifile)
            throw std::runtime_error("File " + file.getFilePathObj().string() + " can't be read even after opening");

        return true;
    }
    catch(...)
    {
        auto excpPtr = std::current_exception();
        file.addRaisedException(excpPtr);
    }
    return false;
}

void FileOps::populateFilePathObj(const StdTupple& fileDetails)
{
    //First of all let us wait for any ongoing file operations (if any) to finish
    std::unique_lock<std::mutex> lock(m_FileOpsMutex);
    m_FileOpsCv.wait(lock, [this] { return !m_isFileOpsRunning; });

    m_isFileOpsRunning = true;  // Make the flag true to indicate that file operations are in progress

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
            m_FileName = m_FileName.substr(0, m_FileName.find_last_of('.')); //It is expected that the file name will have only one extension
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
            if (pathSeparator == std::string::npos) //If it is on windows sytem
                pathSeparator = m_FileName.find_last_of('\\');

            if (pathSeparator != std::string::npos)
            {
                //Separate the file name and path from the incoming string
                m_FilePath = m_FileName.substr(0, pathSeparator + 1);
                m_FileName = m_FileName.substr(pathSeparator + 1);
            }
            else    //If the file name does not contain any path, then use the current path
            {
                m_FilePath = std::filesystem::current_path().string();
                m_FilePath += getSeparator();
            }
        }
        else //If the file path doesn't end with a separator, then add it
        {
            if (m_FilePath.back() != '/' && m_FilePath.back() != '\\')
            {
                m_FilePath += getSeparator();
            }
        }
        //Finally create the file path object
        m_FilePathObj = std::filesystem::path(m_FilePath + m_FileName);
    }
    //All done, now we can set the flag to false
    //and notify any waiting threads
    m_isFileOpsRunning = false;
    m_FileOpsCv.notify_one();
}

FileOps::FileOps(const std::uintmax_t maxFileSize,
                 const std::string_view fileName, 
                 const std::string_view filePath, 
                 const std::string_view fileExtension)
    : LoggingOps()
    , m_FileName(fileName)
    , m_FilePath(filePath)
    , m_FileExtension(fileExtension)
    , m_FileContent(DataQ())
    , m_MaxFileSize(maxFileSize)
    , m_isFileOpsRunning(false)
{
    auto fileDetails = std::make_tuple(m_FileName, m_FilePath, m_FileExtension);
    // Initialize the file path object
    populateFilePathObj(fileDetails);
    //Spawn a thread to keep watch and pull the data from the data records queue
    //and write it to the file whenever it is available
    std::function<void()> watcherThread = [this]() { keepWatchAndPull(); };
    m_watcher = std::thread(std::move(watcherThread));
}

FileOps::~FileOps()
{
}

FileOps& FileOps::setFileName(const std::string_view fileName)
{
    if (fileName.empty() || fileName == m_FileName)
        return *this;

    populateFilePathObj(std::make_tuple(std::string(fileName), nullString.data(), nullString.data()));
    return *this;   // Builder pattern
}

FileOps& FileOps::setFilePath(const std::string_view filePath)
{
    if (filePath.empty() || filePath == m_FilePath)
        return *this;

    populateFilePathObj(std::make_tuple(nullString.data(), std::string(filePath), nullString.data()));
    return *this;
}

FileOps& FileOps::setFileExtension(const std::string_view fileExtension)
{
    if (fileExtension.empty() || fileExtension == m_FileExtension)
        return *this;

    populateFilePathObj(std::make_tuple(nullString.data(), nullString.data(), std::string(fileExtension)));
    return *this;
}

std::uintmax_t FileOps::getFileSize()
{
    std::uintmax_t fileSize = 0;
    if (fileExists())
    {
        flush();
        std::scoped_lock<std::mutex> fileLock(m_FileOpsMutex);
        m_isFileOpsRunning = true;
        std::error_code ec;
        fileSize = std::filesystem::file_size(m_FilePathObj, ec);
        m_isFileOpsRunning = false;
        if (ec)
        {
            std::ostringstream os;
            os << "Exception occured while reading file size-->";
            os << "Thread[" << std::this_thread::get_id() << "], ";
            os << "Excp Msg: " << ec.message() << std::endl;
            throw std::runtime_error(os.str());
        }
    }
    return fileSize;
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

    // Check if there is any data in the data records queue
    // and wait for it to be processed before reading the file
    flush();

    DataQ().swap(m_FileContent); // Clear the file content queue

    std::unique_lock<std::mutex> fileLock(m_FileOpsMutex);
    m_FileOpsCv.wait(fileLock, [this]{ return !m_isFileOpsRunning; });
    m_isFileOpsRunning = true;

    if (std::filesystem::exists(m_FilePathObj))
    {
        std::ifstream file(m_FilePathObj, std::ios::binary);
        if (file.is_open())
        {
            std::string line;
            while (std::getline(file, line))
            {
                m_FileContent.emplace(line);
                line.clear();
            }
        }
        else
        {
            m_isFileOpsRunning = false;
            fileLock.unlock();
            m_FileOpsCv.notify_all();
            throw std::runtime_error("Failed to open file: " + m_FilePathObj.string());
        }
    }
    m_isFileOpsRunning = false;
    fileLock.unlock();
    m_FileOpsCv.notify_all();
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

void FileOps::writeDataTo(const std::string_view data)
{
    try
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
            /**
             * First check if the current log file
             * size and the data size to be written
             * exceeds the max file size limit set
             * by the user. If so then rename the
             * existing file with the current time
             * stamp and create a new blank file
             * for further logging/writting. Throw
             * exception for any runtime error case.
             */
            flush(); //Flush the data whatever in the queue waiting to be written
            // Now get the file size
            auto currFileSize = getFileSize();
            if ((currFileSize + data.size()) >= m_MaxFileSize)
            {
                Clock clock;
                auto currentTimeStr = clock.getLocalTimeStr("%d%m%Y_%H%M%S");
                auto currentFileName = getFileName();
                auto currentFileExtension = getFileExtension();
                auto newFileName = currentFileName.substr(0, currentFileName.find(currentFileExtension))
                                     + "_" + currentTimeStr + currentFileExtension;
                if (renameFile(newFileName))
                {
                    setFileName(currentFileName);
                    createFile();
                }
                else
                {
                    throw std::runtime_error("File limit exceeds but can not be renamed");
                }
            }
            push(data);
        }
    }
    catch(...)
    {
        m_excpPtrVec.emplace_back(std::current_exception());
    }
}

void FileOps::writeToOutStreamObject(BufferQ&& dataQueue, std::exception_ptr& excpPtr)
{
    if (dataQueue.empty())
        return;

    try
    {
        std::string errMsg;
        std::unique_lock<std::mutex> fileLock(m_FileOpsMutex);
        m_FileOpsCv.wait(fileLock, [this]{ return !m_isFileOpsRunning; });
        m_isFileOpsRunning = true;

        std::ofstream file(m_FilePathObj, std::ios::out | std::ios::app | std::ios::binary);
        std::array<char, bufferSize> data;
        if (file.is_open())
        {
            while (!dataQueue.empty())
            {
                data.fill('\0');
                data = dataQueue.front();
                dataQueue.pop();
                file << data.data() << std::endl;
                file.flush();
            }
            file.close();
        }
        else
        {
            std::ostringstream osstr;
            osstr << "WRITING_ERROR : [";
            osstr << std::this_thread::get_id();
            osstr << "]: File [" << m_FilePathObj.string();
            osstr << "] can not be opened to write log data: " << data.data() << "\n";
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

bool FileOps::isEmpty()
{
    flush();
    std::unique_lock<std::mutex> fileLock(m_FileOpsMutex);
    m_FileOpsCv.wait(fileLock, [this]{ return !m_isFileOpsRunning; });
    m_isFileOpsRunning = true;
    std::ifstream file(m_FilePathObj.string(), std::ios::ate | std::ios::binary);
    if (!file)
        return false;

    auto retVal = file.tellg();
    file.close();
    m_isFileOpsRunning = false;
    fileLock.unlock();
    m_FileOpsCv.notify_one();
    return retVal == 0;
}

