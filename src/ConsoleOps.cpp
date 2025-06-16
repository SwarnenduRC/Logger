#include "ConsoleOps.hpp"

#include <iostream>

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
        flush();
        // Wait for any ongoing data operations to finish
        std::this_thread::sleep_for(std::chrono::microseconds(500));
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
                if (m_testing)
                {
                    m_testStringStream << data.data();
                    if (!m_testStringStream.good())
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

