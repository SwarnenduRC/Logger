/*
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

/*
 * CommonFunc.hpp
 *
 * Purpose:
 *   This header provides utility classes and functions for generating random test data,
 *   such as random strings, binary data of various sizes, and random file names.
 *   It is intended for use in unit tests, especially with Google Test, to facilitate
 *   the creation of diverse and robust test cases. The file also includes a class for
 *   generating random hexadecimal values of different widths.
 */

#ifndef COMMON_FUNC_HPP
#define COMMON_FUNC_HPP

#include <string>
#include <vector>
#include <random>

#include <gtest/gtest.h>

#define nullString ""

class CommonTestDataGenerator : public ::testing::Test
{
    public:
        /**
         * @brief Get the path separator for the current platform
         * * This function returns the path separator for the current platform.
         * * It uses preprocessor directives to determine the correct separator.
         * * The function caches the result in a static variable to avoid repeated calculations.
         *
         * @return std::string The path separator for the current platform
         */
        static std::string getPathSeperator()
        {
            static std::string pathSep = nullString;
            if (!pathSep.empty())
                return pathSep;

#ifdef _WIN32
            pathSep += "\\"
#elif __linux__ || __APPLE__ || __FreeBSD__ || __OpenBSD__ || __NetBSD__ || __MACH__
            pathSep += "/";
#endif
            return pathSep;
        }
        /**
         * @brief Generate a random text string of specified length
         * * This function generates a random text string of specified length using a predefined character set.
         * * The character set includes digits, uppercase and lowercase letters, and some common punctuation marks.
         * * The function uses a random number generator to select characters from the character set.
         *
         * @param textLen The length of the random text string to be generated
         * @return std::string The generated random text string of specified length
         */
        static std::string generateRandomText(const size_t textLen)
        {
            static const char charset[] =
            {
                "0123456789"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "abcdefghijklmnopqrstuvwxyz"
                //" .,!?;:-_+=()[]{}<>|/@#$%^&*~`"
                //As the same function is used to genrate random file names thus
                // better to avoid special char set otherwise in some rare cases
                // it may failed to create files with some special chars in the name
            };
            std::random_device rd;   // Random seed
            std::mt19937 gen(rd());  // Mersenne Twister engine
            std::uniform_int_distribution<> dist(0, sizeof(charset) - 2); // -2 for null terminator

            std::string result;
            result.reserve(textLen);
            for (size_t i = 0; i < textLen; ++i)
            {
                result += charset[dist(gen)];
            }
            return result;
        }
        /**
         * @brief Generate random binary data of 1, 2, 4, or 8 bytes
         * * These functions generate random binary data of specified byte lengths.
         * * They use a random number generator to create binary data within the specified range.
         *
         * @param byteLen The length of the binary data to be generated
         * @return std::vector<uint8_t> The generated random binary data as a vector
         */
        static std::vector<uint8_t> generateRandomBinary_1_Byte_Data(const size_t byteLen)
        {
            std::random_device rd;   // Random seed
            std::mt19937 gen(rd());  // Mersenne Twister engine
            std::uniform_int_distribution<> dist(0, 255); //One byte

            std::vector<uint8_t> binaryStream(byteLen);
            for (size_t idx = 0; idx < byteLen; ++idx)
                binaryStream[idx] = static_cast<uint8_t>(dist(gen));

            return binaryStream;
        }
        /**
         * @brief Generate random binary data of 2, 4, or 8 bytes
         * * These functions generate random binary data of specified byte lengths.
         * * They use a random number generator to create binary data within the specified range.
         *
         * @param byteLen The length of the binary data to be generated
         * @return std::vector<uint16_t> The generated random binary data as a vector
         */
        static std::vector<uint16_t> generateRandomBinary_2_Bytes_Data(const size_t byteLen)
        {
            std::random_device rd;   // Random seed
            std::mt19937 gen(rd());  // Mersenne Twister engine
            std::uniform_int_distribution<> dist(0, 511); //Two bytes

            std::vector<uint16_t> binaryStream(byteLen);
            for (size_t idx = 0; idx < byteLen; ++idx)
                binaryStream[idx] = static_cast<uint16_t>(dist(gen));

            return binaryStream;
        }
        /**
         * @brief Generate random binary data of 4 or 8 bytes
         * * These functions generate random binary data of specified byte lengths.
         * * They use a random number generator to create binary data within the specified range.
         *
         * @param byteLen The length of the binary data to be generated
         * @return std::vector<uint32_t> The generated random binary data as a vector
         */
        static std::vector<uint32_t> generateRandomBinary_4_Bytes_Data(const size_t byteLen)
        {
            std::random_device rd;   // Random seed
            std::mt19937 gen(rd());  // Mersenne Twister engine
            std::uniform_int_distribution<> dist(0, 1023); //Four bytes

            std::vector<uint32_t> binaryStream(byteLen);
            for (size_t idx = 0; idx < byteLen; ++idx)
                binaryStream[idx] = static_cast<uint16_t>(dist(gen));

            return binaryStream;
        }
        /**
         * @brief Generate random binary data of 8 bytes
         * * This function generates random binary data of 8 bytes.
         * * It uses a random number generator to create binary data within the specified range.
         *
         * @param byteLen The length of the binary data to be generated
         * @return std::vector<uint64_t> The generated random binary data as a vector
         */
        static std::vector<uint64_t> generateRandomBinary_8_Bytes_Data(const size_t byteLen)
        {
            std::random_device rd;   // Random seed
            std::mt19937 gen(rd());  // Mersenne Twister engine
            std::uniform_int_distribution<> dist(0, 2047); //Eight bytes

            std::vector<uint64_t> binaryStream(byteLen);
            for (size_t idx = 0; idx < byteLen; ++idx)
                binaryStream[idx] = static_cast<uint16_t>(dist(gen));

            return binaryStream;
        }
        /**
         * @brief Generate a random file name with a specified prefix and extension
         * * This function generates a random file name by combining a prefix, a random alphanumeric string,
         * * and a specified file extension. The random part is generated using the generateRandomText function.
         *
         * @param prefix The prefix for the file name (default is "tmp_")
         * @param extension The file extension (default is ".txt")
         * @return std::string The generated random file name
         */
        static std::string generateRandomFileName(const std::string& prefix = "tmp_", const std::string& extension = ".txt")
        {
            std::string randomPart = generateRandomText(8);  // 8-char random string
            return prefix + randomPart + extension;
        }
};

class RandomHexGenerator
{
    public:
        RandomHexGenerator()
            : rng(std::random_device{}()) {}
        /**
         * @brief Get a random 8-bit HEX value
         * 
         * @return uint8_t Generated 8-bit HEX value
         */
        uint8_t  get8()  { return get<uint8_t>(); }
        /**
         * @brief Get a random 16-bit HEX value
         * 
         * @return uint16_t Generated 16-bit HEX value
         */
        uint16_t get16() { return get<uint16_t>(); }
        /**
         * @brief Get a random 32-bit HEX value
         * 
         * @return uint32_t Generated 32-bit HEX value
         */
        uint32_t get32() { return get<uint32_t>(); }
        /**
         * @brief Get a random 64-bit HEX value
         * 
         * @return uint64_t Generated 64-bit HEX value
         */
        uint64_t get64() { return get<uint64_t>(); }

    private:
        std::mt19937 rng;
        /**
         * @brief Generate a random value of type T
         * * This function generates a random value of the specified type T using a uniform distribution.
         * * It ensures that the generated value is within the range of the type's maximum value.
         *
         * @return T A random value of type T
         */
        template<typename T>
        T get()
        {
            std::uniform_int_distribution<uint64_t> dist(0, static_cast<uint64_t>(std::numeric_limits<T>::max()));
            return static_cast<T>(dist(rng));
        }
};

#endif // COMMON_FUNC_HPP