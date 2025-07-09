/**
 * @file FileOps.hpp
 * @brief Header file for the FileOps class, providing file operations such as reading,
 *        writing, appending, renaming, and deleting files, with thread-safe mechanisms.
 *        Supports file size limits, file content buffering, and exception handling.
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

#ifndef FILE_OPS_HPP
#define FILE_OPS_HPP

#include "LoggingOps.hpp"

#include <fstream>
#include <string>
#include <filesystem>

namespace logger
{
    using DataQ = std::queue<std::string>;

    class FileOps : public LoggingOps
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
             * @brief Read a range of bytes from the file
             *
             * Reads a range of bytes from the file specified by the FileOps object.
             * The range is specified by the start and end positions.
             *
             * @note Throws exceptions which is collected in the static
             *       std::vector<std::exception_ptr> m_excpPtrVec
             *       and can be accessed using getAllExceptions() function.
             * @note This function is not thread safe. The caller must ensure thread safety.
             *
             * @param [in] file The FileOps object
             * @param [in] start The start position of the range
             * @param [in] end The end position of the range
             * @param [out] outBuff The output buffer to store the read bytes
             *
             * @return true If the read was successful, otherwise
             * @return false
             */
            static bool readFileByteRange(FileOps& file,
                                        const std::streampos start,
                                        const std::streampos end,
                                        std::vector<char>& outBuff);

            /**
             * @brief Read a range of lines from the file
             * Reads a range of lines from the file specified by the FileOps object.
             * The range is specified by the start and end line numbers.
             *
             * @note Throws exceptions which is collected in the static
             *      std::vector<std::exception_ptr> m_excpPtrVec
             *      and can be accessed using getAllExceptions() function.
             * @note This function is not thread safe. The caller must ensure thread safety.
             *
             * @param [in] file The FileOps object
             * @param [in] startLineNo The starting line number (1-based index)
             * @param [in] endLineNo The ending line number (1-based index)
             * @note The line numbers are inclusive, meaning both startLineNo and endLineNo
             *       are included in the range.
             * @note If startLineNo is greater than endLineNo, the function will return false
             *       and will throw an exception.
             * @param [out] outBuf The output buffer to store the read lines
             * @note The outBuf will be cleared before reading the lines.
             *       If no lines are read, it will remain empty.
             * @return true If the read was successful and lines were read,
             *         otherwise
             * @return false
             */
            static bool readFileLineRange(FileOps& file,
                                    const size_t startLineNo,
                                    const size_t endLineNo,
                                    std::vector<std::string>& outBuf);


            /**
             * @brief Construct a new File Ops object
             * This is a deleted constructor to prevent
             * creating an object of FileOps without any parameters.
             * It is not allowed to create a FileOps object
             * without specifying the maximum file size.
             * This is to ensure that the file operations
             * are always performed with a valid file size limit.
             */
            FileOps() = delete;
            /**
             * @brief Construct a new File Ops object
             *
             * @param [in] maxFileSize Maximum size of the file
             * @param [in] fileName Name of the file (default blank)
             * @param [in] filePath Path where file would be placed eventually (default current path)
             * @param [in] fileExtension Extension of the file like .txt or .log etc. (default .txt)
             * @note The max file size should be greater than
             * the max line length allowed, which is 4096 bytes
             * or 4KB, defined as bufferSize to prevent truncation
             * if you are writing single line length to its full capacity.
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
            ~FileOps();

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
             * @note The max file size should be greater than
             * the max line length allowed, which is 4096 bytes
             * or 4KB, defined as bufferSize to prevent truncation
             * if you are writing single line length to its full capacity.
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
             * @brief Checks if the file path is empty or not
             *
             * @return true If the file path is empty, otherwise
             * @return false
             */
            inline bool isFilePathEmpty() const                             { return std::filesystem::is_empty(m_FilePathObj);  }
            /**
             * @brief Checks if the file exists or not
             *
             * @return true If the file exists, otherwise
             * @return false
             */
            inline bool fileExists() const                                  { return std::filesystem::exists(m_FilePathObj);    }
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
            inline const std::string getClassId() const override             { return "FileOps";                                 }
            /**
             * @brief Get the file size
             *
             * @return std::uintmax_t The file size
             */
            std::uintmax_t getFileSize();

            bool isEmpty();

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

        protected:
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

            /**
             * @brief Write data to the out stream object
             *
             * @param [in] data The data to be written to the out stream object
             * @note This function is pure virtual and must be implemented by the derived classes.
             * It is used to write the data to the out stream object.
             */
            void writeDataTo(const std::string_view data) override;

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

            /// Data members for file opening, closing, reading and writing
            std::string m_FileName;
            std::string m_FilePath;
            std::string m_FileExtension;
            DataQ m_FileContent;
            std::filesystem::path m_FilePathObj;
            std::uintmax_t m_MaxFileSize;
            std::mutex m_FileOpsMutex;
            std::condition_variable m_FileOpsCv;
            std::atomic_bool m_isFileOpsRunning;
    };
};  //logger namespace

#endif // FILE_OPS_HPP