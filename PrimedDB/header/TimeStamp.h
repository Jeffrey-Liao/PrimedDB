#pragma once
#include "Defs.h"
namespace liao::Util { 
    constexpr int TIMESTAMP_LENGTH = 20;
	class TimeStamp {
	public:
        using TimePoint = std::chrono::system_clock::time_point;
        using SystemClock = std::chrono::system_clock;
    private:
        std::string m_literal;
        long long m_timestamp;
        ShareMutex m_mutex;

        void setTime(const TimePoint& time);
        void convertToString(const TimePoint& time);
        void initializeString();
    public:
        TimeStamp();
        TimeStamp(const TimePoint);
        TimeStamp(const TimeStamp& obj);
        TimeStamp(TimeStamp&& obj) noexcept;
        const std::string& getString() const;
        std::string&& moveString();
        long long getTimestamp() const;
        void reset(const TimePoint now = SystemClock::now());
        void clear();
        bool isEmpty() const;
        bool compare(const TimeStamp& obj) const;
        bool isEarlier(const TimeStamp& obj)const;
        bool isLater(const TimeStamp& obj)const;
        long long distance(const TimeStamp& obj) const; 

        ~TimeStamp() = default;
    };
}