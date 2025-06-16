#ifndef LOGGER_HPP
#define LOGGER_HPP

#include "LoggingOps.hpp"
#include "Clock.hpp"

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
    static constexpr std::string_view FIELD_SEPARATOR      = VERTICAL_SEP;

    using UNORD_STRING_MAP = std::unordered_map<std::string, LOG_TYPE>;
    using UNORD_LOG_TYPE_MAP = std::unordered_map<LOG_TYPE, std::string>;

    class Logger
    {
        public:
            static LOG_TYPE convertStringToLogTypeEnum(const std::string_view type) noexcept;
            static std::string covertLogTypeEnumToString(const LOG_TYPE& type) noexcept;

            Logger() = delete;
            Logger(const std::string_view timeFormat);
            virtual ~Logger() = default;
            Logger(const Logger& rhs) = delete;
            Logger(Logger&& rhs) = delete;
            Logger& operator=(const Logger& rhs) = delete;
            Logger& operator=(Logger&& rhs) = delete;

            Logger& setThreadId(const std::thread::id& val);
            Logger& setLineNo(const size_t val);
            Logger& setFunctionName(const std::string_view val);
            Logger& setFileName(const std::string_view val);

            inline const std::stringstream& getLogStream() const noexcept { return m_logStream; }

            virtual void populatePrerequisitFileds(const LOG_TYPE& logType);
            virtual void populatePrerequisitFileds(const std::string_view logType);

        protected:
            virtual void constructLogMsgPrefix(const LOG_TYPE& logType);
            virtual void constructLogMsgPrefixFirstPart();
            virtual void constructLogMsgPrefixSecondPart();

        private:
            static const UNORD_STRING_MAP m_stringToEnumMap;
            static const UNORD_LOG_TYPE_MAP m_EnumToStringMap;

            std::unique_ptr<LoggingOps> m_pLogger;

            std::thread::id m_threadID;
            Clock m_clock;
            size_t m_lineNo;
            std::string m_funcName;
            std::string m_fileName;
            bool m_isFileNameRequired;

            std::stringstream m_logStream;
    };
};

#endif // LOGGER_HPP