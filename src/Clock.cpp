/**
 * File: Clock.cpp
 * Brief: Implementation of the Clock class for time measurement and formatting.
 * 
 * This file contains the implementation of the Clock class, which provides
 * functionalities for measuring elapsed time, getting the current time in
 * various formats, and retrieving specific components of the current time.
 * 
 * Copyright (c) 2025 Swarnendu RC
 * 
 * @license MIT License
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Clock.hpp"

#include <iomanip>

using namespace logger;

void Clock::start()
{
    if (m_isRunning)
        return;

    std::lock_guard<std::mutex> lock(m_mutex);
    m_startTime = std::chrono::high_resolution_clock::now();
    m_isRunning = true;
    m_threadId = std::this_thread::get_id();
}

void Clock::stop()
{
    if (!m_isRunning || (m_threadId != std::this_thread::get_id()))
        return;
    
    std::lock_guard<std::mutex> lock(m_mutex);
    m_endTime = std::chrono::high_resolution_clock::now();
    m_isRunning = false;
    m_condition.notify_one();
}

double Clock::getElapsedTime(const TimeUnits& units)
{
    if (m_isRunning || (m_threadId != std::this_thread::get_id()))
        return -1.0;

    switch (units)
    {
        case TimeUnits::SECONDS:
        return std::chrono::duration_cast<std::chrono::seconds>(m_endTime - m_startTime).count();
        case TimeUnits::MILLISECONDS:
            return std::chrono::duration_cast<std::chrono::milliseconds>(m_endTime - m_startTime).count();
        case TimeUnits::MICROSECONDS:
            return std::chrono::duration_cast<std::chrono::microseconds>(m_endTime - m_startTime).count();
        case TimeUnits::NANOSECONDS:
            return std::chrono::duration_cast<std::chrono::nanoseconds>(m_endTime - m_startTime).count();
        default:
            return -1.0;
    }
}

std::string Clock::getGmtTime() const
{
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto gmtTime = std::gmtime(&now);
    std::ostringstream oss;
    oss << std::put_time(gmtTime, "%c %Z");
    return oss.str();
}

std::string Clock::getLocalTime() const
{
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%c %Z");
    return oss.str();
}

std::string Clock::getGmtTimeStr(const std::string_view format) const
{
    auto now = std::chrono::system_clock::now();
    auto nowTimeT = std::chrono::system_clock::to_time_t(now);
    std::array<char, 80> buffer;
    std::strftime(buffer.data(), sizeof(buffer), 
                    (format.empty() ? m_strFormat.data() : format.data()), 
                    std::gmtime(&nowTimeT));
    return std::string(buffer.data());
}

std::string Clock::getLocalTimeStr(const std::string_view format) const
{
    auto nowTimeT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto localTimeT = std::localtime(&nowTimeT);
    std::array<char, 80> buffer;
    std::strftime(buffer.data(), sizeof(buffer), 
                    (format.empty() ? m_strFormat.data() : format.data()), 
                    localTimeT);
    return std::string(buffer.data());
}

std::string Clock::getDayOfWeek() const
{
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%A");
    return oss.str();
}

std::string Clock::getMonth() const
{
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%B");
    return oss.str();
}

std::string Clock::getYear() const
{
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto localTime = std::localtime(&now);
    std::ostringstream oss;
    oss << std::put_time(localTime, "%Y");
    return oss.str();
}

std::tuple<int, int, int> Clock::getTimeOfTheDay() const
{
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto localTime = std::localtime(&now);
    return std::make_tuple(localTime->tm_hour, localTime->tm_min, localTime->tm_sec);
}

std::tuple<int, int, int> Clock::getGmtTimeOfTheDay() const
{
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto gmtTime = std::gmtime(&now);
    return std::make_tuple(gmtTime->tm_hour, gmtTime->tm_min, gmtTime->tm_sec);
}

