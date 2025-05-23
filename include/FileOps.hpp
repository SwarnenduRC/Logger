/**
 * File: FileOps.hpp
 * Description: Header file for the FileOps class, providing file operations such as reading,
 *              writing, appending, renaming, and deleting files, with thread-safe mechanisms.
 *              Supports file size limits, file content buffering, and exception handling.
 *
 * Author: Swarnendu
 * Date: May, 2025
 *
 * MIT License
 *
 * Copyright (c) 2025 Swarnendu Roy Chowdhury
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

#ifndef FILEOPS_HPP
#define FILEOPS_HPP

#include <fstream>
#include <string>
#include <queue>
#include <filesystem>
#include <thread>

using BufferQ = std::queue<std::array<char, 1025>>;
using DataQ = std::queue<std::shared_ptr<std::string>>;

class FileOps
{
    public:
        using StdTupple = std::tuple<std::string, std::string, std::string>;

        /**
         * @brief Checks if the file is empty or not
         *
         * Checks if a file specified by the input file
         * path object, is empty or not. The function
         * is static to the class and not thread safe.
         *
         * @note Throws no exception
         * @note This function is not thread safe. The caller must ensure thread safety.
         *
         * @param [in] file The file to be checked for
         *
         * @return true If the file contains some data
         * @return false otherwise. Also if the file is
         * non existing (internally calls fileExists)
         * then it also returns false.
         *
         * @see fileExists
         */
        static bool isFileEmpty(const std::filesystem::path& file) noexcept;
        /**
         * @brief Check if the file does exist at all.
         * Checks if a the file exists at all.
         *
         * @note Throws no exception
         * @note This function is not thread safe. The caller must ensure thread safety.
         *
         * @param [in] file The file to be checked for
         *
         * @return true If the file does exist, otherwise
         * @return false
         */
        static bool fileExists(const std::filesystem::path& file) noexcept;
        /**
         * @brief Remove the file if it exists
         * Internally calls fileExists to check if the file exists. If
         * it does, then it removes the file.
         * @note Throws no exception
         * @note This function is not thread safe. The caller must ensure thread safety.
         *
         * @param [in] file The file to be removed
         * @return true If the file was removed successfully, otherwise
         * @return false
         */
        static bool removeFile(const std::filesystem::path& file) noexcept;
        /**
         * @brief Clear the file if it exists
         * Internally calls fileExists to check if the file exists. If
         * it does, then it clears the file.
         * @note Throws no exception
         * @note This function is not thread safe. The caller must ensure thread safety.
         *
         * @param [in] file The file to be cleared
         * @return true If the file was cleared successfully, otherwise
         * @return false
         */
        static bool clearFile(const std::filesystem::path& file) noexcept;
        /**
         * @brief Create a file if it does not exist
         * Internally calls fileExists to check if the file exists. If
         * it does not, then it creates the file.
         * @note Throws no exception
         * @note This function is not thread safe. The caller must ensure thread safety.
         *
         * @param [in] file The file to be created
         * @return true If the file was created successfully, otherwise
         * @return false
         */
        static bool createFile(const std::filesystem::path& file) noexcept;
        /**
         * @brief Get all the exceptions happened during the file ops
         *
         * @return std::filesystem::path The file path object
         */
        static const std::vector<std::exception_ptr>& getAllExceptions() noexcept    { return m_excpPtrVec; }

        FileOps() = delete;
        /**
         * @brief Construct a new File Ops object
         *
         * @param [in] maxFileSize Maximum size of the file
         * @param [in] fileName Name of the file (default blank)
         * @param [in] filePath Path where file would be placed eventually (default current path)
         * @param [in] fileExtension Extension of the file like .txt or .log etc. (default .txt)
         */
        FileOps(const std::uintmax_t maxFileSize,
                const std::string_view fileName = "", 
                const std::string_view filePath = "", 
                const std::string_view fileExtension = "");

        /**
         * @brief Destroy the File Ops object
         * Destructor for the FileOps class. It will
         * stop the file watcher thread and clear the
         * data records queue.
         */
        virtual ~FileOps();

        /**
         * @brief Copy and move constructors and assignment operators are deleted
         *
         */
        FileOps(const FileOps&) = delete;
        /**
         * @brief Copy and move assignment operators are deleted
         *
         */
        FileOps& operator=(const FileOps&) = delete;
        /**
         * @brief Copy and move assignment operators are deleted
         *
         */
        FileOps(FileOps&&) = delete;
        /**
         * @brief Copy and move assignment operators are deleted
         *
         */
        FileOps& operator=(FileOps&&) = delete;

        /**
         * @brief Set the file name
         *
         * @param [in] fileName Name of the file
         * @return FileOps& Refrence to the current object
         *
         * @note It internally calls populateFilePathObj to adjust full
         * path of the file (if required)
         */
        FileOps& setFileName(const std::string_view fileName);
        /**
         * @brief Set the file path
         *
         * @param [in] filePath Path where file would be placed eventually
         * @return FileOps& Refrence to the current object
         *
         * @note It internally calls populateFilePathObj to adjust full
         * path of the file (if required)
         */
        FileOps& setFilePath(const std::string_view filePath);
        /**
         * @brief Set the file extension
         *
         * @param [in] fileExtension Extension of the file like .txt or .log etc.
         * @return FileOps& Refrence to the current object
         *
         * @note It internally calls populateFilePathObj to adjust full
         * path of the file (if required)
         */
        FileOps& setFileExtension(const std::string_view fileExtension);
        /**
         * @brief Set the maximum file size
         *
         * @param [in] fileSize Maximum size of the file
         * @return FileOps& Refrence to the current object
         */
        inline FileOps& setMaxFileSize(const std::uintmax_t fileSize)   { m_MaxFileSize = fileSize; return *this;           }

        /**
         * @brief Get the file name
         *
         * @return std::string The file name
         */
        inline std::string getFileName() const                          { return m_FileName;                                }
        /**
         * @brief Get the file path
         *
         * @return std::string The file path
         */
        inline std::string getFilePath() const                          { return m_FilePath;                                }
        /**
         * @brief Get the file extension
         *
         * @return std::string The file extension
         */
        inline std::string getFileExtension() const                     { return m_FileExtension;                           }
        /**
         * @brief Get the file path object
         *
         * @return std::filesystem::path The file path object
         */
        inline std::filesystem::path getFilePathObj() const             { return m_FilePathObj;                             }
        /**
         * @brief Get the maximum file size
         *
         * @return std::uintmax_t The maximum file size
         */
        inline std::uintmax_t getMaxFileSize() const                    { return m_MaxFileSize;                             }
        /**
         * @brief Get the file content
         *
         * @return DataQ The file content which is a queue of shared pointers
         *               to strings. The strings are the lines of the file.
         *               The queue is thread safe and can be accessed by multiple
         *               threads at the same time.
         */
        inline DataQ getFileContent() const                             { return m_FileContent;                             }
        /**
         * @brief Checks if the file is empty or not
         *
         * @return true If the file is empty, otherwise
         * @return false
         */
        inline bool isFileEmpty() const                                 { return std::filesystem::is_empty(m_FilePathObj);  }
        /**
         * @brief Checks if the file exists or not
         *
         * @return true If the file exists, otherwise
         * @return false
         */
        inline bool fileExists() const                                  { return std::filesystem::exists(m_FilePathObj);    }
        /**
         * @brief Get the file size
         *
         * @return std::uintmax_t The file size
         */
        std::uintmax_t getFileSize();

        /**
         * @brief write the file.
         * Writes the file with the data passed to it. The data is  pushed to the
         * data records queue and then the file watcher thread will pick it up.
         * The file is created if it does not exist. The file is opened in binary and
         * appending mode. The file is closed after writing the data.
         *
         * @param [in] data The data to be written to the file
         */
        void writeFile(const std::string_view data);
        /**
         * @brief write the file.
         * Writes the file with the binary data passed to it. The data is  pushed to the
         * data records queue and then the file watcher thread will pick it up.
         * The file is created if it does not exist. The file is opened in binary and
         * appending mode. The file is closed after writing the data.
         *
         * @param [in] data The data to be written to the file. Is of type uint8_t
         * @note The data is converted to a string of 8 bits and then written to the file.
         */
        void writeFile(const uint8_t data);
        /**
         * @brief write the file.
         * Writes the file with the data passed to it. The data is  pushed to the
         * data records queue and then the file watcher thread will pick it up.
         * The file is created if it does not exist. The file is opened in binary and
         * appending mode. The file is closed after writing the data.
         *
         * @param [in] data The data to be written to the file. Is of type uint16_t
         * @note The data is converted to a string of 16 bits and then written to the file.
         */
        void writeFile(const uint16_t data);
        /**
         * @brief write the file.
         * Writes the file with the data passed to it. The data is  pushed to the
         * data records queue and then the file watcher thread will pick it up.
         * The file is created if it does not exist. The file is opened in binary and
         * appending mode. The file is closed after writing the data.
         *
         * @param [in] data The data to be written to the file. Is of type uint64_t
         * @note The data is converted to a string of 64 bits and then written to the file.
         */
        void writeFile(const uint64_t data);
        /**
         * @brief write the file.
         * Writes the file with the data passed to it. The data is  pushed to the
         * data records queue and then the file watcher thread will pick it up.
         * The file is created if it does not exist. The file is opened in binary and
         * appending mode. The file is closed after writing the data.
         *
         * @param [in] data The data to be written to the file. Is of type uint32_t
         * @note The data is converted to a string of 32 bits and then written to the file.
         */
        void writeFile(const uint32_t data);
        /**
         * @brief write the file.
         * Writes the file with the binary data stream passed to it. The data is  pushed to the
         * data records queue and then the file watcher thread will pick it up.
         * The file is created if it does not exist. The file is opened in binary and
         * appending mode. The file is closed after writing the data.
         *
         * @param [in] binaryStream The binary data stream (uint8_t) to be written to the file.
         * @note It internally calls writeFile(const uint8_t data) in a loop
         * @see writeFile(const uint8_t data)
         */
        void writeFile(const std::vector<uint8_t>& binaryStream);
        /**
         * @brief write the file.
         * Writes the file with the binary data stream passed to it. The data is  pushed to the
         * data records queue and then the file watcher thread will pick it up.
         * The file is created if it does not exist. The file is opened in binary and
         * appending mode. The file is closed after writing the data.
         *
         * @param [in] binaryStream The binary data stream (uint16_t) to be written to the file.
         * @note It internally calls writeFile(const uint16_t data) in a loop
         * @see writeFile(const uint16_t data)
         */
        void writeFile(const std::vector<uint16_t>& binaryStream);
        /**
         * @brief write the file.
         * Writes the file with the binary data stream passed to it. The data is  pushed to the
         * data records queue and then the file watcher thread will pick it up.
         * The file is created if it does not exist. The file is opened in binary and
         * appending mode. The file is closed after writing the data.
         *
         * @param [in] binaryStream The binary data stream (uint32_t) to be written to the file.
         * @note It internally calls writeFile(const uint32_t data) in a loop
         * @see writeFile(const uint32_t data)
         */
        void writeFile(const std::vector<uint32_t>& binaryStream);
        /**
         * @brief write the file.
         * Writes the file with the binary data stream passed to it. The data is  pushed to the
         * data records queue and then the file watcher thread will pick it up.
         * The file is created if it does not exist. The file is opened in binary and
         * appending mode. The file is closed after writing the data.
         *
         * @param [in] binaryStream The binary data stream (uint64_t) to be written to the file.
         * @note It internally calls writeFile(const uint64_t data) in a loop
         * @see writeFile(const uint64_t data)
         */
        void writeFile(const std::vector<uint64_t>& binaryStream);

        /**
         * @brief Append the file.
         * Appends the file with the data passed to it.
         *
         * @param [in] data The data to be appended to the file
         * @note It internally calls writeFile(const std::string_view data)
         * @see writeFile(const std::string_view data)
         */
        void appendFile(const std::string_view data);
        /**
         * @brief Append the file.
         * Appends the file with the binary data passed to it.
         *
         * @param [in] data The data to be appended to the file. Is of type uint8_t
         * @note It internally calls writeFile(const uint8_t data)
         * @see writeFile(const uint8_t data)
         */
        void appendFile(const uint8_t data);
        /**
         * @brief Append the file.
         * Appends the file with the data passed to it.
         *
         * @param [in] data The data to be appended to the file. Is of type uint16_t
         * @note It internally calls writeFile(const uint16_t data)
         * @see writeFile(const uint16_t data)
         */
        void appendFile(const uint16_t data);
        /**
         * @brief Append the file.
         * Appends the file with the data passed to it.
         *
         * @param [in] data The data to be appended to the file. Is of type uint64_t
         * @note It internally calls writeFile(const uint64_t data)
         * @see writeFile(const uint64_t data)
         */
        void appendFile(const uint64_t data);
        /**
         * @brief Append the file.
         * Appends the file with the data passed to it.
         *
         * @param [in] data The data to be appended to the file. Is of type uint32_t
         * @note It internally calls writeFile(const uint32_t data)
         * @see writeFile(const uint32_t data)
         */
        void appendFile(const uint32_t data);
        /**
         * @brief Append the file.
         * Appends the file with the binary data stream passed to it.
         *
         * @param [in] binaryStream The binary data stream (uint8_t) to be appended to the file.
         * @note It internally calls writeFile(const std::vector<uint8_t>& binaryStream)
         * @see writeFile(const std::vector<uint8_t>& binaryStream)
         */
        void appendFile(const std::vector<uint8_t>& binaryStream);
        /**
         * @brief Append the file.
         * Appends the file with the binary data stream passed to it.
         *
         * @param [in] binaryStream The binary data stream (uint16_t) to be appended to the file.
         * @note It internally calls writeFile(const std::vector<uint16_t>& binaryStream)
         * @see writeFile(const std::vector<uint16_t>& binaryStream)
         */
        void appendFile(const std::vector<uint16_t>& binaryStream);
        /**
         * @brief Append the file.
         * Appends the file with the binary data stream passed to it.
         *
         * @param [in] binaryStream The binary data stream (uint32_t) to be appended to the file.
         * @note It internally calls writeFile(const std::vector<uint32_t>& binaryStream)
         * @see writeFile(const std::vector<uint32_t>& binaryStream)
         */
        void appendFile(const std::vector<uint32_t>& binaryStream);
        /**
         * @brief Append the file.
         * Appends the file with the binary data stream passed to it.
         *
         * @param [in] binaryStream The binary data stream (uint64_t) to be appended to the file.
         * @note It internally calls writeFile(const std::vector<uint64_t>& binaryStream)
         * @see writeFile(const std::vector<uint64_t>& binaryStream)
         */
        void appendFile(const std::vector<uint64_t>& binaryStream);

        /**
         * @brief Rename the file.
         * Renames the file with the new name passed to it.
         *
         * @param [in] newFileName The new name of the file
         * @return true If the file was renamed successfully, otherwise
         * @return false
         */
        bool renameFile(const std::string_view newFileName);

        /**
         * @brief Read the file
         * Reads the file and stores the content in the data records queue.
         * The file is opened in binary and read mode. The file is closed after reading the data.
         *
         * @note Before reading it makes sure if there is any data in the data records queue
         * which is yet to be processed. If there is, then it signals the file watcher thread
         * to process the data first before reading the file. Thread safe.
         *
         * @see keepWatchAndPull
         * @see writeToFile
         * @see pop
         * @see push
         */
        void readFile();

        /**
         * @brief Create a File object.
         * Creates a file if it does not exist.
         *
         * @return true If the file was created successfully, otherwise
         * @return false
         *
         * @note This function is thread safe. It uses mutex and condition variable
         * to ensure that only one thread can create the file at a time.
         */
        bool createFile();

        /**
         * @brief Delete the file.
         * Deletes the file if it exists.
         *
         * @return true If the file was deleted successfully, otherwise
         * @return false
         *
         * @note This function is thread safe. It uses mutex and condition variable
         * to ensure that only one thread can delete the file at a time.
         */
        bool deleteFile();

        /**
         * @brief Clear the file.
         * Clears the file contents if it exists.
         *
         * @return true If the file was cleared successfully, otherwise
         * @return false
         *
         * @note This function is thread safe. It uses mutex and condition variable
         * to ensure that only one thread can clear the file at a time.
         */
        bool clearFile();
    
    private:
        /**
         * @brief Populate the file path object.
         * It populates the file path object with the file name, file path and file extension.
         * If any of the parameters are empty, then it uses the default values.
         *
         * @param [in] fileDetails The file details to be populated
         * @note This function is thread safe. It uses mutex and condition variable
         * to ensure that only one thread can populate the file path object at a time.
         */
        void populateFilePathObj(const StdTupple& fileDetails);

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
         * to ensure that only one thread can write to the file at a time.
         */
        void writeToFile(BufferQ&& dataQueue, std::exception_ptr& excpPtr);

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

        /// Data members for file opening, closing, reading and writing
        std::string m_FileName;
        std::string m_FilePath;
        std::string m_FileExtension;
        DataQ m_FileContent;
        BufferQ m_DataRecords;
        std::filesystem::path m_FilePathObj;
        std::uintmax_t m_MaxFileSize;

        std::mutex m_FileOpsMutex;
        std::mutex m_DataRecordsMtx;
        std::condition_variable m_FileOpsCv;
        std::condition_variable m_DataRecordsCv;
        std::atomic_bool m_isFileOpsRunning = false;
        std::atomic_bool m_dataReady = false;
        std::atomic_bool m_shutAndExit = false;
        std::thread m_watcher;

        /**
         * @brief It is a vector of exception pointers
         * which is used to store the exceptions occurred
         * during the file operations. It is static and
         * shared among all the instances of the class.
         */
        static std::vector<std::exception_ptr> m_excpPtrVec;
};

#endif // FILEOPS_HPP