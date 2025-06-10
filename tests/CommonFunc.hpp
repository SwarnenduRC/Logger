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

        uint8_t  get8()  { return get<uint8_t>(); }
        uint16_t get16() { return get<uint16_t>(); }
        uint32_t get32() { return get<uint32_t>(); }
        uint64_t get64() { return get<uint64_t>(); }

    private:
        std::mt19937 rng;

        template<typename T>
        T get()
        {
            std::uniform_int_distribution<uint64_t> dist(0, static_cast<uint64_t>(std::numeric_limits<T>::max()));
            return static_cast<T>(dist(rng));
        }
};

#endif // COMMON_FUNC_HPP