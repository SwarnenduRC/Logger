#ifndef CLOCK_HPP
#define CLOCK_HPP

#include <chrono>
#include <thread>
#include <atomic>
#include <string>
#include <mutex>
#include <condition_variable>

using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

enum class TimeUnits
{
    SECONDS,
    MILLISECONDS,
    MICROSECONDS,
    NANOSECONDS
};

class Clock
{
    public:
        void start();
        void stop();
        double getElapsedTime(const TimeUnits& units = TimeUnits::SECONDS);
        inline bool isRunning() const { return m_isRunning; };
                
        std::string getGmtTime() const;
        std::string getLocalTime() const;
        std::string getGmtTimeStr(const std::string_view format = "%d/%m/%Y %H:%M:%S") const;
        std::string getLocalTimeStr(const std::string_view format = "%d/%m/%Y %H:%M:%S") const;

        std::string getDayOfWeek() const;
        std::string getMonth() const;
        std::string getYear() const;
        std::tuple<int, int, int> getTimeOfTheDay() const;
        std::tuple<int, int, int> getGmtTimeOfTheDay() const;
        
    private:
        TimePoint m_startTime;
        TimePoint m_endTime;

        std::atomic_bool m_isRunning;
        std::thread::id m_threadId;
        std::mutex m_mutex;
        std::condition_variable m_condition;
};

#endif // CLOCK_HPP
