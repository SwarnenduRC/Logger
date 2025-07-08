/*
 * MIT License
 * 
 * Copyright (c) 2024 Swarnendu RC
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
 * File: LoggingOps.cpp
 * Description: Implementation of the LoggingOps class for asynchronous data operations.
 * See LoggingOps.hpp for class definition and documentation.
 */

#include "LoggingOps.hpp"
#include "Clock.hpp"

#include <sstream>
#include <bitset>
#include <filesystem>
#include <fstream>

using namespace logger;

static std::mutex m_excpFileMtx;

/*friend*/ void logger::operator<<(LoggingOps& obj, const std::ostringstream& oss)
{
    if (oss.good())
        obj.write(oss.str());
}

/*friend*/ void logger::operator<<(LoggingOps& obj, const std::istringstream& iss)
{
    if (iss.good())
        obj.write(iss.str());
}

/*friend*/ void logger::operator<<(LoggingOps& obj, const std::string_view data)
{
    if (!data.empty())
        obj.write(data);
}

/*friend*/ void logger::operator<<(LoggingOps& obj, const std::vector<std::string>& dataVec)
{
    if (!dataVec.empty())
        obj.write(dataVec);
}

/*friend*/ void logger::operator<<(LoggingOps& obj, const std::list<std::string>& dataList)
{
    if (!dataList.empty())
        obj.write(dataList);
}

LoggingOps::LoggingOps()
    : m_DataRecords()
    , m_dataReady(false)
    , m_shutAndExit(false)
    , m_excpPtrVec(0)
{
}

/*virtual*/LoggingOps::~LoggingOps()
{
    // Wait for any ongoing data operations to finish
    std::unique_lock<std::mutex> dataLock(m_DataRecordsMtx);
    // Set the flag to true to indicate that we are shutting down
    m_shutAndExit = true;
    // Notify the watcher thread to wake up and complete
    // any pending operations before exiting
    dataLock.unlock();
    m_DataRecordsCv.notify_one();

    if (m_watcher.joinable())
        m_watcher.join();

    collectAndPrintExceptions();
}

void LoggingOps::push(const std::string_view data)
{
    if (data.empty())
        return;

    auto push = [this](std::array<char, bufferSize>& dataRecord, const std::string_view data)
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
        std::array<char, bufferSize> dataRecord; // One extra byte for null termination
        std::scoped_lock<std::mutex> lock(m_DataRecordsMtx);
        if (data.size() > dataRecord.size())
        {
            std::string dataCopy = data.data();
            while (dataCopy.size() > dataRecord.size()) // Split the data into 1024 byte chunks
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
    // If the data queue contains at least 256 elements
    // then notify the watcher thread that data is available
    // and it can start writing to the outstream object
    if (m_DataRecords.size() == 256)
    {
        m_dataReady = true;
        m_DataRecordsCv.notify_one();
    }
}

bool LoggingOps::pop(BufferQ& data)
{
    if (m_DataRecords.empty())
        return false;

    // Clear the outgoing data buffer
    BufferQ().swap(data);
    data.swap(m_DataRecords);
    m_dataReady = false;

    return true;
}

void LoggingOps::keepWatchAndPull()
{
    BufferQ dataq;
    // It is an infinite loop, but it will break out of the loop
    // when the m_shutAndExit flag is set to true
    do
    {
        std::unique_lock<std::mutex> dataLock(m_DataRecordsMtx);
        m_DataRecordsCv.wait(dataLock, [this]{ return m_dataReady || m_shutAndExit.load(); });

        auto success = pop(dataq);
        dataLock.unlock();
        m_DataRecordsCv.notify_one();
        // Spawn a thread to write to the file
        // and pass the data queue to it so that
        // the data records queue can be free for
        // other threads to push data to it
        // and the file can be written to in parallel
        // The thread will be joined after the file is written
        std::thread writerThread;
        std::exception_ptr excpPtr = nullptr;
        if (success)
        {
            writerThread = std::thread([this, &dataq, &excpPtr](){ writeToOutStreamObject(std::move(dataq), excpPtr); });
        }
        if (writerThread.joinable())
        {
            if (excpPtr)
                m_excpPtrVec.emplace_back(excpPtr);
            writerThread.join();
        }

        if (m_shutAndExit)
            break;
    } while (true);
}

void LoggingOps::flush()
{
    if (!m_DataRecords.empty())
    {
        m_dataReady = true;
        m_DataRecordsCv.notify_one();
        // Give it sometime to get flashed
        std::this_thread::sleep_for(std::chrono::microseconds(200));
    }
}

void LoggingOps::write(const std::string_view data)
{
    if (data.empty())
        return;

    writeDataTo(data);
}

void LoggingOps::write(const std::vector<std::string_view>& dataVec) noexcept
{
    if (!dataVec.empty())
    {
        for (const auto& data : dataVec)
            write(data);
    }
}

void LoggingOps::write(const std::list<std::string_view>& dataList) noexcept
{
    if (!dataList.empty())
    {
        for (const auto& data : dataList)
            write(data);
    }
}

void LoggingOps::write(const std::vector<std::string>& dataVec) noexcept
{
    if (!dataVec.empty())
    {
        for (const auto& data : dataVec)
            write(data);
    }
}

void LoggingOps::write(const std::list<std::string>& dataList) noexcept
{
    if (!dataList.empty())
    {
        for (const auto& data : dataList)
            write(data);
    }
}

void LoggingOps::write(const uint8_t data)
{
    write(std::bitset<8>(data).to_string());
}

void LoggingOps::write(const uint16_t data)
{
    write(std::bitset<16>(data).to_string());
}

void LoggingOps::write(const uint32_t data)
{
    write(std::bitset<32>(data).to_string());
}

void LoggingOps::write(const uint64_t data)
{
    write(std::bitset<64>(data).to_string());
}

void LoggingOps::append(const std::string_view data)
{
    write(data);
}

void LoggingOps::append(const uint8_t data)
{
    write(std::bitset<8>(data).to_string());
}

void LoggingOps::append(const uint16_t data)
{
    write(std::bitset<16>(data).to_string());
}

void LoggingOps::append(const uint32_t data)
{
    write(std::bitset<32>(data).to_string());
}

void LoggingOps::append(const uint64_t data)
{
    write(std::bitset<64>(data).to_string());
}

void LoggingOps::append(const std::vector<uint8_t>& binaryStream)
{
    if (!binaryStream.empty())
    {
        for (const auto& bindata : binaryStream)
            write(bindata);
    }
}

void LoggingOps::append(const std::vector<uint16_t>& binaryStream)
{
    if (!binaryStream.empty())
    {
        for (const auto& bindata : binaryStream)
            write(bindata);
    }
}

void LoggingOps::append(const std::vector<uint32_t>& binaryStream)
{
    if (!binaryStream.empty())
    {
        for (const auto& bindata : binaryStream)
            write(bindata);
    }
}

void LoggingOps::append(const std::vector<uint64_t>& binaryStream)
{
    if (!binaryStream.empty())
    {
        for (const auto& bindata : binaryStream)
            write(bindata);
    }
}

void LoggingOps::write(const std::vector<uint8_t>& binaryStream)
{
    if (!binaryStream.empty())
    {
        for (const auto& bindata : binaryStream)
            write(bindata);
    }
}

void LoggingOps::write(const std::vector<uint16_t>& binaryStream)
{
    if (!binaryStream.empty())
    {
        for (const auto& bindata : binaryStream)
            write(bindata);
    }
}

void LoggingOps::write(const std::vector<uint32_t>& binaryStream)
{
    if (!binaryStream.empty())
    {
        for (const auto& bindata : binaryStream)
            write(bindata);
    }
}

void LoggingOps::write(const std::vector<uint64_t>& binaryStream)
{
    if (!binaryStream.empty())
    {
        for (const auto& bindata : binaryStream)
            write(bindata);
    }
}

void LoggingOps::collectAndPrintExceptions()
{
    // Check for if there is any data write exceptions
    // before quitting. If so, log them in a common file
    if (!m_excpPtrVec.empty())
    {
        auto constructMsg = [](const std::exception& excp)
        {
            std::stringstream ss;
            Clock clock("%Y%m%d_%H%M%S");
            ss << m_FieldSep << clock.getLocalTimeStr() << m_FieldSep;
            ss << std::this_thread::get_id() << m_FieldSep;
            ss << ">> " << excp.what() << "\n";
            return ss.str();
        };

        auto currFilePath = std::filesystem::current_path().string();
        currFilePath += "/";
        //Finally create the file path object
        auto filePathObj = std::filesystem::path(currFilePath + m_ExcpLogFileName.data());

        std::unique_lock<std::mutex> fileLock(m_excpFileMtx);
        std::ofstream excpFile(filePathObj, std::ios::app | std::ios::binary);
        if (excpFile.is_open())
        {
            for (const auto& excp : m_excpPtrVec)
            {
                try
                {
                    if (excp)
                        std::rethrow_exception(excp);
                }
                catch(const std::exception& e)
                {
                    excpFile << constructMsg(e);
                }
            }
            excpFile.close();
        }
        fileLock.unlock();
    }
}

