#include "Logger.hpp"
#include "FileOps.hpp"
#include "ConsoleOps.hpp"

#include "ENV_VARS.hpp"

#include <iomanip>

using namespace logger;

/*static*/const UNORD_STRING_MAP Logger::m_stringToEnumMap =
{
    { "ERR",        LOG_TYPE::LOG_ERR           },
    { "INF",        LOG_TYPE::LOG_INFO          },
    { "WARN",       LOG_TYPE::LOG_WARN          },
    { "DBG",        LOG_TYPE::LOG_DBG           },
    { "FATAL",      LOG_TYPE::LOG_FATAL         },
    { "IMP",        LOG_TYPE::LOG_IMP           },
    { "ASRT",       LOG_TYPE::LOG_ASSERT        },
    { "DEFAULT",    LOG_TYPE::LOG_DEFAULT       }
};

/*static*/const UNORD_LOG_TYPE_MAP Logger::m_EnumToStringMap =
{
    { LOG_TYPE::LOG_ERR,        "ERR"           },
    { LOG_TYPE::LOG_INFO,       "INF"           },
    { LOG_TYPE::LOG_WARN,       "WARN"          },
    { LOG_TYPE::LOG_DBG,        "DBG"           },
    { LOG_TYPE::LOG_FATAL,      "FATAL"         },
    { LOG_TYPE::LOG_IMP,        "IMP"           },
    { LOG_TYPE::LOG_ASSERT,     "ASRT"          },
    { LOG_TYPE::LOG_DEFAULT,    "DEFAULT"       }
};

/*static*/LOG_TYPE Logger::convertStringToLogTypeEnum(const std::string_view type) noexcept
{
    if (type.empty())
        return m_stringToEnumMap.cend()->second;

    auto itr = m_stringToEnumMap.find(type.data());
    if (itr != m_stringToEnumMap.end())
        return itr->second;
    else
        return m_stringToEnumMap.cend()->second;
}

/*static*/std::string Logger::covertLogTypeEnumToString(const LOG_TYPE& type) noexcept
{

    auto itr = m_EnumToStringMap.find(type);
    if (itr != m_EnumToStringMap.end())
        return itr->second;
    else
        return m_EnumToStringMap.cend()->second;
}

/*static*/ LoggingOps& Logger::buildLoggingOpsObject() noexcept
{
    static std::unique_ptr<LoggingOps> pLoggingOps;
    static auto initialize = true;  // First time TRUE and then always FALSE
    while (initialize)
    {
#if defined(FILE_LOGGING) // Is it going to be a file logging ops?
        std::ostringstream parser;
        uintmax_t fileSize = 1024 * 1000;   // Default max log file size is 1 MB
        std::string fileName;
        std::string fileExtn;
        std::string filePath;
#ifdef LOG_FILE_NAME
        parser << LOG_FILE_NAME;
        fileName = parser.str();
        // Break out then and there if file logging is requested
        // but none provided. We are not assuming
        // any log file name on our own in any case
        if (fileName.empty())
            break;
#endif  // LOG_FILE_NAME
#ifdef FILE_SIZE    // Is the max log file size provided?
        fileSize = FILE_SIZE;
#endif // FILE_SIZE
#ifdef LOG_FILE_EXTN    // IS there a separate file extn also available from user?
        std::ostringstream().swap(parser);
        parser << LOG_FILE_EXTN;
        fileExtn = parser.str();
#endif  // LOG_FILE_EXTN
#ifdef LOG_FILE_PATH
        std::ostringstream().swap(parser);
        parser << LOG_FILE_PATH;
        filePath = parser.str();
        std::filesystem::path path(filePath);
        if (!std::filesystem::exists(path) && std::filesystem::is_directory(path))
            break;  // Invalid file path not allowed
#endif // LOG_FILE_PATH
        pLoggingOps.reset(new FileOps(fileSize, fileName, filePath, fileExtn));
#else   // Plain console logging it is
        pLoggingOps.reset(new ConsoleOps());
#endif  // FILE_LOGGING
        initialize = false; // By this time initialization is completed
    }
    return *pLoggingOps;
}

Logger::Logger(const std::string_view timeFormat)
    : m_threadID()
    , m_clock(timeFormat)
    , m_lineNo(0)
{}

Logger& Logger::setFileName(const std::string_view val) noexcept
{
    if (!val.empty())
        m_fileName = val.data();

    return *this;
}

Logger& Logger::setFunctionName(const std::string_view val) noexcept
{
    if (!val.empty())
        m_funcName = val.data();

    return *this;
}

Logger& Logger::setLineNo(const size_t val) noexcept
{
    m_lineNo = val;
    return *this;
}

Logger& Logger::setThreadId(const std::thread::id& val) noexcept
{
    m_threadID = val;
    return *this;
}

Logger& Logger::setMarker(const std::string_view val) noexcept
{
    if (!val.empty())
        m_logMarker = val;

    return *this;
}

Logger& Logger::setLogType(const LOG_TYPE& logType) noexcept
{
    m_logType = logType;
    return *this;
}

Logger& Logger::setLogType(const std::string_view logType) noexcept
{
    return setLogType(convertStringToLogTypeEnum(logType));
}

Logger& Logger::setAssertCondition(const std::string_view cond) noexcept
{
    m_assertCond = cond.data();
    return *this;
}

void Logger::populatePrerequisitFileds()
{
    // Clear the log stream before populating it with new log message
    std::stringstream().swap(m_logStream);
    constructLogMsgPrefix();
    // Segregate the function name and class name from the
    // m_funcName, if it is in the format "ClassName:FunctionName"
    // If it is not in that format, then just use the function name
    // as it is, without any class name.
    // This is useful for logging purposes, to identify which class
    // and function the log message is coming from.
    // For example, if the function name is "Logger::log", then
    // the class name will be "Logger" and the function name will be "log".
    // If the function name is "log", then the class name will be empty
    // and the function name will be "log".
    // If the function name is "Logger::log()", then the class name will be "Logger"
    // and the function name will be "log".
    // If the function name is "Logger::log(const std::string&)", then the
    // class name will be "Logger" and the function name will be "log".
    std::string funcName;
    std::string className;
    if (std::string::npos != m_funcName.find(":"))
    {
        className = m_funcName.substr(0, m_funcName.find_first_of(":"));
        funcName = m_funcName.substr(m_funcName.find_last_of(":") + 1);
    }
    else
    {
        funcName = m_funcName;
    }
    funcName = funcName.substr(0, funcName.find_first_of("("));
    m_logStream << LEFT_SQUARE_BRACE
                << className
                << ONE_SPACE
                << COLONE_SEP
                << ONE_SPACE
                << funcName
                << RIGHT_SQUARE_BRACE
                << ONE_SPACE;

    // Check if the log message is due to an assertion failure. If so,
    // then log the condition details along with file and line no details
    if (!m_assertCond.empty())
    {
        auto assertionCond = m_assertCond;
        m_assertCond.clear(); // Clear the condition for next log msg
        m_logStream << "ASSERTION FAILURE in "
                    << m_fileName
                    << " at LN:"
                    << m_lineNo
                    << ", for [CONDITION: "
                    << assertionCond << "]"
                    << " evaluating to FALSE. ";
    }
}

void Logger::constructLogMsgPrefix()
{
    constructLogMsgPrefixFirstPart();
    constructLogMsgPrefixSecondPart();
}

void Logger::constructLogMsgPrefixFirstPart()
{
    m_logStream << FIELD_SEPARATOR << m_clock.getLocalTimeStr();
    m_logStream << FIELD_SEPARATOR << ONE_SPACE;
}

void Logger::constructLogMsgPrefixSecondPart()
{
    m_logStream << std::right
                << std::setw(10) //Assuming a thread ID in decimal can be of max 10 digits
                << m_threadID
                << FIELD_SEPARATOR
                << ONE_SPACE;

    m_logStream << std::left
                << std::setw(20) // Assuming a file name consists max 20 char
                << m_fileName 
                << FIELD_SEPARATOR
                << ONE_SPACE;

    m_logStream << std::right
                << std::setw(4)     //Assuming a file may contain max 9,999 no of lines
                << m_lineNo
                << FIELD_SEPARATOR
                << std::right
                << covertLogTypeEnumToString(m_logType)
                << m_logMarker
                << std::left;

    // Beutify the pre requisits fields by aligning them properly (optional)
    auto maxLogTypeSize = covertLogTypeEnumToString(LOG_TYPE::LOG_ASSERT).size();
    auto currLogTypeSize = covertLogTypeEnumToString(m_logType).size();
    auto currMarkerSize = m_logMarker.size();
    while ((currLogTypeSize + currMarkerSize) < (maxLogTypeSize + 1))
    {
        m_logStream << ONE_SPACE;
        ++currMarkerSize;
    }
    m_logStream << ONE_SPACE;   // Last space before next part begins
}

void Logger::vlog(const std::string_view formatStr, std::format_args args)
{
    populatePrerequisitFileds();
    auto logMsg = std::vformat(formatStr, args);
    // Remove any "\"" from the formed string, might have been left, due to
    // MACRO expression expansion
    if (logMsg.find(DOUBLE_QUOTES) != std::string::npos)
    {
        logMsg = logMsg.substr(logMsg.find_first_of(DOUBLE_QUOTES) + 1, 
                    logMsg.find_last_of(DOUBLE_QUOTES) - 1);
        // Recheck once again to be sure
        if (logMsg.find(DOUBLE_QUOTES) != std::string::npos)
            logMsg = logMsg.substr(logMsg.find_first_of(DOUBLE_QUOTES) + 1, 
                    logMsg.find_last_of(DOUBLE_QUOTES) - 1);
    }
    m_logStream << logMsg;
}

