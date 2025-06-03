#include "ConsoleOps.hpp"

std::vector<std::pair<const std::string_view, const ConsoleOps::LoggingTypes>> 
    ConsoleOps::LoggingTypesEnumStringMap = 
        {   std::make_pair("ERROR_LOG", LoggingTypes::ERROR_LOG),
            std::make_pair("INFO_LOG", LoggingTypes::INFO_LOG),
            std::make_pair("DEBUG_LOG", LoggingTypes::DEBUG_LOG),
            std::make_pair("DEFAULT_LOG", LoggingTypes::DEFAULT_LOG) 
        };

/*static*/ConsoleOps::LoggingTypes ConsoleOps::loggingTypesStringToEnum(const std::string_view enumString) noexcept
{
    auto retVal = LoggingTypes::DEBUG_LOG;
    if (!enumString.empty())
    {
        std::string enumStringCopy;
        for (auto stringChar : enumString)
            enumStringCopy += std::toupper(stringChar);

        for (const auto& pair : LoggingTypesEnumStringMap)
        {
            if (!pair.first.compare(enumStringCopy))
            {
                retVal = pair.second;
                break;
            }
        }
    }
    return retVal;
}

/*static*/ std::string ConsoleOps::loggingTypesEnumToString(const ConsoleOps::LoggingTypes& loggingType) noexcept
{
    auto retVal = std::string();
    for (const auto& pair : LoggingTypesEnumStringMap)
    {
        if (pair.second == loggingType)
        {
            retVal = pair.first;
            break;
        }
    }
    return retVal;
}

ConsoleOps::ConsoleOps()
    : DataOps()
    , m_isOpsRunning(false)
{}

ConsoleOps::~ConsoleOps()
{
    {
        std::lock_guard<std::mutex> lock(m_mtx);
    }
    m_shutAndExit = true;
    m_cv.notify_all();
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

