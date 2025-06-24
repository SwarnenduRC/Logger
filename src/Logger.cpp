#include "Logger.hpp"
#include "FileOps.hpp"
#include "ConsoleOps.hpp"

#include <iomanip>

//#define __FILE_LOGGING__
#define __FILE_SIZE__ 1024
#define __LOG_FILE_NAME__ "AppLogs.txt"
#define __FILE_PATH__ "/tmp/"
#define __FILE_EXTN__ ".log"

namespace logger
{
    /*static*/const UNORD_STRING_MAP Logger::m_stringToEnumMap = 
    { 
        { "ERR",        LOG_TYPE::LOG_ERR           },
        { "INF",        LOG_TYPE::LOG_INFO          },
        { "WARN",       LOG_TYPE::LOG_WARN          },
        { "DBG",        LOG_TYPE::LOG_DBG           },
        { "FATAL",      LOG_TYPE::LOG_FATAL         },
        { "IMP",        LOG_TYPE::LOG_IMP           },
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
        { LOG_TYPE::LOG_ASSERT,     "ASSERT"        },
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

    /*static*/ LoggingOps& Logger::buildLogObject() noexcept
    {
        static std::unique_ptr<LoggingOps> pLoggingOps;
        static auto initialize = true;  // First time TRUE and then always FALSE
        while (initialize)
        {
#ifdef __FILE_LOGGING__ // Is it going to be a file logging ops?
            std::ostringstream parser;
            uintmax_t fileSize = 1024 * 1000;   // Default max log file size is 1 MB
            std::string fileName;
            std::string fileExtn;
            std::string filePath;
#ifdef __LOG_FILE_NAME__
            parser << __LOG_FILE_NAME__;
            fileName = parser.str();
            // Break out then and there if file logging is requested
            // but log file name is not provided. We are not assuming
            // any log file name on our own in any case
            if (fileName.empty())
                break;
#endif  // __LOG_FILE_NAME__
#ifdef __FILE_SIZE__    // Is the max log file size provided?
            parser << __FILE_SIZE__;
            char* pConverted = nullptr;
            fileSize = std::strtoul(parser.str().c_str(), &pConverted, 10);

            if (pConverted == parser.str().c_str())
                break; // Invalid file size not acceptable
#endif // __FILE_SIZE__
#ifdef __FILE_EXTN__    // IS there a separate file extn also available from user?
            parser.clear();
            parser << __FILE_EXTN__;
            fileExtn = parser.str();
#endif  // __FILE_EXTN__
#ifdef __FILE_PATH__
            parser.clear();
            parser << __FILE_PATH__;
            filePath = parser.str();
            std::filesystem::path path(filePath);
            if (!std::filesystem::exists(path) && std::filesystem::is_directory(path))
                break;  // Invalid file path not allowed
#endif // __FILE_PATH__
            pLoggingOps.reset(new FileOps(fileSize, fileName, filePath, fileExtn));
#else   // Plain console logging it is
            pLoggingOps.reset(new ConsoleOps());
#endif  // __FILE_LOGGING__
        }
        initialize = false; // By this time initialization is completed
        return *pLoggingOps;
    }

    Logger::Logger(const std::string_view timeFormat)
        : m_threadID()
        , m_clock(timeFormat)
        , m_lineNo(0)
        , m_isFileNameRequired(false)
    {
#if defined(__FILE_NAME_REQUIRED__) // Should have been defined either in make file or cmake during compilation
        m_isFileNameRequired = true;
#endif
    }

    Logger& Logger::setFileName(const std::string_view val)
    {
        if (!val.empty())
            m_fileName = val.data();
        
        return *this;
    }

    Logger& Logger::setFunctionName(const std::string_view val)
    {
        if (!val.empty())
            m_fileName = val.data();
        
        return *this;
    }

    Logger& Logger::setLineNo(const size_t val)
    {
        m_lineNo = val;
        return *this;
    }

    Logger& Logger::setThreadId(const std::thread::id& val)
    {
        m_threadID = val;
        return *this;
    }

    Logger& Logger::setMarker(const std::string_view val)
    {
        if (!val.empty())
            m_logMarker = val;

        return *this;
    }

    void Logger::populatePrerequisitFileds(const LOG_TYPE& logType)
    {
        constructLogMsgPrefix(logType);
    }

    void Logger::constructLogMsgPrefix(const LOG_TYPE& logType)
    {        
        constructLogMsgPrefixFirstPart();
        constructLogMsgPrefixSecondPart();
        if (LOG_TYPE::LOG_DBG == logType)
        {
#if defined(DEBUG) || defined(__DEBUG__)
                m_logStream << covertLogTypeEnumToString(logType) << FIELD_SEPARATOR;
#else
                m_logStream.clear();
#endif
        }
        else
        {
            m_logStream << covertLogTypeEnumToString(logType) << FIELD_SEPARATOR;
        }
    }

    void Logger::constructLogMsgPrefixFirstPart()
    {
        m_logStream << m_clock.getLocalTimeStr();
        m_logStream << FIELD_SEPARATOR;
    }

    void Logger::constructLogMsgPrefixSecondPart()
    {
        m_logStream << std::right 
                    << std::setw(sizeof(size_t)) 
                    << m_threadID 
                    << FIELD_SEPARATOR;

        if (m_isFileNameRequired)
            m_logStream << std::right 
                        << std::setw(128) 
                        << m_fileName 
                        << FIELD_SEPARATOR;

        std::string funcName;
        if (std::string::npos != m_funcName.find(":"))
        {
            auto className = m_funcName.substr(0, m_funcName.find_first_of(":") - 1);
            funcName = m_funcName.substr(m_funcName.find_last_of(":") + 1);
            m_logStream << std::right 
                        << std::setw(128) 
                        << className 
                        << FIELD_SEPARATOR;
        }
        else
        {
            funcName = m_funcName;
        }
        if (funcName.find("(") != std::string::npos)
            m_logStream << std::right 
                        << std::setw(128) 
                        << funcName.substr(0, funcName.find_first_of("(")) 
                        << FIELD_SEPARATOR;
        else
            m_logStream << std::right 
                        << std::setw(128) 
                        << m_funcName 
                        << FIELD_SEPARATOR;
        
        m_logStream << std::right 
                    << std::setw(sizeof(size_t)) 
                    << m_lineNo 
                    << FIELD_SEPARATOR
                    << m_logMarker
                    << ONE_SPACE;
    }

    void Logger::vlog(const LOG_TYPE& logType,
                        const std::string_view formatStr, 
                        std::format_args args)
    {
        populatePrerequisitFileds(logType);
        auto logMsg = std::vformat(formatStr, args);
        m_logStream << logMsg << "\n";  //Add a line break after each log message printed
    }
}   //namespace logger

