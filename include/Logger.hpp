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
 * @file Logger.hpp
 * @brief Defines the Logger class and related types for structured logging.
 *
 * This file provides the declaration of the Logger class, which supports
 * configurable, thread-aware, and type-based logging with customizable
 * formatting. It also defines log type enumerations, string constants for
 * formatting, and utility functions for log type conversions.
 */

#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Clock.hpp"
#include "LoggingOps.hpp"

#include <cassert>
#include <fmtmsg.h>
#include <unordered_map>

namespace logger
{
    /**
     * @brief Enum class for different log types.
     *
     * This enum class defines various log types that can be used to categorize
     * log messages. Each type corresponds to a specific logging level or purpose.
     * The values are used to control the verbosity and importance of log messages.
     */
    enum class LOG_TYPE
    {
        LOG_ERR     = 0x01,
        LOG_INFO    = 0x02,
        LOG_DBG     = 0x03,
        LOG_FATAL   = 0x04,
        LOG_WARN    = 0x05,
        LOG_IMP     = 0x06,
        LOG_ASSERT  = 0x07,
        // Any new type/entry should be added above this. Also update the maps
        // m_stringToEnumMap & m_EnumToStringMap, accordingly in the CPP file.
        LOG_DEFAULT = 0xFF
    };

    /**
     * @brief String constants for log formatting and separators.
     *
     * These constants are used to define common string patterns and separators
     * used in log messages. They help maintain consistency in log formatting
     * across different parts of the application.
     */
    inline static constexpr std::string_view VERTICAL_SEP         = "|";
    inline static constexpr std::string_view COLONE_SEP           = ":";
    inline static constexpr std::string_view DOT_SEP              = ".";
    inline static constexpr std::string_view ONE_SPACE            = " ";
    inline static constexpr std::string_view TWO_SPACES           = "  ";
    inline static constexpr std::string_view THREE_SPACES         = "   ";
    inline static constexpr std::string_view FOUR_SPACES          = "    ";
    inline static constexpr std::string_view ONE_TAB              = "\t";
    inline static constexpr std::string_view TWO_TABS             = "\t\t";
    inline static constexpr std::string_view ONE_LINE_BREAK       = "\n";
    inline static constexpr std::string_view TWO_LINE_BREAKS      = "\n\n";
    inline static constexpr std::string_view FORWARD_ANGLE        = ">";
    inline static constexpr std::string_view FORWARD_ANGLES       = ">>";
    inline static constexpr std::string_view BACKWARD_ANGLE       = ">>";
    inline static constexpr std::string_view BACKWARD_ANGLES      = "<<";
    inline static constexpr std::string_view LEFT_SQUARE_BRACE    = "[";
    inline static constexpr std::string_view RIGHT_SQUARE_BRACE   = "]";
    inline static constexpr std::string_view LEFT_CURLEY_BRACE    = "{";
    inline static constexpr std::string_view RIGHT_CURLEY_BRACE   = "}";
    inline static constexpr std::string_view LEFT_OPENING_BRACE   = "(";
    inline static constexpr std::string_view RIGHT_CLOSING_BRACE  = ")";
    inline static constexpr std::string_view DOUBLE_QUOTES        = "\"";
    inline static constexpr std::string_view SINGLE_QUOTE         = "'";
    inline static constexpr std::string_view FIELD_SEPARATOR      = VERTICAL_SEP;

    /**
     * @brief Type alias for amps used in logging.
     *
     * This type alias defines two maps for converting between
     * log type strings and their corresponding enum values.
     * The first map converts strings to log type enums, and the second
     * map converts log type enums to their string representations.
     */
    using UNORD_STRING_MAP = std::unordered_map<std::string, LOG_TYPE>;
    using UNORD_LOG_TYPE_MAP = std::unordered_map<LOG_TYPE, std::string>;

    class Logger
    {
        public:
            /**
             * @brief Converts a string to a LOG_TYPE enum.
             *
             * This function takes a string representation of a log type
             * and converts it to the corresponding LOG_TYPE enum value.
             * If the string does not match any known log type, it returns
             * LOG_TYPE::LOG_DEFAULT.
             *
             * @param [in] type The string representation of the log type.
             * @return The corresponding LOG_TYPE enum value.
             */
            static LOG_TYPE convertStringToLogTypeEnum(const std::string_view type) noexcept;

            /**
             * @brief Converts a LOG_TYPE enum to a string representation.
             *
             * This function takes a LOG_TYPE enum value and converts it
             * to its string representation. If the enum value does not
             * match any known log type, it returns an empty string.
             *
             * @param [in] type The LOG_TYPE enum value.
             * @return The string representation of the log type.
             */
            static std::string covertLogTypeEnumToString(const LOG_TYPE& type) noexcept;

            /**
             * @brief Builds and returns a LoggingOps object.
             *
             * This function creates and returns a reference to a
             * LoggingOps object that is used for logging operations.
             * It ensures that the LoggingOps object is initialized
             * only once and can be reused across multiple Logger instances.
             *
             * @return A reference to the LoggingOps object.
             */
            static LoggingOps& buildLoggingOpsObject() noexcept;

            Logger() = delete;
            Logger(const std::string_view timeFormat);
            virtual ~Logger() = default;
            Logger(const Logger& rhs) = delete;
            Logger(Logger&& rhs) = delete;
            Logger& operator=(const Logger& rhs) = delete;
            Logger& operator=(Logger&& rhs) = delete;

            /**
             * @brief Set the Thread Id object
             * This function sets the thread ID for the logger
             * @note It is used to identify which thread is logging the message.
             * 
             * @param [in] val The thread ID to be set.
             * @note It is a reference to the current thread ID.
             * @return Logger& Returns a reference to the Logger object
             * to allow for method chaining.
             */
            Logger& setThreadId(const std::thread::id& val) noexcept;

            /**
             * @brief Set the Line Number object
             * This function sets the line number for the logger
             * @note It is used to identify which line of code is logging the message.
             * 
             * @param [in] val The line number to be set.
             * @return Logger& Returns a reference to the Logger object
             * to allow for method chaining.
             */
            Logger& setLineNo(const size_t val) noexcept;

            /**
             * @brief Set the Function Name object
             * This function sets the function name for the logger
             * @note It is used to identify which function is logging the message.
             * 
             * @param [in] val The function name to be set.
             * @return Logger& Returns a reference to the Logger object
             * to allow for method chaining.
             */
            Logger& setFunctionName(const std::string_view val) noexcept;

            /**
             * @brief Set the File Name object
             * This function sets the file name for the logger
             * @note It is used to identify which file is logging the message.
             * 
             * @param [in] val The file name to be set.
             * @return Logger& Returns a reference to the Logger object
             * to allow for method chaining.
             */
            Logger& setFileName(const std::string_view val) noexcept;

            /**
             * @brief Set the Log Marker object
             * This function sets the log marker for the logger
             * @note It is used to identify the type of log message.
             * e.g., function entry, exit, or any other custom marker.
             * 
             * @param [in] val The log marker to be set.
             * @return Logger& Returns a reference to the Logger object
             * to allow for method chaining.
             */
            Logger& setMarker(const std::string_view val) noexcept;

            /**
             * @brief Set the Log Type object
             * This function sets the log type for the logger
             * @note It is used to categorize the log message.
             * e.g., error, info, debug, etc.
             * 
             * @param [in] logType The log type to be set.
             * @return Logger& Returns a reference to the Logger object
             * to allow for method chaining.
             */
            Logger& setLogType(const LOG_TYPE& logType) noexcept;

            /**
             * @brief Set the Log Type object
             * This function sets the log type for the logger
             * @note It is used to categorize the log message.
             * e.g., error, info, debug, etc.
             * 
             * @param [in] logType The log type to be set.
             * @return Logger& Returns a reference to the Logger object
             * to allow for method chaining.
             */
            Logger& setLogType(const std::string_view logType) noexcept;

            /**
             * @brief Set the Assert Condition object
             * This function sets the assert condition for the logger
             * @note It is used to log the condition that caused an assertion failure.
             * 
             * @param [in] cond The assert condition to be set.
             * @return Logger& Returns a reference to the Logger object
             * to allow for method chaining.
             */
            Logger& setAssertCondition(const std::string_view cond) noexcept;

            /**
             * @brief Get the Log Stream object
             * This function returns the log stream
             * @note It is used to retrieve the log stream where the log messages are written.
             * 
             * @return const std::stringstream& The log stream object
             * @note It is a constant reference to the log stream object.
             * This allows read-only access to the log stream without modifying it.
             */
            inline const std::stringstream& getLogStream() const noexcept { return m_logStream; }

            /**
             * @brief Logs a message with the specified format and arguments.
             * This function formats the log message using the provided format string
             * and arguments, and writes it to the log stream.
             * 
             * @tparam Args Variadic template parameters for the arguments to be formatted into the log message.
             * @param [in] formatStr The format string for the log message.
             *                 It can contain placeholders for the arguments, similar to printf-style formatting.
             *                 For example, "Log message: {}" where {} will be replaced by the provided arguments.
             * @param [in] args The arguments to be formatted into the log message.
             *                 These arguments will be formatted according to the format string.
             *                 The number and types of arguments should match the placeholders in the format string.
             */
            template<typename ...Args>
            void log(const std::string_view formatStr, Args&&... args)
            {
                vlog(formatStr, std::make_format_args(std::forward<Args>(args)...));
            }

        protected:
            /**
             * @brief Populates prerequisite fields for logging.
             * This function initializes the necessary fields required for logging,
             * such as thread ID, clock, line number, function name, file name,
             * log marker, and log type.
             */
            virtual void populatePrerequisitFileds();

            /**
             * @brief Constructs the log message prefix.
             * This function constructs the prefix for the log message,
             * which includes the thread ID, clock time, line number,
             * function name, file name, and log marker.
             */
            virtual void constructLogMsgPrefix();

            /**
             * @brief Constructs the first part of the log message prefix.
             * This function constructs the first part of the log message prefix,
             * which includes the thread ID, clock time, and line number.
             */
            virtual void constructLogMsgPrefixFirstPart();

            /**
             * @brief Constructs the second part of the log message prefix.
             * This function constructs the second part of the log message prefix,
             * which includes the function name, file name, and log marker.
             */
            virtual void constructLogMsgPrefixSecondPart();

        private:
            static const UNORD_STRING_MAP m_stringToEnumMap;
            static const UNORD_LOG_TYPE_MAP m_EnumToStringMap;

            void vlog(const std::string_view formatStr, std::format_args args);

            std::thread::id m_threadID;
            Clock m_clock;
            size_t m_lineNo;
            std::string m_funcName;
            std::string m_fileName;
            /**
             * @brief Log marker
             * Marks what kind of log function
             * is in play. For function entry
             * it should be >>, for function exit
             * it is <<, for any other cases it
             * is >, unless customized otherwise.
             * 
             * @note By default it is FORWARD_ANGLE(>)
             */
            std::string m_logMarker = FORWARD_ANGLE.data();

            std::stringstream m_logStream;
            LOG_TYPE m_logType = LOG_TYPE::LOG_INFO;
            std::string m_assertCond;
    };
};

#endif // LOGGER_HPP