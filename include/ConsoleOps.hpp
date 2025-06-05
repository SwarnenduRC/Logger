#ifndef CONSOLE_OPS_HPP
#define CONSOLE_OPS_HPP

#include "DataOps.hpp"

#include <iostream>
#include <string_view>

class ConsoleOps : public DataOps
{
        ConsoleOps();
        ~ConsoleOps();
        ConsoleOps(const ConsoleOps& rhs) = delete;
        ConsoleOps(ConsoleOps&& rhs) = delete;
        ConsoleOps& operator=(const ConsoleOps& rhs) = delete;
        ConsoleOps& operator=(ConsoleOps&& rhs) = delete;

        /**
         * @brief Get the Class Id for the object
         * This function is used to get the class id of the object.
         *
         * @return std::string The class id of the object
         * @note This function is used to identify the class of the object
         *       in the polymorphic hierarchy. It is used to identify the class
         *       of the object at runtime.
         * @see DataOps::getClassId()
         * @see DataOps::getClassId() for more details
         */
        inline const std::string getClassId() const override { return "ConsoleOps"; }

    protected:
        void writeToOutStreamObject(BufferQ&& dataQueue, std::exception_ptr& excpPtr) override;

    private:
        std::mutex m_mtx;
        std::condition_variable m_cv;
        std::atomic_bool m_isOpsRunning;
};

#endif  //CONSOLE_OPS_HPP