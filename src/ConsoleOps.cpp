/*
 * ConsoleOps.cpp
 * 
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
 * Purpose:
 *   This file implements the ConsoleOps class, which provides logging operations
 *   for writing log data to the console (stdout). It manages a background thread
 *   to process queued log messages and supports both normal and testing modes.
 */

#include "ConsoleOps.hpp"

#include <iostream>
#include <functional>

using namespace logger;

ConsoleOps::ConsoleOps()
    : LoggingOps()
    , m_testing(false)
    , m_testStringStream()
    , m_isOpsRunning(false)
{
    //Spawn a thread to keep watch and pull the data from the data records queue
    //and write it to the file whenever it is available
    std::function<void()> watcherThread = [this]() { keepWatchAndPull(); };
    m_watcher = std::thread(std::move(watcherThread));
}

ConsoleOps::~ConsoleOps()
{
}

void ConsoleOps::writeDataTo(const std::string_view data)
{
    if (!data.empty())
    {
        push(data);
        flush(); //Flush it immediately as the data might be critical error or warning
    }
}

void ConsoleOps::writeToOutStreamObject(BufferQ&& dataQueue, std::exception_ptr& excpPtr)
{
    if (dataQueue.empty())
        return;

    try
    {
        std::string errMsg;
        std::unique_lock<std::mutex> consoleLock(m_mtx);
        m_cv.wait(consoleLock, [this]{ return !m_isOpsRunning; });
        m_isOpsRunning = true;

        std::ostream& outStream = std::cout;
        if (outStream.good())
        {
            while (!dataQueue.empty())
            {
                auto data = dataQueue.front();
                dataQueue.pop();
                if (m_testing && m_testStringStream.good()) // If testing mode is ON, write to the test string stream
                {
                    m_testStringStream << data.data() << std::endl;
                    if (!m_testStringStream.good()) // If writing to the test string stream fails, set the error message
                    {
                        std::ostringstream osstr;
                        osstr << "WRITING_ERROR : [";
                        osstr << std::this_thread::get_id();
                        osstr << "]: to test stringstream for data" << "[" << data.data() << "]";
                        if (osstr.good())
                            errMsg = osstr.str();
                    }
                }
                else
                {
                    outStream << data.data() << std::endl;
                    outStream.flush();
                }
            }
        }
        else
        {
            std::ostringstream osstr;
            osstr << "WRITING_ERROR : [";
            osstr << std::this_thread::get_id();
            osstr << "]: to console for data";
            errMsg = osstr.str();
        }
        m_isOpsRunning = false;
        consoleLock.unlock();
        m_cv.notify_all();

        if (!errMsg.empty())
            throw std::runtime_error(errMsg);
    }
    catch(...)
    {
        excpPtr = std::current_exception();
    }
}

