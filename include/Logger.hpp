#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "Clock.hpp"
#include "LoggingOps.hpp"

#include <cassert>

#ifdef __linux__
#include <fmt/core.h>
#elif __APPLE__ || __MACH__
#include <__format/format_arg_store.h>
#endif

#include <unordered_map>

namespace logger
{
    enum class LOG_TYPE
    {
        LOG_ERR     = 0x01,
        LOG_INFO    = 0x02,
        LOG_DBG     = 0x03,
        LOG_FATAL   = 0x04,
        LOG_WARN    = 0x05,
        LOG_IMP     = 0x06,
        LOG_ASSERT  = 0x07,
        LOG_DEFAULT = 0xFF
    };

    static constexpr std::string_view VERTICAL_SEP         = "|";
    static constexpr std::string_view COLONE_SEP           = ":";
    static constexpr std::string_view DOT_SEP              = ".";
    static constexpr std::string_view ONE_SPACE            = " ";
    static constexpr std::string_view TWO_SPACES           = "  ";
    static constexpr std::string_view THREE_SPACES         = "   ";
    static constexpr std::string_view FOUR_SPACES          = "    ";
    static constexpr std::string_view ONE_TAB              = "\t";
    static constexpr std::string_view TWO_TABS             = "\t\t";
    static constexpr std::string_view ONE_LINE_BREAK       = "\n";
    static constexpr std::string_view TWO_LINE_BREAKS      = "\n\n";
    static constexpr std::string_view FORWARD_ANGLE        = ">";
    static constexpr std::string_view FORWARD_ANGLES       = ">>";
    static constexpr std::string_view BACKWARD_ANGLE       = ">>";
    static constexpr std::string_view BACKWARD_ANGLES      = "<<";
    static constexpr std::string_view LEFT_SQUARE_BRACE    = "[";
    static constexpr std::string_view RIGHT_SQUARE_BRACE   = "]";
    static constexpr std::string_view LEFT_CURLEY_BRACE    = "{";
    static constexpr std::string_view RIGHT_CURLEY_BRACE   = "}";
    static constexpr std::string_view LEFT_OPENING_BRACE   = "(";
    static constexpr std::string_view RIGHT_CLOSING_BRACE  = ")";
    static constexpr std::string_view DOUBLE_QUOTES        = "\"";
    static constexpr std::string_view SINGLE_QUOTE         = "'";
    static constexpr std::string_view FIELD_SEPARATOR      = VERTICAL_SEP;

    using UNORD_STRING_MAP = std::unordered_map<std::string, LOG_TYPE>;
    using UNORD_LOG_TYPE_MAP = std::unordered_map<LOG_TYPE, std::string>;

    class Logger
    {
        public:
            static LOG_TYPE convertStringToLogTypeEnum(const std::string_view type) noexcept;
            static std::string covertLogTypeEnumToString(const LOG_TYPE& type) noexcept;
            static LoggingOps& buildLoggingOpsObject() noexcept;

            Logger() = delete;
            Logger(const std::string_view timeFormat);
            virtual ~Logger() = default;
            Logger(const Logger& rhs) = delete;
            Logger(Logger&& rhs) = delete;
            Logger& operator=(const Logger& rhs) = delete;
            Logger& operator=(Logger&& rhs) = delete;

            Logger& setThreadId(const std::thread::id& val) noexcept;
            Logger& setLineNo(const size_t val) noexcept;
            Logger& setFunctionName(const std::string_view val) noexcept;
            Logger& setFileName(const std::string_view val) noexcept;
            Logger& setMarker(const std::string_view val) noexcept;
            Logger& setLogType(const LOG_TYPE& logType) noexcept;
            Logger& setLogType(const std::string_view logType) noexcept;
            Logger& setAssertCondition(const std::string_view cond) noexcept;


            inline const std::stringstream& getLogStream() const noexcept { return m_logStream; }

            template<typename ...Args>
            void log(const std::string_view formatStr, Args&&... args)
            {
                vlog(formatStr, std::make_format_args(std::forward<Args>(args)...));
            }

        protected:
            virtual void populatePrerequisitFileds();
            virtual void constructLogMsgPrefix();
            virtual void constructLogMsgPrefixFirstPart();
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