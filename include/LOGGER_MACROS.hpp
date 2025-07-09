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
 * @file LOGGER_MACROS.hpp
 * @brief Defines logging macros for convenient and consistent logging throughout the codebase.
 *
 * This header provides a set of macros that wrap logging functions, automatically including
 * file name, function name, and line number information. These macros simplify logging
 * statements and help maintain uniform log formatting and context.
 */
#ifndef LOGGER_MACROS_HPP
#define LOGGER_MACROS_HPP

#include "LogHelper.hpp"

namespace logger
{
    /**
     * @brief Macro to log a list or vector of strings with a formatted message.
     *
     * This macro logs a list or vector of strings along with a formatted message.
     * It automatically includes the file name, function name, and line number in the log.
     *
     * @param LIST_OR_VEC_OF_STRINGS The list or vector of strings to be logged.
     * @param fmt_str The format string for the log message.
     * @param ... Additional arguments for formatting the log message.
     */
    #define LOG_LIST(LIST_OR_VEC_OF_STRINGS, fmt_str, ...)                                                        \
    log_list(__FILE__, __PRETTY_FUNCTION__, __LINE__, LIST_OR_VEC_OF_STRINGS, #fmt_str __VA_OPT__(,) __VA_ARGS__)  \

    /**
     * @brief Macro to log an entry point message.
     * This macro logs a message indicating the entry point of a function or code block.
     * It automatically includes the file name, function name, and line number in the log
     * @param fmt_str The format string for the log message.
     * @param ... Additional arguments for formatting the log message.
     */
    #define LOG_ENTRY(fmt_str, ...)                                                         \
    log_entry(__FILE__, __PRETTY_FUNCTION__, __LINE__, #fmt_str __VA_OPT__(,) __VA_ARGS__);  \

    /**
     * @brief Macro to log an exit point message.
     * This macro logs a message indicating the exit point of a function or code block.
     * It automatically includes the file name, function name, and line number in the log
     * @param fmt_str The format string for the log message.
     * @param ... Additional arguments for formatting the log message.
     */
    #define LOG_EXIT(fmt_str, ...)                                                          \
    log_exit(__FILE__, __PRETTY_FUNCTION__, __LINE__, #fmt_str __VA_OPT__(,) __VA_ARGS__);   \

    /**
     * @brief Macro to log an informational message.
     * This macro logs an informational message with the specified format and arguments.
     * It automatically includes the file name, function name, and line number in the log.
     * @param fmt_str The format string for the log message.
     * @param ... Additional arguments for formatting the log message.
     */
    #define LOG_INFO(fmt_str, ...)                                                         \
    log_info(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str __VA_OPT__(,) __VA_ARGS__);   \

    /**
     * @brief Macro to log an important detail message.
     * This macro logs a message indicating an importance of the logged message.
     * It automatically includes the file name, function name, and line number in the log.
     * @param fmt_str The format string for the log message.
     * @param ... Additional arguments for formatting the log message.
     */
    #define LOG_IMP(fmt_str, ...)                                                          \
    log_imp(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str __VA_OPT__(,) __VA_ARGS__);    \

    /**
     * @brief Macro to log a warning message.
     * This macro logs a warning message with the specified format and arguments.
     * It automatically includes the file name, function name, and line number in the log.
     * @param fmt_str The format string for the log message.
     * @param ... Additional arguments for formatting the log message.
     */
    #define LOG_WARN(fmt_str, ...)                                                        \
    log_warn(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str __VA_OPT__(,) __VA_ARGS__);  \

    /**
     * @brief Macro to log an error message.
     * This macro logs an error message with the specified format and arguments.
     * It automatically includes the file name, function name, and line number in the log.
     * @param fmt_str The format string for the log message.
     * @param ... Additional arguments for formatting the log message.
     */
    #define LOG_ERR(fmt_str, ...)                                                        \
    log_err(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str __VA_OPT__(,) __VA_ARGS__);  \

    /**
     * @brief Macro to log a debug message.
     * This macro logs a debug message with the specified format and arguments.
     * It automatically includes the file name, function name, and line number in the log.
     * Note: This macro is only compiled and executed in debug mode.
     * @param fmt_str The format string for the log message.
     * @param ... Additional arguments for formatting the log message.
     * @note This macro is only compiled and executed in debug mode.
     *       If the code is compiled in release mode, this macro will not generate any code
     *       and will not log anything.
     *       This is useful for debugging purposes, as it allows developers to log debug messages
     *       without affecting the performance of the release build.
     *       It is recommended to use this macro for logging debug messages only,
     *       and not for logging important information or errors.
     *       For logging important information or errors, use the other logging macros like LOG_INFO,
     *       LOG_WARN, LOG_ERR, etc.
     */
    #define LOG_DBG(fmt_str, ...)                                                        \
    log_dbg(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str __VA_OPT__(,) __VA_ARGS__);  \

    /**
     * @brief Macro to log an assertion failure message.
     * This macro logs an assertion failure message with the specified condition and format.
     * It automatically includes the file name, function name, and line number in the log.
     * If `exitGracefuly` is true, it will exit the program gracefully.
     * @param cond The condition that failed (assertion).
     * @param exitGracefuly Whether to exit gracefully or abort on assertion failure.
     * @param fmt_str The format string for the log message.
     * @param ... Additional arguments for formatting the log message.
     */
    #define LOG_ASSERT(cond, ...)                                                                          \
    do                                                                                                      \
    {                                                                                                        \
        if (!(cond))                                                                                          \
            log_assert(__FILE__, __PRETTY_FUNCTION__, __LINE__, #cond, true, #cond __VA_OPT__(,) __VA_ARGS__); \
    } while (0);                                                                                                \

    /**
     * @brief Macro to log an assertion failure message with a custom message.
     * This macro logs an assertion failure message with the specified condition and format.
     * It automatically includes the file name, function name, and line number in the log.
     * If `exitGracefuly` is true, it will exit the program gracefully.
     * @param cond The condition that failed (assertion).
     * @param fmt_str The format string for the log message.
     * @param ... Additional arguments for formatting the log message.
     */
    #define LOG_ASSERT_MSG(cond, fmt_str, ...)                                                               \
    do                                                                                                        \
    {                                                                                                          \
        if (!(cond))                                                                                            \
            log_assert(__FILE__, __PRETTY_FUNCTION__, __LINE__, #cond, true, fmt_str __VA_OPT__(,) __VA_ARGS__); \
    } while (0);                                                                                                  \

    /**
     * @brief Macro to log a fatal error message and abort the program.
     * This macro logs a fatal error message with the specified format and arguments.
     * It automatically includes the file name, function name, and line number in the log.
     * After logging, it aborts the program.
     * @param fmt_str The format string for the log message.
     * @param ... Additional arguments for formatting the log message.
     */
    #define LOG_FATAL(fmt_str, ...)                                                        \
    log_fatal(__FILE__, __PRETTY_FUNCTION__, __LINE__, fmt_str __VA_OPT__(,) __VA_ARGS__);  \

} // namespace logger

#endif  //LOGGER_MACROS_HPP