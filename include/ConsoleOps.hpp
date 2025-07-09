/**
 * @file ConsoleOps.hpp
 * @brief Declares the ConsoleOps class for console-based logging operations.
 *
 * This file is part of a logging library and defines the ConsoleOps class,
 * which provides thread-safe mechanisms to write log data to the console.
 * It inherits from LoggingOps and implements the required interfaces for
 * outputting log messages to standard output streams.
 *
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

#ifndef CONSOLE_OPS_HPP
#define CONSOLE_OPS_HPP

#include "LoggingOps.hpp"

#include <string>
#include <string_view>
#include <sstream>
#include <condition_variable>

namespace logger
{
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
};  // logger namespace

#endif  //CONSOLE_OPS_HPP

