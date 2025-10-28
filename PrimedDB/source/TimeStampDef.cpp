#include "TimeStamp.h"
USESTD;
USECRPT;
namespace liao::Util
{
    TimeStamp::TimePoint TimeStamp::SystemTime()
    {
        auto timeZone = chrono::current_zone();
        auto now = SystemClock::now();
        const auto info = timeZone->get_info(now);
        const auto offset_seconds = info.offset;
        return now + offset_seconds;
    }
    TimeStamp TimeStamp::Now()
    {
        return TimeStamp(SystemTime());
    }
    void TimeStamp::setTime(TimePoint& time)
    {
        WriteLock lock(m_mutex);
        m_timestamp =time;
    }
    void TimeStamp::initializeString()
    {
        WriteLock lock(m_mutex);
        if (m_literal.capacity() < TIMESTAMP_LENGTH)
            m_literal.reserve(TIMESTAMP_LENGTH);
    }
    void TimeStamp::convertToString(const TimePoint& time)
    {
        initializeString();
        WriteLock lock(m_mutex);
        m_literal = format("{:%Y-%m-%d %H:%M:%S}", time);
    }
    std::string TimeStamp::toYear(std::string& date)
    {
        return date.substr(0, date.find("-"));
    }
    std::string TimeStamp::toMonth(std::string& date)
    {
        return date.substr(date.find("-") + 1, date.find_last_of("-") - date.find("-") - 1);
    }
    std::string TimeStamp::toDay(std::string& date)
    {
        return date.substr(date.find_last_of("-") + 1);
    }
    std::string TimeStamp::toHour(std::string& time)
    {
        return time.substr(0, time.find(":"));
    }
    std::string TimeStamp::toMinute(std::string& time)
    {
        return time.substr(time.find(":") + 1, time.find_last_of(":") - time.find(":") - 1);
    }
    std::string TimeStamp::toSecond(std::string& time)
    {
        return time.substr(time.find_last_of(":") + 1);
    }
    std::string TimeStamp::getDate() const
    {
        return m_literal.substr(0, m_literal.find(" "));
    }
    std::string TimeStamp::get(TimeType type)const
    {
        if (isEmpty())
            return m_literal;
        if (type < TimeType::Hour)
        {
            string date = m_literal.substr(0,m_literal.find(" "));
            if (type == TimeType::Year)
            {
                return toYear(date);
            }
            else if (type == TimeType::Month)
            {
                return toMonth(date);
            }
            else if (type == TimeType::Day)
            {
                return toDay(date);
            }
        }
        else
        {
            string time = m_literal.substr(m_literal.find(" ")+1, m_literal.size() - m_literal.find(" "));
            if (type == TimeType::Hour)
            {
                return toHour(time);
            }
            else if (type == TimeType::Minute)
            {
                return toMinute(time);
            }
            else
                return toSecond(time);
        }
        return "";
    }
    TimeStamp::TimeStamp()
    {
        initializeString();
    }
    TimeStamp::TimeStamp(TimePoint now)
        : m_timestamp(now)
    {
        reset(m_timestamp);
    }
    TimeStamp::TimeStamp(const TimeStamp& obj)
        :m_timestamp(obj.m_timestamp),m_literal(obj.m_literal)
    {}
    TimeStamp::TimeStamp(TimeStamp&& obj) noexcept
        :m_literal(std::move(obj.m_literal)),m_timestamp(obj.m_timestamp)
    {}
    TimeStamp::TimeStamp(long long seconds)
        : m_timestamp(std::chrono::time_point<std::chrono::system_clock>(
            std::chrono::seconds(seconds)
        ))
    {
        reset(m_timestamp);
    }
    const std::string& TimeStamp::getString()
    {
        if (m_literal.empty())
        {
            reset(m_timestamp);
        }
        ReadLock lock(m_mutex);
        return m_literal;
    }
    std::string&& TimeStamp::moveString()
    {
        WriteLock lock(m_mutex);
        return std::move(m_literal);
    }
    long long TimeStamp::getTimestamp() const
    {
        ReadLock lock(m_mutex);
        return std::chrono::duration_cast<std::chrono::seconds>(
            m_timestamp.time_since_epoch()
        ).count();
    }
    void TimeStamp::reset(TimePoint& now)
    {
        setTime(now);
        convertToString(now);
    }
    void TimeStamp::clear()
    {
        WriteLock lock(m_mutex);
        m_literal.clear();
        m_timestamp = TimePoint();
    }
    bool TimeStamp::isEmpty() const
    {
        ReadLock lock(m_mutex);
        return m_literal.empty()||m_timestamp.time_since_epoch().count() == 0;
    }
    bool TimeStamp::compare(const TimeStamp& obj) const
    {
        return distance(obj)==0;
    }
    bool TimeStamp::isEarlier(const TimeStamp& obj) const
    {
        return distance(obj)<0;
    }
    bool TimeStamp::isLater(const TimeStamp& obj) const
    {
        return distance(obj)>0;
    }
    long long TimeStamp::distance(const TimeStamp& obj) const
    {
        chrono::duration<long long> duration;
        {
            ReadLock lock(m_mutex);
            auto interval = m_timestamp - obj.m_timestamp;
            duration = duration_cast<chrono::seconds>(interval);
        }
        return duration.count();
    }
    TimeStamp::operator std::string() const
    {
        return m_literal;
    }
    TimeStamp& TimeStamp::operator=(TimeStamp&& move)
    {
        m_literal = std::move(move.m_literal);
        m_timestamp = move.m_timestamp;
        return *this;
    }
}