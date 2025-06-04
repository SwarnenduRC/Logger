/**
 * @file DataOps.hpp
 * @brief Declaration of the DataOps class for thread-safe data queue operations.
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

#ifndef DATA_OPS_HPP
#define DATA_OPS_HPP

#include <queue>
#include <vector>
#include <array>
#include <mutex>
#include <thread>
#include <atomic>
#include <exception>

using BufferQ = std::queue<std::array<char, 1025>>;

class DataOps
{
    public:
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
         * @brief Default constructor for DataOps class
         * Initializes the data records queue, data ready flag,
         * shutdown and exit flag, and starts the watcher thread
         *
         * @note The watcher thread will keep watch and pull the data
         * from the data records queue and write it to the file
         * whenever it is available.
         * @note This function is thread safe. It uses mutex and condition variable
         * to ensure that only one thread can keep watch and pull the data at a time.
         */
        DataOps();

        /**
         * @brief Destructor for DataOps class
         * Stops the watcher thread and clears the data records queue
         *
         * @note This function is thread safe. It uses mutex and condition variable
         * to ensure that only one thread can stop the watcher thread and clear the data records queue at a time.
         */
        virtual ~DataOps();

        /**
         * @brief Deleted copy constructor and move constructor
         * to prevent copying and moving of DataOps objects
         */
        DataOps(const DataOps& rhs) = delete;
        DataOps(DataOps&& rhs) = delete;
        DataOps& operator=(const DataOps& rhs) = delete;
        DataOps& operator=(DataOps&& rhs) = delete;

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
};

#endif  //DATA_OPS_HPP