/**
 * @file Clock.hpp
 * @brief A header file for the Clock class which provides various time-related functionalities.
 *
 * This file contains the declaration of the Clock class, which is used to measure elapsed time,
 * retrieve current time in different formats, and provide various time-related utilities.
 * The class supports different time units such as seconds, milliseconds, microseconds, and nanoseconds.
 * It also provides functions to get the current GMT and local time, formatted time strings, 
 * and components of the current time such as day of the week, month, and year.
 *
 * @author Swarnendu Roy Chowdhury
 * @date 12/02/2025
 * @version 1.0
 * 
 * @note This class is a part of the C++ logger library.
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

#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <chrono>
#include <thread>
#include <atomic>
#include <string>
#include <mutex>
#include <condition_variable>

namespace logger
{
    using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

    /**
     * @enum TimeUnits
     * @brief Enumeration for specifying different time units.
     *
     * This enumeration defines the various time units that can be used
     * to measure elapsed time or represent time durations. It includes
     * seconds, milliseconds, microseconds, and nanoseconds.
     */
    enum class TimeUnits
    {
        SECONDS,
        MILLISECONDS,
        MICROSECONDS,
        NANOSECONDS
    };

    /**
     * @class Clock
     * @brief A class to provide time-related functionalities.
     *
     * The Clock class provides various time-related functionalities such as
     * measuring elapsed time, retrieving current time in different formats,
     * and providing utilities to get the day of the week, month, year, etc.
     * It supports different time units for measuring elapsed time and provides
     * thread-safe timer operations.
     */
    class Clock
    {
        public:
            /**
             * @brief Default constructor for the Clock class.
             */
            Clock() = default;
            ~Clock() = default;
            /**
             * @brief Parameterized constructor for the Clock class.
             * @param format A string view representing the format for time strings.
             */
            Clock(const std::string_view format) : m_strFormat(format) {}
            /**
             * @brief Starts the timer.
             */
            void start();
            /**
             * @brief Stops the timer.
             */
            void stop();
            /**
             * @brief Gets the elapsed time in the specified time units.
             * @param units The time units for the elapsed time (default: seconds).
             * @return The elapsed time in the specified units.
             */
            double getElapsedTime(const TimeUnits& units = TimeUnits::SECONDS);
            /**
             * @brief Checks if the timer is running.
             * @return A boolean value indicating if the timer is running.
             */
            inline bool isRunning() const { return m_isRunning; };
            /**
             * @brief Gets the current GMT time as a string.
             * @return The current GMT time as a string.
             */
            std::string getGmtTime() const;
            /**
             * @brief Gets the current local time as a string.
             * @return The current local time as a string.
             */
            std::string getLocalTime() const;
            /**
             * @brief Gets the current GMT time as a formatted string.
             * @param format The format for the time string (default: empty
             *               which translates to class's default format
             *               "%d/%m/%Y %H:%M:%S" or anything else if was
             *               setup during the object construction via
             *               parameterized constructor of this class).
             * @return The current GMT time as a formatted string.
             */
            std::string getGmtTimeStr(const std::string_view format = "") const;
            /**
             * @brief Gets the current local time as a formatted string.
             * @param format The format for the time string (default: empty
             *               which translates to class's default format
             *               "%d/%m/%Y %H:%M:%S" or anything else if was
             *               setup during the object construction via
             *               parameterized constructor of this class).
             * @return The current local time as a formatted string.
             */
            std::string getLocalTimeStr(const std::string_view format = "") const;
            /**
             * @brief Gets the day of the week.
             * @return The day of the week as a string.
             */
            std::string getDayOfWeek() const;
            /**
             * @brief Gets the month.
             * @return The month as a string.
             */
            std::string getMonth() const;
            /**
             * @brief Gets the year.
             * @return The year as a string.
             */
            std::string getYear() const;
            /**
             * @brief Gets the current time of the day (hours, minutes, seconds).
             * @return A tuple containing the current time of the day.
             */
            std::tuple<int, int, int> getTimeOfTheDay() const;
            /**
             * @brief Gets the current GMT time of the day (hours, minutes, seconds).
             * @return A tuple containing the current GMT time of the day.
             */
            std::tuple<int, int, int> getGmtTimeOfTheDay() const;

        private:
            /**
             * @brief The start time of the timer.
             */
            TimePoint m_startTime;
            /**
             * @brief The end time of the timer.
             */
            TimePoint m_endTime;
            /**
             * @brief The format for time strings.
             */
            const std::string_view m_strFormat = "%d/%m/%Y %H:%M:%S";
            /**
             * @brief A boolean flag to indicate if the timer is running.
             */
            std::atomic_bool m_isRunning;
            /**
             * @brief The thread ID of the timer thread.
             */
            std::thread::id m_threadId;
            /**
             * @brief A mutex for thread safety.
             */
            std::mutex m_mutex;
            /**
             * @brief A condition variable for thread safety.
             */
            std::condition_variable m_condition;
    };
}; // logger namespace

#endif // CLOCK_HPP
