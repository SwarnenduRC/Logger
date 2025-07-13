/*
 * MIT License
 * 
 * Copyright (c) 2025 Swarnendu RC
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
 */

/**
 * @file LogHelper.hpp
 * @brief Helper utilities and wrapper functions for the Logger library.
 *
 * This header provides a set of template utilities and convenience functions
 * to simplify logging operations using the Logger class. It includes type
 * traits for container detection, functions for setting logger properties,
 * and various logging wrappers for different log levels (info, warning, error,
 * debug, fatal, etc.), as well as support for logging lists and assertion failures.
 */
#ifndef LOGGER_HELPER_HPP
#define LOGGER_HELPER_HPP

#include "Logger.hpp"

namespace logger
{
    template <typename T>
    struct is_vector : std::false_type{};

    template <typename T, typename Alloc>
    struct is_vector<std::vector<T, Alloc>> : std::true_type{};

    template <typename T>
    struct is_list : std::false_type{};

    template <typename T, typename Alloc>
    struct is_list<std::list<T, Alloc>> : std::true_type{};

    /**
     * @brief The logger object for the application.
     * This object is used to construct log messages
     * and write them to the log stream.
     *
     * @note inline because otherwise it will cause linker errors
     * when used in multiple translation units.
     */
    inline static Logger loggerObj("%Y%m%d_%H%M%S");

    /**
     * @brief The stream object for logging operations.
     * This object is used to write log messages
     * either to console or to a file, depending on the configuration.
     * It is built using the Logger class's static method.
     *
     * @note inline because otherwise it will cause linker errors
     * when used in multiple translation units.
     */
    inline static auto& loggingOps = Logger::buildLoggingOpsObject();

    /**
     * @brief Set the logger properties for the current log message.
     *
     * This function sets the file name, function name, marker, line number,
     * thread ID, and log type for the logger object. It is used to prepare
     * the logger object with necessary context before logging a message.
     *
     * @param [in] fileName The name of the file where the log is being generated.
     * @param [in] funcName The name of the function where the log is being generated.
     * @param [in] marker A string marker to indicate the type of log (e.g., entry, exit).
     * @param [in] lineNo The line number in the source code where the log is being generated.
     * @param [in] tid The thread ID of the thread generating the log.
     * @param [in] logType The type of log (e.g., info, error, debug).
     *
     * @note This function is typically called before logging a message
     * to ensure that the logger object has all the necessary context
     * for generating a meaningful log entry.
     *
     * @note inline because otherwise it will cause linker errors
     * when used in multiple translation units.
     */
    inline void setLoggerProperties
    (   const std::string_view fileName,
        const std::string_view funcName,
        const std::string_view marker,
        const size_t lineNo,
        const std::thread::id& tid,
        const LOG_TYPE& logType
    )
    {
        loggerObj.setFileName(fileName)
                .setFunctionName(funcName)
                .setLineNo(lineNo)
                .setThreadId(tid)
                .setMarker(marker)
                .setLogType(logType);
    }

    /**
     * @brief Log a message with the specified format and arguments.
     *
     * This function formats the log message using the provided format string
     * and arguments, and writes it to the log stream. It is a convenience wrapper
     * around the Logger's log method.
     *
     * @tparam Args Variadic template parameters for the arguments to be formatted into the log message.
     * @param [in] format_str The format string for the log message.
     *                 It can contain placeholders for the arguments, similar to printf-style formatting.
     *                 For example, "Log message: {}" where {} will be replaced by the provided arguments.
     * @param [in] args The arguments to be formatted into the log message.
     *                 These arguments will be formatted according to the format string.
     *                 The number and types of arguments should match the placeholders in the format string.
     */
    template<typename ...Args>
    void logMsg(const std::string_view format_str, Args&&... args)
    {
        static std::mutex raceMtx;
        // Static objects are atomic in inature only during
        // the lifetime of the program. Accessing or modifying
        // them from multiple threads requires external synchronization.
        std::lock_guard<std::mutex> lock(raceMtx);
        loggerObj.log(format_str, args...);
        loggingOps << loggerObj.getLogStream().str();
    }

    /**
     * @brief Log a list or vector of messages.
     *
     * This function logs a list or vector of messages using the specified format string
     * and arguments. It checks if the provided List is either a std::vector or std::list
     * of strings, and if so, it logs each message in the list.
     *
     * @tparam List The type of the list or vector to be logged.
     * @tparam Args Variadic template parameters for additional arguments to be formatted into the log message.
     * @param [in] fileName The name of the file where the log is being generated.
     * @param [in] funcName The name of the function where the log is being generated.
     * @param [in] lineNo The line number in the source code where the log is being generated.
     * @param [in] msgList The list or vector of messages to be logged.
     * @param [in] format_str The format string for the log message.
     * @param [in] args Additional arguments to be formatted into the log message.
     */
    template<typename List, typename ...Args>
    void log_list
    (
        const std::string_view fileName,
        const std::string_view funcName,
        const size_t lineNo,
        const List& msgList,
        const std::string_view format_str,
        Args&&... args
    )
    {
        // If and only if, it is either a vector or std::list of strings
        if constexpr (is_list<List>::value || is_vector<List>::value)
        {
            setLoggerProperties(fileName,
                            funcName,
                            FORWARD_ANGLES,
                            lineNo,
                            std::this_thread::get_id(),
                            LOG_TYPE::LOG_INFO);

            logMsg(format_str, args...);
            loggingOps << msgList;
        }
    }

    /**
     * @brief Log an entry point message.
     *
     * This function logs a message indicating the entry point of a function or code block.
     * It sets the logger properties such as file name, function name, line number, thread ID,
     * and log type before logging the message.
     *
     * @tparam Args Variadic template parameters for additional arguments to be formatted into the log message.
     * @param [in] fileName The name of the file where the log is being generated.
     * @param [in] funcName The name of the function where the log is being generated.
     * @param [in] lineNo The line number in the source code where the log is being generated.
     * @param [in] format_str The format string for the log message.
     * @param [in] args Additional arguments to be formatted into the log message.
     */
    template<typename ...Args>
    void log_entry
    (
        const std::string_view fileName,
        const std::string_view funcName,
        const size_t lineNo,
        const std::string_view format_str,
        Args&&... args
    )
    {
        setLoggerProperties(fileName,
                            funcName,
                            FORWARD_ANGLES,
                            lineNo,
                            std::this_thread::get_id(),
                            LOG_TYPE::LOG_INFO);

        logMsg(format_str, args...);
    }

    /**
     * @brief Log an exit point message.
     *
     * This function logs a message indicating the exit point of a function or code block.
     * It sets the logger properties such as file name, function name, line number, thread ID,
     * and log type before logging the message.
     *
     * @tparam Args Variadic template parameters for additional arguments to be formatted into the log message.
     * @param [in] fileName The name of the file where the log is being generated.
     * @param [in] funcName The name of the function where the log is being generated.
     * @param [in] lineNo The line number in the source code where the log is being generated.
     * @param [in] format_str The format string for the log message.
     * @param [in] args Additional arguments to be formatted into the log message.
     */
    template<typename ...Args>
    void log_exit
    (
        const std::string_view fileName,
        const std::string_view funcName,
        const size_t lineNo,
        const std::string_view format_str,
        Args&&... args
    )
    {
        setLoggerProperties(fileName,
                            funcName,
                            BACKWARD_ANGLES,
                            lineNo,
                            std::this_thread::get_id(),
                            LOG_TYPE::LOG_INFO);

        logMsg(format_str, args...);
    }

    /**
     * @brief Log an error message.
     *
     * This function logs an error message with the specified format and arguments.
     * It sets the logger properties such as file name, function name, line number, thread ID,
     * and log type before logging the message.
     *
     * @tparam Args Variadic template parameters for additional arguments to be formatted into the log message.
     * @param [in] fileName The name of the file where the log is being generated.
     * @param [in] funcName The name of the function where the log is being generated.
     * @param [in] lineNo The line number in the source code where the log is being generated.
     * @param [in] format_str The format string for the log message.
     * @param [in] args Additional arguments to be formatted into the log message.
     */
    template<typename ...Args>
    void log_err
    (
        const std::string_view fileName,
        const std::string_view funcName,
        const size_t lineNo,
        const std::string_view format_str,
        Args&&... args
    )
    {
        setLoggerProperties(fileName,
                            funcName,
                            FORWARD_ANGLE,
                            lineNo,
                            std::this_thread::get_id(),
                            LOG_TYPE::LOG_ERR);

        logMsg(format_str, args...);
    }

    /**
     * @brief Log a warning message.
     *
     * This function logs a warning message with the specified format and arguments.
     * It sets the logger properties such as file name, function name, line number, thread ID,
     * and log type before logging the message.
     *
     * @tparam Args Variadic template parameters for additional arguments to be formatted into the log message.
     * @param [in] fileName The name of the file where the log is being generated.
     * @param [in] funcName The name of the function where the log is being generated.
     * @param [in] lineNo The line number in the source code where the log is being generated.
     * @param [in] format_str The format string for the log message.
     * @param [in] args Additional arguments to be formatted into the log message.
     */
    template<typename ...Args>
    void log_warn
    (
        const std::string_view fileName,
        const std::string_view funcName,
        const size_t lineNo,
        const std::string_view format_str,
        Args&&... args
    )
    {
        setLoggerProperties(fileName,
                            funcName,
                            FORWARD_ANGLE,
                            lineNo,
                            std::this_thread::get_id(),
                            LOG_TYPE::LOG_WARN);

        logMsg(format_str, args...);
    }

    /**
     * @brief Log an informational message.
     *
     * This function logs an informational message with the specified format and arguments.
     * It sets the logger properties such as file name, function name, line number, thread ID,
     * and log type before logging the message.
     *
     * @tparam Args Variadic template parameters for additional arguments to be formatted into the log message.
     * @param [in] fileName The name of the file where the log is being generated.
     * @param [in] funcName The name of the function where the log is being generated.
     * @param [in] lineNo The line number in the source code where the log is being generated.
     * @param [in] format_str The format string for the log message.
     * @param [in] args Additional arguments to be formatted into the log message.
     */
    template<typename ...Args>
    void log_info
    (
        const std::string_view fileName,
        const std::string_view funcName,
        const size_t lineNo,
        const std::string_view format_str,
        Args&&... args
    )
    {
        setLoggerProperties(fileName,
                            funcName,
                            FORWARD_ANGLE,
                            lineNo,
                            std::this_thread::get_id(),
                            LOG_TYPE::LOG_INFO);

        logMsg(format_str, args...);
    }

    /**
     * @brief Log an important detail message.
     *
     * This function logs a message indicating an importance of the logged message.
     * It sets the logger properties such as file name, function name, line number, thread ID,
     * and log type before logging the message.
     *
     * @tparam Args Variadic template parameters for additional arguments to be formatted into the log message.
     * @param [in] fileName The name of the file where the log is being generated.
     * @param [in] funcName The name of the function where the log is being generated.
     * @param [in] lineNo The line number in the source code where the log is being generated.
     * @param [in] format_str The format string for the log message.
     * @param [in] args Additional arguments to be formatted into the log message.
     */
    template<typename ...Args>
    void log_imp
    (
        const std::string_view fileName,
        const std::string_view funcName,
        const size_t lineNo,
        const std::string_view format_str,
        Args&&... args
    )
    {
        setLoggerProperties(fileName,
                            funcName,
                            FORWARD_ANGLE,
                            lineNo,
                            std::this_thread::get_id(),
                            LOG_TYPE::LOG_IMP);

        logMsg(format_str, args...);
    }

    /**
     * @brief Log a debug message.
     *
     * This function logs a debug message with the specified format and arguments.
     * It sets the logger properties such as file name, function name, line number, thread ID,
     * and log type before logging the message.
     *
     * @tparam Args Variadic template parameters for additional arguments to be formatted into the log message.
     * @param [in] fileName The name of the file where the log is being generated.
     * @param [in] funcName The name of the function where the log is being generated.
     * @param [in] lineNo The line number in the source code where the log is being generated.
     * @param [in] format_str The format string for the log message.
     * @param [in] args Additional arguments to be formatted into the log message.
     *
     * @note This function is only compiled and executed in debug mode.
     * It is used to log debug information during development and testing.
     * In release mode, this function will not be included in the binary.
     */
    template<typename ...Args>
    void log_dbg
    (
        [[maybe_unused]] const std::string_view fileName,
        [[maybe_unused]] const std::string_view funcName,
        [[maybe_unused]] const size_t lineNo,
        [[maybe_unused]] const std::string_view format_str,
        [[maybe_unused]] Args&&... args
    )
    {
    #if defined (DEBUG) || (__DEBUG__)
        setLoggerProperties(fileName,
                            funcName,
                            FORWARD_ANGLE,
                            lineNo,
                            std::this_thread::get_id(),
                            LOG_TYPE::LOG_DBG);

        logMsg(format_str, args...);
    #endif
    }

    /**
     * @brief Log an assertion failure message.
     *
     * This function logs an assertion failure message with the specified condition and format.
     * It sets the logger properties such as file name, function name, line number, thread ID,
     * and log type before logging the message. If `exitGracefuly` is true, it will exit the program gracefully.
     *
     * @tparam Args Variadic template parameters for additional arguments to be formatted into the log message.
     * @param [in] fileName The name of the file where the log is being generated.
     * @param [in] funcName The name of the function where the log is being generated.
     * @param [in] lineNo The line number in the source code where the log is being generated.
     * @param [in] cond The condition that failed (assertion).
     * @param [in] exitGracefuly Whether to exit gracefully or abort on assertion failure.
     * @param [in] format_str The format string for the log message.
     * @param [in] args Additional arguments to be formatted into the log message.
     */
    template<typename ...Args>
    void log_assert
    (
        const std::string_view fileName,
        const std::string_view funcName,
        const size_t lineNo,
        const std::string_view cond,
        const bool exitGracefuly,
        const std::string_view format_str,
        Args&&... args
    )
    {
        if (cond.empty())
            return;

        loggerObj.setAssertCondition(cond); // Set the assertion condition
        setLoggerProperties(fileName,
                            funcName,
                            FORWARD_ANGLE,
                            lineNo,
                            std::this_thread::get_id(),
                            LOG_TYPE::LOG_ASSERT);

        logMsg(format_str, args...);
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

    /**
     * @brief Log a fatal error message and abort the program.
     *
     * This function logs a fatal error message with the specified format and arguments.
     * It sets the logger properties such as file name, function name, line number, thread ID,
     * and log type before logging the message. After logging, it aborts the program.
     *
     * @tparam Args Variadic template parameters for additional arguments to be formatted into the log message.
     * @param [in] fileName The name of the file where the log is being generated.
     * @param [in] funcName The name of the function where the log is being generated.
     * @param [in] lineNo The line number in the source code where the log is being generated.
     * @param [in] format_str The format string for the log message.
     * @param [in] args Additional arguments to be formatted into the log message.
     */
    template<typename ...Args>
    void log_fatal
    (
        const std::string_view fileName,
        const std::string_view funcName,
        const size_t lineNo,
        const std::string_view format_str,
        Args&&... args
    )
    {
        setLoggerProperties(fileName,
                            funcName,
                            FORWARD_ANGLE,
                            lineNo,
                            std::this_thread::get_id(),
                            LOG_TYPE::LOG_FATAL);

        logMsg(format_str, args...);
        std::abort();
    }
};  // namespace logger

#endif // LOGGER_HELPER_HPP