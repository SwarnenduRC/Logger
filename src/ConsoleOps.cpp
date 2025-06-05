#include "ConsoleOps.hpp"

ConsoleOps::ConsoleOps()
    : DataOps()
    , m_isOpsRunning(false)
{}

ConsoleOps::~ConsoleOps()
{
}

void ConsoleOps::writeToOutStreamObject(BufferQ&& dataQueue, std::exception_ptr& excpPtr)
{
    if (dataQueue.empty())
        return;

    try
    {
        // Place holder for now
    }
    catch(...)
    {
        excpPtr = std::current_exception();
    }
}

