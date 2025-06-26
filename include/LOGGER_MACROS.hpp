#ifndef LOGGER_MACROS_HPP
#define LOGGER_MACROS_HPP

#include "Logger.hpp"

#include <thread>

namespace logger
{
    static Logger loggerObj("%Y%m%d_%H%M%S");

    void setupLoggerProperties( const std::string_view fileName,
                                const std::string_view funcName,
                                const size_t lineNo,
                                const std::thread::id& runningThreadId,
                                const std::string_view marker)
    {
        loggerObj.setFileName(fileName)
                 .setFunctionName(funcName)
                 .setLineNo(lineNo)
                 .setThreadId(runningThreadId)
                 .setMarker(marker);
    }

    #define SETUP_LOGGING_PROPERTIES(MARKER)                    \
    setupLoggerProperties(                                      \
            __FILE__,                                           \
            __PRETTY_FUNCTION__,                                \
            __LINE__,                                           \
            std::this_thread::get_id(),                         \
            MARKER                                              \
        );

    #define LOG_ENTRY(fmt_str, __VA_ARGS__...)                  \
    auto pLoggingOps = Logger::buildLoggingOpsObject();         \
    if (pLoggingOps)                                            \
    {                                                           \
        SETUP_LOGGING_PROPERTIES(FORWARD_ANGLES)                \
        loggerObj.log(  LOG_TYPE::LOG_INFO,                     \
                        #fmt_str,                               \
                        ##__VA_ARGS__);                         \
        std::ostringstream oss;                                 \
        oss << loggerObj.getLogStream().str();                  \
    }

} // namespace logger

#endif  //LOGGER_MACROS_HPP