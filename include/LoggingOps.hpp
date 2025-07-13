/**
 * @file LoggingOps.hpp
 * @brief Declaration of the LoggingOps class for thread-safe data queue operations.
 *
 * @copyright
 * MIT License
 * 
 * Copyright (c) 2024 Swarnendu RC
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

#ifndef LOGGING_OPS_HPP
#define LOGGING_OPS_HPP

#include <queue>
#include <vector>
#include <list>
#include <array>
#include <mutex>
#include <thread>
#include <atomic>
#include <exception>
#include <condition_variable>

namespace logger
{
    constexpr size_t bufferSize = 4097; //4KB each line length max (+1 for NULL char)

    using BufferQ = std::queue<std::array<char, bufferSize>>;

    class LoggingOps
    {
        public:
            /**
             * @brief Overloaded output operator for LoggingOps class
             *
             * @param [in] oss The output string stream
             * @param [inout] obj The LoggingOps object
             * @note Doesn't support chaining. It is used to write data to the LoggingOps object.
             */
            friend void operator<<(LoggingOps& obj, const std::ostringstream& oss);

            /**
             * @brief Overloaded output operator for LoggingOps class
             *
             * @param [in] iss The input string stream
             * @param [inout] obj The LoggingOps object
             * @note Doesn't support chaining. It is used to write data to the LoggingOps object.
             */
            friend void operator<<(LoggingOps& obj, const std::istringstream& iss);

            /**
             * @brief Overloaded output operator for LoggingOps class
             *
             * @param [in] data The data to be written to the LoggingOps object
             * @param [inout] obj The LoggingOps object
             * @note Doesn't support chaining. It is used to write data to the LoggingOps object.
             */
            friend void operator<<(LoggingOps& obj, const std::string_view data);

            /**
             * @brief Overloaded output operator for LoggingOps class
             *
             * @param [in] dataVec The data vector to be written to the LoggingOps object
             * @param [inout] obj The LoggingOps object
             * @note Doesn't support chaining. It is used to write data to the LoggingOps object.
             */
            friend void operator<<(LoggingOps& obj, const std::vector<std::string>& dataVec);

            /**
             * @brief Overloaded output operator for LoggingOps class
             *
             * @param [in] dataList The data list to be written to the LoggingOps object
             * @param [inout] obj The LoggingOps object
             * @note Doesn't support chaining. It is used to write data to the LoggingOps object.
             */
            friend void operator<<(LoggingOps& obj, const std::list<std::string>& dataList);

            /**
             * @brief Get all the exceptions happened during the file ops
             *
             * @return std::filesystem::path The file path object
             */
            inline const std::vector<std::exception_ptr>& getAllExceptions() noexcept       { return m_excpPtrVec; }

            /**
             * @brief Add a raised exception to the exception vector
             *
             * @param [in] excpPtr The exception pointer to be added
             * @note This function is thread safe. It uses mutex to ensure that
             * only one thread can add an exception at a time.
             */
            inline void addRaisedException(const std::exception_ptr& excpPtr) noexcept      { m_excpPtrVec.emplace_back(excpPtr); }

            /**
             * @brief Default constructor for LoggingOps class
             * Initializes the data records queue, data ready flag,
             * shutdown and exit flag, and starts the watcher thread
             *
             * @note The watcher thread will keep watch and pull the data
             * from the data records queue and write it to the file
             * whenever it is available.
             * @note This function is thread safe. It uses mutex and condition variable
             * to ensure that only one thread can keep watch and pull the data at a time.
             */
            LoggingOps();

            /**
             * @brief Destructor for LoggingOps class
             * Stops the watcher thread and clears the data records queue.
             * 
             * @note It also collects and prints any exceptions that occurred
             * during the data operations before the object is destroyed.
             *
             * @note This function is thread safe. It uses mutex and condition variable
             * to ensure that only one thread can stop the watcher thread and clear the data records queue at a time.
             */
            virtual ~LoggingOps();

            /**
             * @brief Deleted copy constructor and move constructor
             * to prevent copying and moving of LoggingOps objects
             */
            LoggingOps(const LoggingOps& rhs) = delete;
            LoggingOps(LoggingOps&& rhs) = delete;
            LoggingOps& operator=(const LoggingOps& rhs) = delete;
            LoggingOps& operator=(LoggingOps&& rhs) = delete;

            /**
             * @brief flush the data records queue.
             * It checks the data records queue and if it is not empty then
             * notifies the watcher thread to write the data to the file/console
             * immediately.
             */
            void flush();

            /**
             * @brief write the data.
             * Writes the data passed to it. The data is  pushed to the
             * data records queue and then the watcher thread will pick it up.
             *
             * @param [in] data The data to be written to the file
             */
            void write(const std::string_view data);

            /**
             * @brief write the data.
             * Writes the data passed to it. The data is  pushed to the
             * data records queue and then the watcher thread will pick it up.
             *
             * @param [in] dataVec The data in a vector to be written to the file
             */
            void write(const std::vector<std::string_view>& dataVec) noexcept;

            /**
             * @brief write the data.
             * Writes the data passed to it. The data is  pushed to the
             * data records queue and then the file watcher thread will pick it up.
             *
             * @param [in] dataList The data in a list to be written to the file
             */
            void write(const std::list<std::string_view>& dataList) noexcept;

            /**
             * @brief write the data.
             * Writes the data passed to it. The data is  pushed to the
             * data records queue and then the watcher thread will pick it up.
             *
             * @param [in] dataVec The data in a vector to be written to the file
             */
            void write(const std::vector<std::string>& dataVec) noexcept;

            /**
             * @brief write the data.
             * Writes the data passed to it. The data is  pushed to the
             * data records queue and then the file watcher thread will pick it up.
             *
             * @param [in] dataList The data in a list to be written to the file
             */
            void write(const std::list<std::string>& dataList) noexcept;

            /**
             * @brief write the data.
             * Writes the binary data passed to it. The data is  pushed to the
             * data records queue and then the file watcher thread will pick it up.
             *
             * @param [in] data The data to be written to. Is of type uint8_t
             */
            void write(const uint8_t data);

            /**
             * @brief write the data.
             * Writes the data passed to it. The data is  pushed to the
             * data records queue and then the file watcher thread will pick it up.
             *
             * @param [in] data The data to be written to the out stream. Is of type uint16_t
             * @note The data is converted to a string of 16 bits and then written to the out stream.
             */
            void write(const uint16_t data);

            /**
             * @brief write the data.
             * Writes the  data passed to it. The data is  pushed to the
             * data records queue and then the watcher thread will pick it up.
             *
             * @param [in] data The data to be written to the outstream. Is of type uint64_t
             * @note The data is converted to a string of 64 bits and then written to.
             */
            void write(const uint64_t data);

            /**
             * @brief write the data.
             * Writes the data passed to it. The data is  pushed to the
             * data records queue and then the watcher thread will pick it up.
             *
             * @param [in] data The data to be written to the outstream. Is of type uint32_t
             * @note The data is converted to a string of 32 bits and then written to.
             */
            void write(const uint32_t data);
            /**
             * @brief write the data.
             * Writes the out stream with the binary data stream passed to it. The data is  pushed to the
             * data records queue and then the watcher thread will pick it up.
             *
             * @param [in] binaryStream The binary data stream (uint8_t) to be written to.
             * @note It internally calls write(const uint8_t data) in a loop
             * @see write(const uint8_t data)
             */
            void write(const std::vector<uint8_t>& binaryStream);

            /**
             * @brief write the data.
             * Writes the out stream with the binary data stream passed to it. The data is  pushed to the
             * data records queue and then the watcher thread will pick it up.
             *
             * @param [in] binaryStream The binary data stream (uint16_t) to be written to.
             * @note It internally calls write(const uint16_t data) in a loop
             * @see write(const uint16_t data)
             */
            void write(const std::vector<uint16_t>& binaryStream);

            /**
             * @brief write the data.
             * Writes the out stream with the binary data stream passed to it. The data is  pushed to the
             * data records queue and then the watcher thread will pick it up.
             *
             * @param [in] binaryStream The binary data stream (uint32_t) to be written to.
             * @note It internally calls write(const uint32_t data) in a loop
             * @see write(const uint32_t data)
             */
            void write(const std::vector<uint32_t>& binaryStream);

            /**
             * @brief write the data.
             * Writes the out stream with the binary data stream passed to it. The data is  pushed to the
             * data records queue and then the watcher thread will pick it up.
             *
             * @param [in] binaryStream The binary data stream (uint64_t) to be written to.
             * @note It internally calls write(const uint64_t data) in a loop
             * @see write(const uint64_t data)
             */
            void write(const std::vector<uint64_t>& binaryStream);

            /**
             * @brief Append the data.
             * Appends the outstream object with the data passed to it.
             *
             * @param [in] data The data to be appended to the outstream object
             * @note It internally calls write(const std::string_view data)
             * @see write(const std::string_view data)
             */
            void append(const std::string_view data);

            /**
             * @brief Append the data.
             * Appends the outstream object with the data passed to it.
             *
             * @param [in] data The data to be appended to the outstream object.
             * Is of type uint8_t
             * @note It internally calls write(const uint8_t data)
             * @see write(const uint8_t data)
             */
            void append(const uint8_t data);

            /**
             * @brief Append the data.
             * Appends the outstream object with the data passed to it.
             *
             * @param [in] data The data to be appended to the outstream object.
             * Is of type uint16_t
             * @note It internally calls write(const uint16_t data)
             * @see write(const uint16_t data)
             */
            void append(const uint16_t data);

            /**
             * @brief Append the data.
             * Appends the outstream object with the data passed to it.
             *
             * @param [in] data The data to be appended to the outstream object.
             * Is of type uint64_t
             * @note It internally calls write(const uint64_t data)
             * @see write(const uint64_t data)
             */
            void append(const uint64_t data);

            /**
             * @brief Append the data.
             * Appends the outstream object with the data passed to it.
             *
             * @param [in] data The data to be appended to the outstream object.
             * Is of type uint32_t
             * @note It internally calls write(const uint32_t data)
             * @see write(const uint32_t data)
             */
            void append(const uint32_t data);

            /**
             * @brief Append the data.
             * Appends the outstream object with the binary data stream passed to it.
             *
             * @param [in] binaryStream The binary data stream (uint8_t) to be appended to the outstream object.
             * @note It internally calls write(const std::vector<uint8_t>& binaryStream)
             * @see write(const std::vector<uint8_t>& binaryStream)
             */
            void append(const std::vector<uint8_t>& binaryStream);

            /**
             * @brief Append the data.
             * Appends the outstream object with the binary data stream passed to it.
             *
             * @param [in] binaryStream The binary data stream (uint16_t) to be appended to the outstream object.
             * @note It internally calls write(const std::vector<uint16_t>& binaryStream)
             * @see write(const std::vector<uint16_t>& binaryStream)
             */
            void append(const std::vector<uint16_t>& binaryStream);

            /**
             * @brief Append the data.
             * Appends the outstream object with the binary data stream passed to it.
             *
             * @param [in] binaryStream The binary data stream (uint32_t) to be appended to the outstream object.
             * @note It internally calls write(const std::vector<uint32_t>& binaryStream)
             * @see write(const std::vector<uint32_t>& binaryStream)
             */
            void append(const std::vector<uint32_t>& binaryStream);

            /**
             * @brief Append the data.
             * Appends the outstream object with the binary data stream passed to it.
             *
             * @param [in] binaryStream The binary data stream (uint64_t) to be appended to the outstream object.
             * @note It internally calls write(const std::vector<uint64_t>& binaryStream)
             * @see write(const std::vector<uint64_t>& binaryStream)
             */
            void append(const std::vector<uint64_t>& binaryStream);

            inline virtual const std::string getClassId() const = 0;

        protected:
            /**
             * @brief Keep watch and pull the data from the data records queue
             *
             * @note This function is thread safe. It uses mutex and condition variable
             * to ensure that only one thread can keep watch and pull the data at a time.
             *
             * @see writeToFile
             * @see pop
             * @see push
             */
            void keepWatchAndPull();

            /**
             * @brief Write to the file
             *
             * @param [in] dataQueue The data queue to be written to the file
             * @param [out] excpPtr The exception pointer to be used for exception handling
             *
             * @note This function is thread safe. It uses mutex and condition variable
             * to ensure that only one thread can write to the outstream object at a time.
             */
            virtual void writeToOutStreamObject(BufferQ&& /*dataQueue*/, std::exception_ptr& /*excpPtr*/) {}

            /**
             * @brief Write data to the out stream object
             *
             * @param [in] data The data to be written to the out stream object
             * @note This function is pure virtual and must be implemented by the derived classes.
             * It is used to write the data to the out stream object.
             */
            virtual void writeDataTo(const std::string_view data) = 0;

            /**
             * @brief Pops the data to a data buffer
             *
             * @param [out] data The data to be popped from the data records queue
             * @return true If the data was popped successfully, otherwise
             * @return false
             */
            bool pop(BufferQ& data);

            /**
             * @brief Push the data to the data records queue
             *
             * @param [in] data The data to be pushed to the data records queue
             * @note This function is thread safe. It uses mutex and condition variable
             * to ensure that only one thread can push the data at a time.
             */
            void push(const std::string_view data);

            BufferQ m_DataRecords;
            std::mutex m_DataRecordsMtx;
            std::condition_variable m_DataRecordsCv;
            std::atomic_bool m_dataReady;
            std::atomic_bool m_shutAndExit;
            std::thread m_watcher;

            /**
             * @brief It is a vector of exception pointers
             * which is used to store the exceptions occurred
             * during the data operations.
             */
            std::vector<std::exception_ptr> m_excpPtrVec;

        private:
            /**
             * @brief Collect and print the exceptions occurred during the data operations
             * This function will collect all the exceptions stored in the m_excpPtrVec (if any).
             * It will then print them to a file named "LoggingExceptionsList.txt" 
             * in the current working directory.
             *
             * @note This function is called at the end of the lifetime of the LoggingOps object
             *       when the program is about to exit, or when the LoggingOps object is destroyed
             *       to ensure that all the exceptions are logged before the program exits.
             *
             * @note This function is thread safe. It uses mutex to ensure that only one thread can collect and print the exceptions at a time.
             */
            void collectAndPrintExceptions();

            /**
             * @brief The name of the file where the exceptions will be logged
             * It is a static constexpr string_view which is used to store the name of the file
             * where the exceptions will be logged.
             */
            static constexpr std::string_view m_ExcpLogFileName = "LoggingExceptionsList.txt";
            /**
             * @brief The field separator used in the log file
             * It is a static constexpr string_view which is used to separate the fields in the log file.
             */
            static constexpr std::string_view m_FieldSep = "|";
    };
};  // namespace logger

#endif  //LOGGING_OPS_HPP

