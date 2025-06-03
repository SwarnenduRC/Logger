#ifndef CONSOLE_OPS_HPP
#define CONSOLE_OPS_HPP

#include "DataOps.hpp"

#include <iostream>
#include <string_view>

using DataQ = std::queue<std::pair<std::string, std::string>>;

class ConsoleOps : public DataOps
{
    public:
        enum class LoggingTypes
        {
            ERROR_LOG,
            INFO_LOG,
            DEBUG_LOG,
            DEFAULT_LOG
        };

        static std::vector<std::pair<const std::string_view, const LoggingTypes>> 
            LoggingTypesEnumStringMap;

        ConsoleOps();
        ~ConsoleOps();
        ConsoleOps(const ConsoleOps& rhs) = delete;
        ConsoleOps(ConsoleOps&& rhs) = delete;
        ConsoleOps& operator=(const ConsoleOps& rhs) = delete;
        ConsoleOps& operator=(ConsoleOps&& rhs) = delete;

        void flush();
        void writeToConsole();

        static ConsoleOps::LoggingTypes loggingTypesStringToEnum(const std::string_view enumString) noexcept;
        static std::string loggingTypesEnumToString(const ConsoleOps::LoggingTypes& loggingType) noexcept;

    protected:
        void writeToOutStreamObject(BufferQ&& dataQueue, std::exception_ptr& excpPtr) override;

    private:
        std::mutex m_mtx;
        std::condition_variable m_cv;
        std::atomic_bool m_isOpsRunning;
};

#endif  //CONSOLE_OPS_HPP