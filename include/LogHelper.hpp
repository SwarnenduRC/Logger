#ifndef LOGGER_HELPER_HPP
#define LOGGER_HELPER_HPP

#include "Logger.hpp"

using namespace logger;

template <typename T>
struct is_vector : std::false_type{};

template <typename T, typename Alloc>
struct is_vector<std::vector<T, Alloc>> : std::true_type{};

template <typename T>
struct is_list : std::false_type{};

template <typename T, typename Alloc>
struct is_list<std::list<T, Alloc>> : std::true_type{};

inline static Logger loggerObj("%Y%m%d_%H%M%S");
inline static auto& loggingOps = Logger::buildLoggingOpsObject();

template<typename ...Args>
void setupAndLog(const std::string_view fileName,
                const std::string_view funcName,
                const std::string_view marker,
                const size_t lineNo,
                const std::thread::id& tid,
                const LOG_TYPE& logType,
                const std::string_view format_str,
                Args&&... args)
{
    loggerObj.setFileName(fileName)
            .setFunctionName(funcName)
            .setLineNo(lineNo)
            .setThreadId(tid)
            .setMarker(marker)
            .setLogType(logType);

    loggerObj.log(format_str, args...);
    loggingOps << loggerObj.getLogStream().str();
}

template<typename MsgList, typename ...Args>
void setupAndLogListOfMsgs( const std::string_view fileName,
                            const std::string_view funcName,
                            const std::string_view marker,
                            const size_t lineNo,
                            const std::thread::id& tid,
                            const LOG_TYPE& logType,
                            const std::string_view format_str,
                            const MsgList& msgs,
                            Args&&... args)
{
    loggerObj.setFileName(fileName)
            .setFunctionName(funcName)
            .setLineNo(lineNo)
            .setThreadId(tid)
            .setMarker(marker)
            .setLogType(logType);

    loggerObj.log(format_str, args...);
    loggingOps << loggerObj.getLogStream().str();

}

template<typename ...Args>
void log_entry( const std::string_view fileName,
                const std::string_view funcName,
                const size_t lineNo,
                const std::string_view format_str,
                Args&&... args)
{
    setupAndLog(fileName,
                funcName,
                FORWARD_ANGLES,
                lineNo,
                std::this_thread::get_id(),
                LOG_TYPE::LOG_INFO,
                format_str,
                args...);
}

template<typename ...Args>
void log_exit(const std::string_view fileName,
                const std::string_view funcName,
                const size_t lineNo,
                const std::string_view format_str,
                Args&&... args)
{
    setupAndLog(fileName,
                funcName,
                BACKWARD_ANGLES,
                lineNo,
                std::this_thread::get_id(),
                LOG_TYPE::LOG_INFO,
                format_str,
                args...);
}

template<typename ...Args>
void log_err(const std::string_view fileName,
            const std::string_view funcName,
            const size_t lineNo,
            const std::string_view format_str,
            Args&&... args)
{
    setupAndLog(fileName,
                funcName,
                FORWARD_ANGLES,
                lineNo,
                std::this_thread::get_id(),
                LOG_TYPE::LOG_ERR,
                format_str,
                args...);
}

template<typename ...Args>
void log_warn(const std::string_view fileName,
            const std::string_view funcName,
            const size_t lineNo,
            const std::string_view format_str,
            Args&&... args)
{
    setupAndLog(fileName,
                funcName,
                FORWARD_ANGLE,
                lineNo,
                std::this_thread::get_id(),
                LOG_TYPE::LOG_WARN,
                format_str,
                args...);
}

template<typename ...Args>
void log_info(const std::string_view fileName,
            const std::string_view funcName,
            const size_t lineNo,
            const std::string_view format_str,
            Args&&... args)
{
    setupAndLog(fileName,
                funcName,
                FORWARD_ANGLE,
                lineNo,
                std::this_thread::get_id(),
                LOG_TYPE::LOG_INFO,
                format_str,
                args...);
}

template<typename ...Args>
void log_imp(const std::string_view fileName,
            const std::string_view funcName,
            const size_t lineNo,
            const std::string_view format_str,
            Args&&... args)
{
    setupAndLog(fileName,
                funcName,
                FORWARD_ANGLE,
                lineNo,
                std::this_thread::get_id(),
                LOG_TYPE::LOG_IMP,
                format_str,
                args...);
}

template<typename ...Args>
void log_dbg([[maybe_unused]] const std::string_view fileName,
            [[maybe_unused]] const std::string_view funcName,
            [[maybe_unused]] const size_t lineNo,
            [[maybe_unused]] const std::string_view format_str,
            [[maybe_unused]] Args&&... args)
{
#if defined (DEBUG) || (__DEBUG__)
    setupAndLog(fileName,
                funcName,
                FORWARD_ANGLE,
                lineNo,
                std::this_thread::get_id(),
                LOG_TYPE::LOG_DBG,
                format_str,
                args...);
#endif
}

template<typename ...Args>
void log_assert(const std::string_view fileName,
                const std::string_view funcName,
                const size_t lineNo,
                const std::string_view cond,
                const bool exitGracefuly,
                const std::string_view format_str,
                Args&&... args)
{
    if (cond.empty())
        return;

    loggerObj.setAssertCondition(cond); // Set the assertion condition

    setupAndLog(fileName,
                funcName,
                FORWARD_ANGLE,
                lineNo,
                std::this_thread::get_id(),
                LOG_TYPE::LOG_ASSERT,
                format_str,
                args...);

    if (exitGracefuly)
    {
        loggingOps.~LoggingOps();
        loggerObj.~Logger();
        std::exit(EXIT_FAILURE);
    }
    else
    {
        std::abort();
    }
}

template<typename ...Args>
void log_fatal(const std::string_view fileName,
                const std::string_view funcName,
                const size_t lineNo,
                const std::string_view format_str,
                Args&&... args)
{
    setupAndLog(fileName,
                funcName,
                FORWARD_ANGLE,
                lineNo,
                std::this_thread::get_id(),
                LOG_TYPE::LOG_FATAL,
                format_str,
                args...);

    std::abort();
}

#endif // LOGGER_HELPER_HPP