#ifndef CONSOLE_OPS_HPP
#define CONSOLE_OPS_HPP

#include "LoggingOps.hpp"

#include <string>
#include <string_view>
#include <sstream>
#include <condition_variable>

class ConsoleOps : public LoggingOps
{
    public:
        /**
         * @brief Default constructor for ConsoleOps class
         * Initializes the console operations object.
         */
        ConsoleOps();

        /**
         * @brief Destructor for ConsoleOps class
         * Cleans up the console operations object.
         */
        virtual ~ConsoleOps();

        /**
         * @brief Deleted copy constructor and move constructor
         * to prevent copying and moving of ConsoleOps objects
         */
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
         * @see LoggingOps::getClassId()
         * @see LoggingOps::getClassId() for more details
         */
        inline const std::string getClassId() const override { return "ConsoleOps"; }

    protected:
        /**
         * @brief Write data to the out stream object
         *
         * @param [in] data The data to be written to the out stream object
         * @note This function is pure virtual and must be implemented by the derived classes.
         * It is used to write the data to the out stream object.
         */
        void writeDataTo(const std::string_view data) override;

        /**
         * @brief Write to the out stream object
         *
         * @param [in] dataQueue The data queue to be written to the out stream object
         * @param [out] excpPtr The exception pointer to be used for exception handling
         *
         * @note This function is thread safe. It uses mutex and condition variable
         * to ensure that only one thread can write to the outstream object at a time.
         */
        void writeToOutStreamObject(BufferQ&& dataQueue, std::exception_ptr& excpPtr) override;

        std::atomic_bool m_testing;
        std::ostringstream m_testStringStream;

    private:
        std::mutex m_mtx;
        std::condition_variable m_cv;
        std::atomic_bool m_isOpsRunning;
};

#endif  //CONSOLE_OPS_HPP

