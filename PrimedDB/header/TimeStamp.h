#pragma once
#include "Defs.h"
namespace liao::Util {
    enum class TimeType:char
    {
        Year,
        Month,
        Day,
        Hour,
        Minute,
        Second
    };
    constexpr int TIMESTAMP_LENGTH = 20;
    DYNAMIC
    concept Concept_TimeTypes = requires
    {
        std::same_as <T, std::chrono::hours>
            || std::same_as < T, std::chrono::minutes>
            || std::same_as < T, std::chrono::seconds>
            || std::same_as < T, std::chrono::milliseconds>
            || std::same_as < T, std::chrono::microseconds>
            || std::same_as < T, std::chrono::nanoseconds>;
    };
	class TimeStamp {
	public:
        using TimePoint = std::chrono::system_clock::time_point;
        using SystemClock = std::chrono::system_clock;
    private:
        std::string m_literal;
        TimePoint m_timestamp;
        mutable ShareMutex m_mutex;

        void setTime(TimePoint& time);
        void convertToString(const TimePoint& time);
        void initializeString();
        static std::string toYear(std::string& date);
        static std::string toMonth(std::string& date);
        static std::string toDay(std::string& date);
        static std::string toHour(std::string& time);
        static std::string toMinute(std::string& time);
        static std::string toSecond(std::string& time);

    public:
        TimeStamp();
        TimeStamp(TimePoint now);
        TimeStamp(const TimeStamp& obj);
        TimeStamp(TimeStamp&& obj) noexcept;
        TimeStamp(long long);
        const std::string& getString();
        std::string&& moveString();
        long long getTimestamp() const;
        void reset(TimePoint& now);
        void clear();
        bool isEmpty() const;
        bool compare(const TimeStamp& obj) const;
        bool isEarlier(const TimeStamp& obj)const;
        bool isLater(const TimeStamp& obj)const;
        std::string get(TimeType)const;
        std::string getDate() const;
        long long distance(const TimeStamp& obj) const;
        DYNAMICCON(Concept_TimeTypes)
        void add(T timeTypes)
        {
            {
                WriteLock lock(m_mutex);
                m_timestamp += timeTypes;
            }
            reset(m_timestamp);
        }
        DYNAMICCON(Concept_TimeTypes)
	    void minus(T timeTypes)
        {
            WriteLock lock(m_mutex);
            m_timestamp -= timeTypes;
            reset(m_timestamp);
        }
        static TimeStamp Now();
        operator std::string() const;
        static TimePoint SystemTime();
        TimeStamp& operator=(TimeStamp&&);
        ~TimeStamp() = default;
    };
}