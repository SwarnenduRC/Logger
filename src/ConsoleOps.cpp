#include "ConsoleOps.hpp"

ConsoleOps::ConsoleOps()
    : DataOps()
    , m_isOpsRunning(false)
{}

ConsoleOps::~ConsoleOps()
{
}

void ConsoleOps::writeDataTo(const std::string_view data)
{
    push(data);
    flush();    //Flush the data immediately to console always
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
                    m_testStringStream << data.data() << std::endl;
                    m_testStringStream.flush();
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

