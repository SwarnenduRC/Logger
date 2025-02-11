#include "Clock.hpp"

#include <iomanip>

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
    {
        return;
    }
    std::lock_guard<std::mutex> lock(m_mutex);
    m_endTime = std::chrono::high_resolution_clock::now();
    m_isRunning = false;
    m_condition.notify_one();
}

double Clock::getElapsedTime(const TimeUnits& units)
{
    if (m_isRunning)
    {
        stop();
        std::unique_lock<std::mutex> lock(m_mutex);
        m_condition.wait(lock, [this] { return !m_isRunning; });
    }

    std::chrono::duration<double> elapsedTime = m_endTime - m_startTime;

    switch (units)
    {
        case TimeUnits::SECONDS:
            return elapsedTime.count();
        case TimeUnits::MILLISECONDS:
            return std::chrono::duration_cast<std::chrono::milliseconds>(elapsedTime).count();
        case TimeUnits::MICROSECONDS:
            return std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime).count();
        case TimeUnits::NANOSECONDS:
            return std::chrono::duration_cast<std::chrono::nanoseconds>(elapsedTime).count();
        default:
            return elapsedTime.count();
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
    std::strftime(buffer.data(), sizeof(buffer), format.data(), std::gmtime(&nowTimeT));
    return std::string(buffer.data());
}

std::string Clock::getLocalTimeStr(const std::string_view format) const
{
    auto nowTimeT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
    auto localTimeT = std::localtime(&nowTimeT);
    std::array<char, 80> buffer;
    std::strftime(buffer.data(), sizeof(buffer), format.data(), localTimeT);
    return std::string(buffer.data());
}

